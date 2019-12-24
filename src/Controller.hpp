#include "ResizeWindow.hpp"
#include "PreviewWindow.hpp"
#include "RGBFrameReader.hpp"

class Controller : private PreviewWindow::SeekListener, private ResizeWindow::OnResizeListener {
public:

    Controller(RGBFrameReader *rgb, ResizeWindow *resize, PreviewWindow *preview);
    Controller(const Controller &rhs) = delete;
    Controller(Controller &&rhs) = delete;
    Controller& operator=(const Controller &rhs) = delete;
    Controller& operator=(Controller &&rhs) = delete;
    ~Controller() override;

private:

    void Callback(PreviewWindow *window) override;
    void Callback(ResizeWindow *window) override;
    void Close(ResizeWindow *window) override;
    void draw();

    RGBFrameReader *mRGB;
    ResizeWindow *mResize;
    PreviewWindow *mPreview;
};
