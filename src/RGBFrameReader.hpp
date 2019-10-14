#ifndef RGB_FRAME_READER_HPP
#define RGB_FRAME_READER_HPP


#include <vector>

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
}

#include "Common.hpp"

class RGBFrameReader {
public:
	RGBFrameReader(const char* filename);
	~RGBFrameReader();
	void SeekToFrame(Ratio where);
	FrameSize GetFrameWidth() const;
	FrameSize GetFrameHeight() const;
	Ratio GetFrameDAR() const;
	const uint8_t *GetFrameData() const;
private:
	enum DecodeResult {
		DR_OK,
		DR_EOF
	};
	
	RGBFrameReader(const RGBFrameReader&);
	RGBFrameReader& operator=(const RGBFrameReader&);
	
	AVFormatContext *mFormatCtx;
	AVCodecContext *mCodecCtx;
	AVStream *mVideoStream;
	SwsContext *mScaler;
	AVFrame *mFrame;
	std::vector<uint8_t> mFrameData;
	
	DecodeResult decodeFrame();
	void close();
};

#endif	/* RGB_FRAME_READER_HPP */
