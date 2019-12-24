// Copyright 2020 Emanuele Giacomelli

#include <FL/fl_draw.H>

#include "PreviewWindow.hpp"

PreviewWindow::PreviewWindow():
    mOnSeek(nullptr),
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

void PreviewWindow::handleSlider(Fl_Widget* /* w */ , void* _p) {
    auto * const p = static_cast<PreviewWindow*>(_p);
    p->evaluate(true);
}

void PreviewWindow::handleClose(Fl_Widget* /* w */, void* /* _p */) {
    // Do not close
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
    int x = 0;
    int y = 0;
    if (!mPictureData.empty() && ((d & FL_DAMAGE_ALL) || (d & FL_DAMAGE_EXPOSE))) {
        if (mHasBorder) {
            int r = static_cast<int>(mBorderColor / (256 * 256 * 256) % 256);
            int g = static_cast<int>(mBorderColor / (256 * 256) % 256);
            int b = static_cast<int>(mBorderColor / (256) % 256);
            fl_rectf(0, 0, BORDER_SIZE, BORDER_SIZE * 2 + mPictureH, r, g, b);  // Left border
            fl_rectf(w() - BORDER_SIZE, 0, BORDER_SIZE, BORDER_SIZE * 2 + mPictureH, r, g, b);  // Right border
            fl_rectf(0, 0, w(), BORDER_SIZE, r, g, b);  // Top border
            fl_rectf(0, BORDER_SIZE + mPictureH, w(), BORDER_SIZE, r, g, b);  // Bottom border
            x = y = BORDER_SIZE;
        }
        fl_draw_image(&mPictureData[0], x, y, mPictureW, mPictureH);
    }
    draw_children();
}
