#include <cmath>

#include "config.h"

#include "RGBFrameReader.hpp"

RGBFrameReader::RGBFrameReader(const char* filename):
		mFormatCtx(0),
		mCodecCtx(0),
		mVideoStream(0),
		mScaler(0),
		mFrame(0)
{
#ifndef FFMPEG_USE_NEW_AVCODEC_API
	av_register_all();
#endif
 
	try {
		avformat_open_input(&mFormatCtx, filename, 0, 0);
		if (mFormatCtx == 0) {
			throw AVException((std::string("Unable to open ") + filename).c_str());
		}

		if (avformat_find_stream_info(mFormatCtx, 0) < 0) {
			throw AVException((std::string("Unable to open ") + filename).c_str());
		}

		int videoStream = av_find_best_stream(mFormatCtx, AVMEDIA_TYPE_VIDEO, -1, -1, 0, 0);
		if (videoStream < 0) {
			throw AVException((std::string("Unable to open ") + filename).c_str());
		}
		mVideoStream = mFormatCtx->streams[videoStream];

#ifdef FFMPEG_USE_NEW_AVCODEC_API
		const AVCodecID codecId = mVideoStream->codecpar->codec_id;
		mCodecCtx = avcodec_alloc_context3(NULL);
		if (mCodecCtx == 0 ||
		        avcodec_parameters_to_context(mCodecCtx, mVideoStream->codecpar) < 0) {
			throw AVException((std::string("Unable to open ") + filename).c_str());
		}
#else
		const AVCodecID codecId = mVideoStream->codec->codec_id;
		mCodecCtx = mVideoStream->codec;
#endif

		if (avcodec_open2(mCodecCtx, avcodec_find_decoder(codecId), 0) < 0) {
			throw AVException((std::string("Unable to open ") + filename).c_str());
		}
		if (mCodecCtx->height <= 0 || mCodecCtx->width <= 0) {
			throw AVException("Codec does not provide frame sizes");
		}

		mScaler = sws_getCachedContext(
			0,
			mCodecCtx->width, mCodecCtx->height, mCodecCtx->pix_fmt,
			mCodecCtx->width, mCodecCtx->height, AV_PIX_FMT_RGB24,
			SWS_FAST_BILINEAR, 0, 0, 0);
		if (!mScaler) {
			throw AVException((std::string("Cannot create AV scaler for ") + filename).c_str());
		}
		
		mFrame = av_frame_alloc();

		DecodeResult dr = decodeFrame();
		if (dr != DR_OK) {
			throw AVException("Unable to decode the first frame");
		}
	} catch (...) {
		close();
		throw;
	}
}


RGBFrameReader::~RGBFrameReader() {
	close();
}

void RGBFrameReader::close() {
	if (mFrame) {
		av_frame_free(&mFrame);
	}
	if (mScaler) {
		sws_freeContext(mScaler);
	}
	if (mCodecCtx) {
#ifdef FFMPEG_USE_NEW_AVCODEC_API
		avcodec_free_context(&mCodecCtx);
#else 
		avcodec_close(mCodecCtx);
#endif
	}
	if (mFormatCtx) {
		avformat_close_input(&mFormatCtx);
	}
}

void RGBFrameReader::SeekToFrame(Ratio where) {
	DecodeResult dr;
	bool seekFailed = true;
	int64_t seekTo;
	
	do {
		uint64_t seekTo = avio_size(mFormatCtx->pb) * where;
		seekFailed = av_seek_frame(mFormatCtx, mVideoStream->index, seekTo, AVSEEK_FLAG_BACKWARD | AVSEEK_FLAG_BYTE) < 0;
		if (seekFailed) {
			if (mFormatCtx->duration > 0) {
				seekTo = mFormatCtx->duration * where;
				seekFailed = av_seek_frame(mFormatCtx, -1, seekTo, AVSEEK_FLAG_BACKWARD) < 0;
			}
		}
		if (seekFailed) {
			throw AVException("Cannot seek the media file");
		}
		avcodec_flush_buffers(mCodecCtx);
		dr = decodeFrame();
	} while (where -= 0.001, dr == DR_EOF && where >= 0);
	if (dr == DR_EOF) {
		// Give up
		throw AVException("Cannot decode frame");
	}
}

FrameSize RGBFrameReader::GetFrameWidth() const {
	return mCodecCtx->width;
}

FrameSize RGBFrameReader::GetFrameHeight() const {
	return mCodecCtx->height;
}

Ratio RGBFrameReader::GetFrameDAR() const {
	if (mCodecCtx->sample_aspect_ratio.num != 0 && mCodecCtx->sample_aspect_ratio.den != 0) {
		// Derive the DAR from SAR and PAR suggested by the compressor
		return Ratio(mCodecCtx->sample_aspect_ratio.num) * mCodecCtx->width /
			mCodecCtx->sample_aspect_ratio.den / mCodecCtx->height;
	} else {
		// Just return the SAR so that we get a square pixel
		return Ratio(mCodecCtx->width) / mCodecCtx->height;
	}
}

RGBFrameReader::DecodeResult RGBFrameReader::decodeFrame() {	
	AVPacket pkt;
	av_init_packet(&pkt);
	pkt.data = 0;
	pkt.size = 0;
	
	do {
#ifdef FFMPEG_USE_NEW_AVCODEC_API
		int what = av_read_frame(mFormatCtx, &pkt);
		if (what < 0) {
			if (what == AVERROR_EOF) {
				return DR_EOF;
			}
			throw AVException("AV frame read error");
		}
		if (pkt.stream_index != mVideoStream->index) {
			av_packet_unref(&pkt);
			continue;
		}

		int got;
		what = avcodec_send_packet(mCodecCtx, &pkt);
		av_packet_unref(&pkt);
		if (what < 0) {
			throw AVException("AV frame decode error");
		}
		what = avcodec_receive_frame(mCodecCtx, mFrame);
		if (what == 0) {
			break;
		} else if (what != AVERROR(EAGAIN)) {
			throw AVException("AV frame decode error");
		}
#else
		int what = av_read_frame(mFormatCtx, &pkt);
		if (what < 0) {
			if (what == AVERROR_EOF) {
				return DR_EOF;
			}
			throw AVException("AV frame read error");
		}
		if (pkt.stream_index != mVideoStream->index) {
			av_free_packet(&pkt);
			continue;
		}

		int got;
		what = avcodec_decode_video2(mVideoStream->codec, mFrame, &got, &pkt);
		if (what < 0) {
			av_free_packet(&pkt);
			throw AVException("AV frame decode error");
		}
		av_free_packet(&pkt);
		if (got) {
			break;
		}
#endif
	} while (true);
	
	// Got a frame
	uint8_t *rgbData[4];
	int rgbLinesize[4];
	
	if (av_image_alloc(rgbData, rgbLinesize, mCodecCtx->width,
			mCodecCtx->height, AV_PIX_FMT_RGB24, 1) < 0) {
		throw ResourceAllocationException("Cannot convert frame to RGB");
	}

	if (sws_scale(mScaler, (const uint8_t **)mFrame->data, mFrame->linesize, 0,
			mCodecCtx->height, rgbData, rgbLinesize) < 0) {
		av_freep(&rgbData[0]);
		throw AVException("AV frame scale error");
	}
	
	mFrameData.resize(rgbLinesize[0] * mCodecCtx->height);
	memcpy(&mFrameData[0], rgbData[0], mFrameData.size());
	av_freep(&rgbData[0]);
	
	return DR_OK;
}

const uint8_t *RGBFrameReader::GetFrameData() const {
	return &mFrameData[0];
}
