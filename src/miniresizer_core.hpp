#ifndef MINIRESIZER_CORE_HPP
#define	MINIRESIZER_CORE_HPP

#include <stdint.h>
#include <vector>
#include "miniresizer.hpp"
#include <memory>
#include <stdexcept>
#include <map>
extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
}


template <typename Base, typename tag>
class Exception : public Base {
public:
	Exception(const char* what): Base(what) {}
};

// Type used to store frame width/height
typedef int FrameSize;
// Type used to represent ratios
typedef double Ratio;
// Type to represent positions/frame numbers
typedef int Position;
// Type used for colors
typedef Fl_Color Color;

typedef Exception<std::logic_error, struct MRExceptionTag> MRException;
typedef Exception<MRException, struct MRNonSevereExceptiongTag> MRNonSevereException;
typedef Exception<MRException, struct MRSevereExceptiongTag> MRSevereException;
typedef Exception<MRSevereException, struct InvalidValueExceptionTag> InvalidValueException;
typedef Exception<MRNonSevereException, struct ResourceAllocationTag> ResourceAllocationException;
typedef Exception<MRNonSevereException, struct AVExceptionTag> AVException;

// Main window class inherits from FLTK generated code
class ResizeWindow : public ResizeWindowBase {
public:
	typedef ResizeWindowBase Base;
	
	class OnResizeListener {
	public:
		virtual ~OnResizeListener() {};
		virtual void Callback(ResizeWindow *sender) = 0;
	};
	
	ResizeWindow();
	~ResizeWindow();
	OnResizeListener* GetOnResizeListener() const;
	ResizeWindow& SetOnResizeListener(OnResizeListener *cb);
	ResizeWindow& SetInputHeight(FrameSize inputHeight);
	FrameSize GetInputHeight() const;
	ResizeWindow& SetInputWidth(FrameSize inputWidth);
	FrameSize GetInputWidth() const;
	ResizeWindow& SetInputDar(Ratio dar);
	ResizeWindow& SetTargetWidth(FrameSize width);
	Ratio GetDar() const;
	Ratio GetPar() const;
	Ratio GetSar() const;
	FrameSize GetCropLeft() const;
	FrameSize GetCropRight() const;
	FrameSize GetCropTop() const;
	FrameSize GetCropBottom() const;
	FrameSize GetCroppedWidth() const;
	FrameSize GetCroppedHeight() const;
	FrameSize GetResizedWidth() const;
	FrameSize GetResizedHeight() const;
	Color GetBorderColor() const;
	bool HasBorder() const;
	Ratio GetZoom() const;
	bool IsZoomEnlarging() const;
	void TriggerUpdate();
#ifdef ENABLE_FILTERGRAPH
	void expandFiltergraph();
#endif

private:
	OnResizeListener *mOnResize;
	
	enum {
		DARCHOICE_INDEX_ASINPUT,
		DARCHOICE_INDEX_CUSTOM,
		DARCHOICE_INDEX_16_9,
		DARCHOICE_INDEX_4_3,
		DARCHOICE_SIZE
	};
	static const Ratio mDarConstants[];
	
	static FrameSize cropConstaints(FrameSize crop, FrameSize limit, FrameSize step);
	static void enforceCropConstaints(Fl_Spinner *w, FrameSize limit, FrameSize step);
	static void handleDARSelection(Fl_Widget *dar, void *_p);
	static void handleCropAlignChange(Fl_Widget *dar, void *_p);
	static void handleCropLeftChange(Fl_Widget *w, void *_p);
	static void handleCropRightChange(Fl_Widget *w, void *_p);
	static void handleCropTopChange(Fl_Widget *w, void *_p);
	static void handleCropBottomChange(Fl_Widget *w, void *_p);
	static void handleDARTyping(Fl_Widget *w, void *_p);
	static void handleTargetWidthChange(Fl_Widget *w, void *_p);
	static void handleClose(Fl_Widget *w, void *_p);
	static void genericHandler(Fl_Widget *w, void *_p);
	static FrameSize nearestInteger(Ratio d);
	static FrameSize nearestMultiple(FrameSize i, FrameSize step);
#ifdef ENABLE_FILTERGRAPH
	static void saveFiltergraph(Fl_Widget *w, void *_p);

	class Environment {
	private:
		typedef std::map<std::string, std::string> VarsType;
		VarsType mVars;

	public:
		void put(const std::string& name, const std::string& value) {
			mVars[name] = value;
		}

		const std::string get(const std::string& name) {
			VarsType::const_iterator item = mVars.find(name);
			if (item == mVars.end()) {
				return getenv(name.c_str());
			} else {
				return item->second;
			}
		}
	};

	template <typename T> void putInEnv(const char * const name, const T& value);
	void expandVar(std::string& varName, std::string& appendHere);
	bool expandHelper(const std::string& filter, std::string& expansion);

	Environment mVars;
#endif
	void evaluate(bool doCallback = true);
};


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
	AVStream *mVideoStream;
	SwsContext *mScaler;
	AVFrame *mFrame;
	std::vector<uint8_t> mFrameData;
	
	DecodeResult decodeFrame();
	void close();
};

#endif	/* MINIRESIZER_CORE_HPP */

