#ifndef PREVIEW_WINDOW_HPP
#define PREVIEW_WINDOW_HPP

#include <vector>

#include "preview.h"
#include "Common.hpp"

class PreviewWindow : public PreviewWindowBase {
public:
	typedef PreviewWindowBase Base;
	
	class SeekListener {
	public:
		~SeekListener() {}
		virtual void Callback(PreviewWindow *theView) = 0;
	};
	
	PreviewWindow();
	~PreviewWindow();
	void DrawPicture(const uint8_t *rgb, FrameSize w, FrameSize h);
	Position GetGranularity();
	Position GetPosition();
	void TriggerUpdate();
	PreviewWindow& SetOnSeekListener(SeekListener *cb);
	SeekListener *GetOnSeekListener();
	void SetBorderColor(Color c);
	void ClearBorder();
private:
	void draw();
	SeekListener *mOnSeek;
	FrameSize mPictureW;
	FrameSize mPictureH;
	std::vector<uint8_t> mPictureData;
	bool mHasBorder;
	Color mBorderColor;
	
	static const FrameSize BORDER_SIZE = 8;
	
	static void handleSlider(Fl_Widget *w, void *p);
	static void handleClose(Fl_Widget* w, void* _p);
	void evaluate(bool docallback);
};

#endif	/* PREVIEW_WINDOW_HPP */
