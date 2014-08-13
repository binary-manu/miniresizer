#include <math.h>
#include <FL/fl_draw.H>
#include <FL/Fl_File_Chooser.H>
#include <string>
#include <iostream>
#include "miniresizer.hpp"	// For the GUI
#include "miniresizer_core.hpp"
#include "version.h"


ResizeWindow& ResizeWindow::SetInputHeight(FrameSize inputHeight) {
	if (inputHeight <= 0) {
		throw InvalidValueException("Cannot set input frame height to non-positive value");
	}
	mInputHeight->value(inputHeight);
	evaluate(false);
	return *this;
}

FrameSize ResizeWindow::GetInputHeight() const {
	return mInputHeight->value();
}

ResizeWindow& ResizeWindow::SetInputWidth(FrameSize inputWidth) {
	if (inputWidth <= 0) {
		throw InvalidValueException("Cannot set input frame height to non-positive value");
	}
	this->mInputWidth->value(inputWidth);
	evaluate(false);
	return *this;
}

FrameSize ResizeWindow::GetInputWidth() const {
	return mInputWidth->value();
}

ResizeWindow& ResizeWindow::SetInputDar(Ratio dar) {
	if (dar <= 0 || !isfinite(dar)) {
		throw InvalidValueException("Cannot set input frame DAR to non-positive or non-finite value");
	}
	mInputDar->value(dar);
	evaluate(false);
	return *this;
}

Ratio ResizeWindow::GetDar() const {
	return mDar->value();
}

ResizeWindow::OnResizeListener* ResizeWindow::GetOnResizeListener() const {
	return mOnResize;
}

ResizeWindow& ResizeWindow::SetOnResizeListener(OnResizeListener *cb) {
	mOnResize = cb;
	return *this;
}

const Ratio ResizeWindow::mDarConstants[ResizeWindow::DARCHOICE_SIZE] = {
	1.0,		// Unused
	1.0,		// Unused
	16.0 / 9.0,
	4.0 / 3.0
};

Ratio ResizeWindow::GetPar() const {
	return mPar->value();
}

Ratio ResizeWindow::GetSar() const {
	return mInputSar->value();
}

FrameSize ResizeWindow::GetCropLeft() const {
	return mCropLeft->value();
}

FrameSize ResizeWindow::GetCropRight() const {
	return mCropRight->value();
}

FrameSize ResizeWindow::GetCropTop() const {
	return mCropTop->value();
}

FrameSize ResizeWindow::GetCropBottom() const {
	return mCropBottom->value();
}

FrameSize ResizeWindow::GetCroppedWidth() const {
	return mCroppedWidth->value();
}

FrameSize ResizeWindow::GetCroppedHeight() const {
	return mCroppedHeight->value();
}

FrameSize ResizeWindow::GetResizedWidth() const {
	return mResizedWidth->value();
}

FrameSize ResizeWindow::GetResizedHeight() const {
	return mResizedHeight->value();
}

ResizeWindow::~ResizeWindow() {
}

ResizeWindow::ResizeWindow():
	mOnResize(0)
{
	label((std::string(label()) + " " + VERSION_STRING).c_str());
	
	callback(handleClose, this);
	mDarChoice->add("As input");
	mDarChoice->add("Custom");
	mDarChoice->add("16:9");
    mDarChoice->add("4:3");
	mDarChoice->value(0);
	
	mPreviewBorder->add("No border");
	mPreviewBorder->add("Black");
	mPreviewBorder->add("White");
	mPreviewBorder->value(0);
	
	mDarChoice->callback(&handleDARSelection, this);
	mPreviewBorder->callback(&genericHandler, this);
	mZoomMultiplier->callback(&genericHandler, this);
	mZoomIn->callback(&genericHandler, this);
	mDar->callback(&handleDARTyping, this);
	mCropAlign->callback(&handleCropAlignChange, this);
	mCropTop->callback(&handleCropTopChange, this);
	mCropRight->callback(&handleCropRightChange, this);
	mCropBottom->callback(&handleCropBottomChange, this);
	mCropLeft->callback(&handleCropLeftChange, this);
	mTargetWidth->callback(&handleTargetWidthChange, this);
	
	// Calculate & display resize parameters based on default values
	// typed in the UI definition file
	evaluate(false);
};

FrameSize ResizeWindow::cropConstaints(FrameSize crop, FrameSize limit, FrameSize step) {
	if (limit <= 0 || step <= 0 || crop < 0) {
		throw InvalidValueException("It's impossible to satisfy crop constraints with these values");
	}
	
	// Find the largest value smaller than 'crop' which is also:
	// - < 'limit'
	// - evenly divisible by 'step'
	if (crop >= limit) {
		crop = limit - 1;
	}
	return crop / step * step;
}
	
void ResizeWindow::enforceCropConstaints(Fl_Spinner *w, FrameSize limit, FrameSize step) {
/*	if (w->value() < w->minimum()) {
		w->value(w->minimum());
	}
	if (w->value() > w->maximum()) {
		w->value(w->maximum());
	}
*/	
	
	// The UI control can return a negative crop value because I've used a negative
	// minimum to avoid the unwanted wrap-around from 0 to the maximum
	FrameSize crop = w->value();
	if (crop < 0) {
		crop = 0;
	}
	w->value(cropConstaints(crop, limit, step));
}

	
void ResizeWindow::handleDARSelection(Fl_Widget *dar, void *_p) {
	ResizeWindow *p = (static_cast<ResizeWindow*>(_p));
	p->evaluate();
}
	
void ResizeWindow::handleCropAlignChange(Fl_Widget *dar, void *_p) {
	ResizeWindow *p = (static_cast<ResizeWindow*>(_p));
	
	FrameSize align = FrameSize(p->mCropAlign->value());
	
/*	if (align < p->mCropAlign->minimum()) {
		align = p->mCropAlign->minimum();
	}
	if (align > p->mCropAlign->maximum()) {
		align = p->mCropAlign->maximum();
	}
	p->mCropAlign->value(align);
*/		
	enforceCropConstaints(
		p->mCropLeft,
		p->mInputWidth->value() - p->mCropRight->value(),
		align
	);
	p->mCropLeft->step(align);
		
	enforceCropConstaints(
		p->mCropRight,
		p->mInputWidth->value() - p->mCropLeft->value(),
		align
	);
	p->mCropRight->step(align);
	
	enforceCropConstaints(
		p->mCropTop,
		p->mInputHeight->value() - p->mCropBottom->value(),
		align
	);
	p->mCropTop->step(align);
	
	enforceCropConstaints(
		p->mCropBottom,
		p->mInputHeight->value() - p->mCropTop->value(),
		align
	);
	p->mCropBottom->step(align);
	
	p->evaluate();
}
	
void ResizeWindow::handleCropLeftChange(Fl_Widget *w, void *_p) {
	ResizeWindow *p = (static_cast<ResizeWindow*>(_p));
	
	enforceCropConstaints(
		p->mCropLeft,
		p->mInputWidth->value() - p->mCropRight->value(),
		p->mCropAlign->value()
	);
	p->evaluate();
}

void ResizeWindow::handleCropRightChange(Fl_Widget *w, void *_p) {
	ResizeWindow *p = (static_cast<ResizeWindow*>(_p));
	
	enforceCropConstaints(
		p->mCropRight,
		p->mInputWidth->value() - p->mCropLeft->value(),
		p->mCropAlign->value()
	);
	p->evaluate();
}
	
void ResizeWindow::handleCropTopChange(Fl_Widget *w, void *_p) {
	ResizeWindow *p = (static_cast<ResizeWindow*>(_p));
	
	enforceCropConstaints(
		p->mCropTop,
		p->mInputHeight->value() - p->mCropBottom->value(),
		p->mCropAlign->value()
	);
	p->evaluate();
}
	
void ResizeWindow::handleCropBottomChange(Fl_Widget *w, void *_p) {
	ResizeWindow *p = (static_cast<ResizeWindow*>(_p));
	
	enforceCropConstaints(
		p->mCropBottom,
		p->mInputHeight->value() - p->mCropTop->value(),
		p->mCropAlign->value()
	);
	p->evaluate();
}
	
void ResizeWindow::handleDARTyping(Fl_Widget *w, void *_p) {
	ResizeWindow *p = (static_cast<ResizeWindow*>(_p));
	Ratio dar = p->mDar->value();
	if (dar < p->mDar->minimum() || dar > p->mDar->maximum()) {
		dar = 1.0;
		p->mDar->value(dar);
	}
	p->evaluate();
}
	
void ResizeWindow::handleTargetWidthChange(Fl_Widget *w, void *_p) {
	ResizeWindow *p = (static_cast<ResizeWindow*>(_p));
/*	if (p->mTargetWidth->value() < p->mTargetWidth->minimum()) {
		p->mTargetWidth->value(p->mTargetWidth->minimum());
	}
	if (p->mTargetWidth->value() > p->mTargetWidth->maximum()) {
		p->mTargetWidth->value(p->mTargetWidth->maximum());
	}
*/	
	if (p->mTargetWidth->value() < p->mTargetWidth->step()) {
		p->mTargetWidth->value(p->mTargetWidth->step());
	}
	p->evaluate();
}

void ResizeWindow::genericHandler(Fl_Widget *w, void *_p) {
	ResizeWindow *p = (static_cast<ResizeWindow*>(_p));
	p->evaluate();
}

void ResizeWindow::handleClose(Fl_Widget *w, void *_p) {
	exit(0);
}

FrameSize ResizeWindow::nearestInteger(Ratio d) {
	d = abs(d);
	return (d - floor(d) <= 0.5) ? floor(d) : ceil(d);
}
	
FrameSize ResizeWindow::nearestMultiple(FrameSize i, FrameSize step) {
	FrameSize min = i / step * step;
	FrameSize max = min + step;
	Ratio mid = min + Ratio(step) / 2;
	return (i > mid) ? max : min;
}
	
void ResizeWindow::evaluate(bool doCallback) {
	// Calculate SAR
	mInputSar->value(mInputWidth->value() / mInputHeight->value());
	
	// Recalculate crop
	const FrameSize cropW = mInputWidth->value() - mCropLeft->value() - mCropRight->value();
	const FrameSize cropH = mInputHeight->value() - mCropTop->value() - mCropBottom->value();
	mCroppedWidth->value(cropW);
	mCroppedHeight->value(cropH);

	if (mDarChoice->value() == DARCHOICE_INDEX_CUSTOM) {
		mDar->activate();
	} else if (mDarChoice->value() == DARCHOICE_INDEX_ASINPUT) {
		mDar->deactivate();
		mDar->value(mInputDar->value());
	} else {
		mDar->deactivate();
		mDar->value(mDarConstants[mDarChoice->value()]);
	}

	// Calculate PAR
	Ratio par = mDar->value() / mInputSar->value();
	mPar->value(par);

	double scaled_h = cropH / par;
	FrameSize target_h = nearestInteger(scaled_h * mTargetWidth->value() / cropW);
	FrameSize target_h_mult16 = nearestMultiple(target_h, 16);
	if (target_h <= 0 || target_h_mult16 <= 0) {
		target_h = target_h_mult16 = 1;
	}
	mResizedWidth->value(mTargetWidth->value());
	mResizedHeight->value(target_h_mult16);
	mResizeError->value((Ratio(target_h_mult16) - target_h) / target_h);

	if (mOnResize && doCallback) {
		mOnResize->Callback(this);
	}
}

void ResizeWindow::TriggerUpdate() {
	evaluate(true);
}

Color ResizeWindow::GetBorderColor() const {
	static Color palette[] = {
		fl_rgb_color(0, 0, 0), 
		fl_rgb_color(0, 0, 0),
		fl_rgb_color(255, 255, 255)
	};
	return palette[mPreviewBorder->value()];
}

bool ResizeWindow::HasBorder() const {
	return mPreviewBorder->value();
}

FrameSize ResizeWindow::GetZoom() const {
	return FrameSize(mZoomMultiplier->value());
}

bool ResizeWindow::IsZoomEnlarging() const {
	return mZoomIn->value();
}

ResizeWindow& ResizeWindow::SetTargetWidth(FrameSize width) {
	mTargetWidth->value(width);
	return *this;
}




PreviewWindow::PreviewWindow():
	mOnSeek(0),
	mPictureW(0),
	mPictureH(0),
	mPictureData(0),
	mHasBorder(false),
	mBorderColor(FL_BLACK)
{
	callback(handleClose, this);
	mPosition->callback(handleSlider, this);
	evaluate(false);
}

PreviewWindow::~PreviewWindow() {
}

Position PreviewWindow::GetGranularity() {
	return mPosition->maximum() + 1;
}

Position PreviewWindow::GetPosition() {
	return mPosition->value();
}

PreviewWindow& PreviewWindow::SetOnSeekListener(SeekListener *onSeek) {
	mOnSeek = onSeek;
	return *this;
}

PreviewWindow::SeekListener* PreviewWindow::GetOnSeekListener() {
	return mOnSeek;
}

void PreviewWindow::TriggerUpdate() {
	evaluate(true);
}

void PreviewWindow::handleSlider(Fl_Widget* w, void* _p) {
	PreviewWindow * const p = static_cast<PreviewWindow*>(_p);
	p->evaluate(true);
}

void PreviewWindow::handleClose(Fl_Widget* w, void* _p) {
	// Do not close
	return;
}

void PreviewWindow::evaluate(bool doCallback) {
	if (mOnSeek && doCallback) {
		mOnSeek->Callback(this);
	}
}

void PreviewWindow::SetBorderColor(Color c) {
	mHasBorder = true;
	mBorderColor = c;
}

void PreviewWindow::ClearBorder() {
	mHasBorder = false;
}

void PreviewWindow::DrawPicture(const uint8_t* rgb, FrameSize w_, FrameSize h_) {
	if (w_ <= 0 || h_ <= 0) {
		throw InvalidValueException("Image cannot have non-positive sizes");
	}
	mPictureData.resize(w_ * h_ * 3);
	memcpy(&mPictureData[0], rgb, mPictureData.size());
	mPictureW = w_;
	mPictureH = h_;
	// Size the window to what is needed to contain the input image and the
	// slider at the bottom, plus the optional border.
	// Then make sure that it cannot be resized by the user.
	w_ += (mHasBorder) ? BORDER_SIZE * 2 : 0;
	h_ += (mHasBorder) ? BORDER_SIZE * 2 : 0;
	size(w_, h_ + mContainer->h());
	size_range(w(), h(), w(), h());
	redraw();
}

void PreviewWindow::draw() {
	const int d = damage();
	int x = 0, y = 0;
	if (mPictureData.size() > 0 && ((d & FL_DAMAGE_ALL) || (d & FL_DAMAGE_EXPOSE))) {
		if (mHasBorder) {
			int r = mBorderColor / (256 * 256 * 256) % 256;
			int g = mBorderColor / (256 * 256) % 256;
			int b = mBorderColor / (256) % 256;
			fl_rectf(0, 0, BORDER_SIZE, BORDER_SIZE * 2 + mPictureH, r, g, b); // Left border
			fl_rectf(w() - BORDER_SIZE, 0, BORDER_SIZE, BORDER_SIZE * 2 + mPictureH, r, g, b); // Right border
			fl_rectf(0, 0, w(), BORDER_SIZE, r, g, b); // Top border
			fl_rectf(0, BORDER_SIZE + mPictureH, w(), BORDER_SIZE, r, g, b); // Bottom border
			x = y = BORDER_SIZE;
		}
		fl_draw_image(&mPictureData[0], x, y, mPictureW, mPictureH);
	}
	draw_children();
}



RGBFrameReader::RGBFrameReader(const char* filename):
		mFormatCtx(0),
		mVideoStream(0),
		mScaler(0),
		mFrame(0)
{
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

		if (avcodec_open2(mFormatCtx->streams[videoStream]->codec,
				avcodec_find_decoder(mFormatCtx->streams[videoStream]->codec->codec_id), 0) < 0) {
			throw AVException((std::string("Unable to open ") + filename).c_str());
		}
		mVideoStream = mFormatCtx->streams[videoStream];
		if (mVideoStream->codec->height <= 0 || mVideoStream->codec->width <= 0) {
			throw AVException("Codec does not provide frame sizes");
		}

		mScaler = sws_getCachedContext(
			0,
			mVideoStream->codec->width, mVideoStream->codec->height, mVideoStream->codec->pix_fmt,
			mVideoStream->codec->width, mVideoStream->codec->height, AV_PIX_FMT_RGB24,
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
	if (mVideoStream) {
		avcodec_close(mVideoStream->codec);
	}
	if (mFormatCtx) {
		avformat_close_input(&mFormatCtx);
	}
}

void RGBFrameReader::SeekToFrame(Ratio where) {
	DecodeResult dr;
	bool seekFailed = true;;
	int64_t seekTo;
	
	do {
		if (mVideoStream->duration > 0) {
			seekTo = mVideoStream->duration * where;
			seekFailed = av_seek_frame(mFormatCtx, mVideoStream->index, seekTo, AVSEEK_FLAG_BACKWARD) < 0;
		}
		if (seekFailed) {
			uint64_t seekTo = avio_size(mFormatCtx->pb) * where;
			seekFailed = av_seek_frame(mFormatCtx, mVideoStream->index, seekTo, AVSEEK_FLAG_BACKWARD | AVSEEK_FLAG_BYTE) < 0;
		}
		if (seekFailed) {
			throw AVException("Cannot seek the media file");
		}
		avcodec_flush_buffers(mVideoStream->codec);
		dr = decodeFrame();
	} while (where -= 0.001, dr == DR_EOF && where >= 0);
	if (dr == DR_EOF) {
		// Give up
		throw AVException("Cannot decode frame");
	}
}

FrameSize RGBFrameReader::GetFrameWidth() const {
	return mVideoStream->codec->width;
}

FrameSize RGBFrameReader::GetFrameHeight() const {
	return mVideoStream->codec->height;
}

Ratio RGBFrameReader::GetFrameDAR() const {
	if (mVideoStream->codec->sample_aspect_ratio.num != 0 && mVideoStream->codec->sample_aspect_ratio.den != 0) {
		// Derive the DAR from SAR and PAR suggested by the compressor
		return Ratio(mVideoStream->codec->sample_aspect_ratio.num) * mVideoStream->codec->width /
			mVideoStream->codec->sample_aspect_ratio.den / mVideoStream->codec->height;
	} else {
		// Just return the SAR so that we get a square pixel
		return Ratio(mVideoStream->codec->width) / mVideoStream->codec->height;
	}
}

RGBFrameReader::DecodeResult RGBFrameReader::decodeFrame() {	
	AVPacket pkt;
	av_init_packet(&pkt);
	pkt.data = 0;
	pkt.size = 0;
	
	do {
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
	} while (true);
	
	// Got a frame
	uint8_t *rgbData[4];
	int rgbLinesize[4];
	
	if (av_image_alloc(rgbData, rgbLinesize, mVideoStream->codec->width,
			mVideoStream->codec->height, AV_PIX_FMT_RGB24, 1) < 0) {
		throw ResourceAllocationException("Cannot convert frame to RGB");
	}

	if (sws_scale(mScaler, (const uint8_t **)mFrame->data, mFrame->linesize, 0,
			mVideoStream->codec->height, rgbData, rgbLinesize) < 0) {
		av_freep(&rgbData[0]);
		throw AVException("AV frame scale error");
	}
	
	mFrameData.resize(rgbLinesize[0] * mVideoStream->codec->height);
	memcpy(&mFrameData[0], rgbData[0], mFrameData.size());
	av_freep(&rgbData[0]);
	
	return DR_OK;
}

const uint8_t *RGBFrameReader::GetFrameData() const {
	return &mFrameData[0];
}

class Controller : private PreviewWindow::SeekListener, private ResizeWindow::OnResizeListener {
public:
	Controller(RGBFrameReader *rgb, ResizeWindow *resize, PreviewWindow *preview) {
		mRGB = rgb;
		mResize = resize;
		mPreview = preview;
		if (!mRGB || !mResize || ! mPreview) {
			throw InvalidValueException("Program logic object instatiated without some required component");
		}
		mResize->SetOnResizeListener(this);
		mPreview->SetOnSeekListener(this);
		const FrameSize w = mRGB->GetFrameWidth();
		mResize->SetTargetWidth((w) ? w : 1);
		mPreview->TriggerUpdate();
	}
	
	~Controller() {
		mResize->SetOnResizeListener(0);
		mPreview->SetOnSeekListener(0);
	}
	
	void Callback(PreviewWindow *) {
		// Seek to the target frame. Then use frames size and DAR (if available)
		// from the codec to set input values in the resize window
		mRGB->SeekToFrame(Ratio(mPreview->GetPosition()) / mPreview->GetGranularity());
		mResize->SetInputHeight(mRGB->GetFrameHeight());
		mResize->SetInputWidth(mRGB->GetFrameWidth());
		mResize->SetInputDar(mRGB->GetFrameDAR());
		draw();
	}
	
	void Callback(ResizeWindow *) {
		draw();
	}
	
	void draw() {
		SwsContext *scaler = 0;
		uint8_t *outScale[4] = {0};
		int outLines[4] = {0};
		
		try {
			const Ratio zoomRatio = (mResize->IsZoomEnlarging()) ? mResize->GetZoom() : (1.0 / mResize->GetZoom());
			const FrameSize w = ceil(mResize->GetResizedWidth() * zoomRatio);
			const FrameSize h = ceil(mResize->GetResizedHeight() * zoomRatio);
			const FrameSize croppedW = mResize->GetCroppedWidth();
			const FrameSize croppedH = mResize->GetCroppedHeight();

			scaler = sws_getCachedContext(
				0,
				croppedW, croppedH, AV_PIX_FMT_RGB24,
				w, h, AV_PIX_FMT_RGB24,
				SWS_FAST_BILINEAR, 0, 0, 0
			);
			if (!scaler) {
				throw ResourceAllocationException("Cannot allocate SwsContext");
			}

			std::vector<uint8_t> scaled;
			scaled.resize(croppedW * croppedH * 3);
			uint8_t *inScale[4] = {0};
			inScale[0] = &scaled[0];
			int inLines[4] = {0};
			inLines[0] = croppedW * 3;


			if (av_image_alloc(outScale, outLines, w, h, AV_PIX_FMT_RGB24, 1) < 0) {
				throw ResourceAllocationException("Cannot allocate image");
			}
			
			// Crop
			const uint8_t *src = mRGB->GetFrameData();
			uint8_t *dst = inScale[0];
			
			// Skip cropped top lines
			src += mRGB->GetFrameWidth() * 3 * mResize->GetCropTop();
			
			const FrameSize cropLeft = mResize->GetCropLeft() * 3;
			const FrameSize lineSize = mRGB->GetFrameWidth() * 3;
			for (size_t i = croppedH; i > 0; i--) {
				memcpy(dst, src + cropLeft, inLines[0]);
				dst += inLines[0];
				src += lineSize;
			}
			if (sws_scale(scaler, (const uint8_t**)inScale, inLines, 0, croppedH, outScale, outLines) < 0) {
				throw AVException("Cannot scale image");
			}
			if (mResize->HasBorder()) {
				mPreview->SetBorderColor(mResize->GetBorderColor());
			} else {
				mPreview->ClearBorder();
			}
			mPreview->DrawPicture(outScale[0], w, h);
			sws_freeContext(scaler);
			scaler = 0;
			av_freep(&outScale[0]);
		} catch (... ) {
			if (scaler) {
				sws_freeContext(scaler);
			}
			av_freep(outScale[0]);
		}
	}
	
private:
	RGBFrameReader *mRGB;
	ResizeWindow *mResize;
	PreviewWindow *mPreview;
};

int main(int argc, char **argv) {
	std::cout << APP_VERSIONED_NAME << std::endl;
	
	av_register_all();
	Fl::visual(FL_RGB);
	
	const char* filename = fl_file_chooser("Open a video file", "*", "", 0);
	if (!filename) {
		exit(0);
	}
	
	try {
		ResizeWindow *resize = new ResizeWindow;
		PreviewWindow *preview = new PreviewWindow;
		resize->show();
		preview->show();
		RGBFrameReader *rgb = new RGBFrameReader(filename);
		Controller logic(rgb, resize, preview);
		return Fl::run();
	} catch (std::exception &e) {
		fl_message(e.what());
		return -1;
	}
}