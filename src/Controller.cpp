// Copyright 2020 Emanuele Giacomelli

#include <cmath>
#include <vector>
#include <memory>

#include "Controller.hpp"

Controller::Controller(RGBFrameReader *rgb, ResizeWindow *resize, PreviewWindow *preview) {
    mRGB = rgb;
    mResize = resize;
    mPreview = preview;
    if (!mRGB || !mResize || !mPreview) {
        throw InvalidValueException("Program logic object instatiated without some required component");
    }
    mPreview->show();
    mResize->show();
    mResize->SetOnResizeListener(this);
    mPreview->SetOnSeekListener(this);
    const FrameSize w = mRGB->GetFrameWidth();
    const FrameSize snap = resize->GetWSnap();
    mResize->SetTargetWidth(snapSize(w, snap));
    mPreview->TriggerUpdate();
}

Controller::~Controller() {
    mResize->SetOnResizeListener(nullptr);
    mPreview->SetOnSeekListener(nullptr);
}

void Controller::Callback(PreviewWindow * /* window */) {
    // Seek to the target frame. Then use frames size and DAR (if available)
    // from the codec to set input values in the resize window
    mRGB->SeekToFrame(Ratio(mPreview->GetPosition()) / mPreview->GetGranularity());
    mResize->SetInputHeight(mRGB->GetFrameHeight());
    mResize->SetInputWidth(mRGB->GetFrameWidth());
    mResize->SetInputDar(mRGB->GetFrameDAR());
    draw();
}

void Controller::Callback(ResizeWindow * /* window */) {
    draw();
}

void Controller::Close(ResizeWindow * /* window */) {
    mPreview->hide();
    mResize->hide();
}

void Controller::draw() {
    SwsContext *scaler = nullptr;
    uint8_t *outScale[4] = {nullptr};
    int outLines[4] = {0};

    const Ratio zoomRatio = (mResize->IsZoomEnlarging()) ? mResize->GetZoom() : (1.0 / mResize->GetZoom());
    const FrameSize w = std::ceil(mResize->GetResizedWidth() * zoomRatio);
    const FrameSize h = std::ceil(mResize->GetResizedHeight() * zoomRatio);
    const FrameSize croppedW = mResize->GetCroppedWidth();
    const FrameSize croppedH = mResize->GetCroppedHeight();

    scaler = sws_getCachedContext(
        nullptr,
        croppedW, croppedH, AV_PIX_FMT_RGB24,
        w, h, AV_PIX_FMT_RGB24,
        SWS_FAST_BILINEAR, nullptr, nullptr, nullptr
    );
    if (!scaler) {
        throw ResourceAllocationException("Cannot allocate SwsContext");
    }
    std::unique_ptr<SwsContext, typeof(&sws_freeContext)> freeScaler {scaler, &sws_freeContext};

    std::vector<uint8_t> scaled;
    scaled.resize(croppedW * croppedH * 3);
    uint8_t *inScale[4] = {nullptr};
    inScale[0] = &scaled[0];
    int inLines[4] = {0};
    inLines[0] = croppedW * 3;


    if (av_image_alloc(outScale, outLines, w, h, AV_PIX_FMT_RGB24, 1) < 0) {
        throw ResourceAllocationException("Cannot allocate image");
    }
    std::unique_ptr<uint8_t*, typeof(&av_freep)> freeData {&outScale[0], &av_freep};

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
    if (sws_scale(scaler, inScale, inLines, 0, croppedH, outScale, outLines) < 0) {
        throw AVException("Cannot scale image");
    }
    if (mResize->HasBorder()) {
        mPreview->SetBorderColor(mResize->GetBorderColor());
    } else {
        mPreview->ClearBorder();
    }
    mPreview->DrawPicture(outScale[0], w, h);
}
