#include "Controller.hpp"

Controller::Controller(RGBFrameReader *rgb, ResizeWindow *resize, PreviewWindow *preview) {
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

Controller::~Controller() {
    mResize->SetOnResizeListener(0);
    mPreview->SetOnSeekListener(0);
}

void Controller::Callback(PreviewWindow *) {
    // Seek to the target frame. Then use frames size and DAR (if available)
    // from the codec to set input values in the resize window
    mRGB->SeekToFrame(Ratio(mPreview->GetPosition()) / mPreview->GetGranularity());
    mResize->SetInputHeight(mRGB->GetFrameHeight());
    mResize->SetInputWidth(mRGB->GetFrameWidth());
    mResize->SetInputDar(mRGB->GetFrameDAR());
    draw();
}

void Controller::Callback(ResizeWindow *) {
    draw();
}

void Controller::draw() {
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
        throw;
    }
}
