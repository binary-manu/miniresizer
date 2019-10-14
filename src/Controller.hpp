#include "ResizeWindow.hpp"
#include "PreviewWindow.hpp"
#include "RGBFrameReader.hpp"

class Controller : private PreviewWindow::SeekListener, private ResizeWindow::OnResizeListener {
public:

	Controller(RGBFrameReader *rgb, ResizeWindow *resize, PreviewWindow *preview);
	~Controller();
	
private:

	void Callback(PreviewWindow *);
	void Callback(ResizeWindow *);
	void draw();

	RGBFrameReader *mRGB;
	ResizeWindow *mResize;
	PreviewWindow *mPreview;
};
