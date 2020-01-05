#ifndef RESIZE_WINDOW_HPP
#define RESIZE_WINDOW_HPP

#include <vector>

#include "resize.h"
#include "Common.hpp"

#ifdef ENABLE_FILTERGRAPH
    #include <map>
#endif

// Main window class inherits from FLTK generated code
class ResizeWindow : public ResizeWindowBase {
public:
    using Base = ResizeWindowBase;

    class OnResizeListener {
    public:
        OnResizeListener() = default;
        virtual ~OnResizeListener() = default;
        OnResizeListener(const OnResizeListener &rhs) = delete;
        OnResizeListener(OnResizeListener &&rhs) = delete;
        OnResizeListener& operator=(const OnResizeListener &rhs) = delete;
        OnResizeListener& operator=(OnResizeListener &&rhs) = delete;
        virtual void Callback(ResizeWindow *sender) = 0;
        virtual void Close(ResizeWindow *sender) = 0;
    };

    ResizeWindow();
    ResizeWindow(const ResizeWindow &rhs) = delete;
    ResizeWindow(ResizeWindow &&rhs) = delete;
    ResizeWindow& operator=(const ResizeWindow &rhs) = delete;
    ResizeWindow& operator=(ResizeWindow &&rhs) = delete;
    ~ResizeWindow() override = default;
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
    FrameSize GetWSnap() const;
    FrameSize GetHSnap() const;
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
    static void handleCropAlignChange(Fl_Widget *dar, void *_p);
    static void handleCropLeftChange(Fl_Widget *w, void *_p);
    static void handleCropRightChange(Fl_Widget *w, void *_p);
    static void handleCropTopChange(Fl_Widget *w, void *_p);
    static void handleCropBottomChange(Fl_Widget *w, void *_p);
    static void handleDARTyping(Fl_Widget *w, void *_p);
    static void handleTargetWidthChange(Fl_Widget *w, void *_p);
    static void handleClose(Fl_Widget *w, void *_p);
    static void handleWSnapChange(Fl_Widget *w, void *_p);
    static void genericHandler(Fl_Widget *w, void *_p);
    static void handleAbout(Fl_Widget *w, void *_p);

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
#if defined(HAVE_WORDEXP_H)
    static
#endif
    bool expandHelper(const std::string& filter, std::string& expansion);

    Environment mVars;
#endif
    void evaluate(bool doCallback = true);
};

#endif  /* RESIZE_WINDOW_HPP */
