#ifndef PREVIEW_WINDOW_HPP
#define PREVIEW_WINDOW_HPP

#include <vector>

#include "preview.h"
#include "Common.hpp"

class PreviewWindow : public PreviewWindowBase {
public:
    using Base = PreviewWindowBase;

    class SeekListener {
    public:
        SeekListener() = default;
        virtual ~SeekListener() = default;
        SeekListener(const SeekListener &rhs) = delete;
        SeekListener(SeekListener &&rhs) = delete;
        SeekListener& operator=(const SeekListener &rhs) = delete;
        SeekListener& operator=(SeekListener &&rhs) = delete;
        virtual void Callback(PreviewWindow *theView) = 0;
    };

    PreviewWindow();
    PreviewWindow(const PreviewWindow &rhs) = delete;
    PreviewWindow(PreviewWindow &&rhs) = delete;
    PreviewWindow& operator=(const PreviewWindow &rhs) = delete;
    PreviewWindow& operator=(PreviewWindow &&rhs) = delete;
    ~PreviewWindow() override = default;
    void DrawPicture(const uint8_t *rgb, FrameSize w, FrameSize h);
    Position GetGranularity();
    Position GetPosition();
    void TriggerUpdate();
    PreviewWindow& SetOnSeekListener(SeekListener *onSeek);
    SeekListener *GetOnSeekListener();
    void SetBorderColor(Color c);
    void ClearBorder();
private:
    void draw() override;
    SeekListener *mOnSeek;
    FrameSize mPictureW;
    FrameSize mPictureH;
    std::vector<uint8_t> mPictureData;
    Color mBorderColor;
    bool mHasBorder;

    static constexpr FrameSize BORDER_SIZE = 8;

    static void handleSlider(Fl_Widget *w, void *p);
    static void handleClose(Fl_Widget* w, void* _p);
    void evaluate(bool docallback);
};

#endif  /* PREVIEW_WINDOW_HPP */
