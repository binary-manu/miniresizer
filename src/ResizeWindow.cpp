// Copyright 2020 Emanuele Giacomelli

#include "./config.h"

#include "ResizeWindow.hpp"
#include "AboutWindow.hpp"

#ifdef HAVE_WORDEXP_H
# include <wordexp.h>
#endif

#include <cmath>
#include <sstream>
#include <string>
#include <memory>

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
    if (dar <= 0 || !std::isfinite(dar)) {
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
    1.0,        // Unused
    1.0,        // Unused
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

ResizeWindow::ResizeWindow():
    mOnResize(nullptr)
{
    label(PACKAGE_NAME);

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

    mDarChoice->callback(&genericHandler, this);
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
    mWSnap->callback(&handleWSnapChange, this);
    mHSnap->callback(&genericHandler, this);
    mMenuFileQuit->callback(&handleClose, this);
    mMenuHelpAbout->callback(&handleAbout, this);

#ifdef ENABLE_FILTERGRAPH
    mMakeDefaultFg->callback(&saveFiltergraph, this);
    Fl_Preferences prefs(Fl_Preferences::USER, "", PACKAGE_NAME);
    Fl_Preferences defaultGroup(prefs, FLTK_PREFS_GROUP_DEFAULT);
    char *fg {nullptr};
    defaultGroup.get(FLTK_PREFS_DEFAULT_FILTERGRAPH, fg, "");
    std::unique_ptr<char, decltype(&free)> pfg {fg, &free};
    if (!pfg) {
        throw ResourceAllocationException("Unable to allocate space for reading the settings");
    }
    mFgSource->buffer()->text(pfg.get());
#else
    h(mAboveFg->y());
#endif

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
    // The UI control can return a negative crop value because I've used a negative
    // minimum to avoid the unwanted wrap-around from 0 to the maximum
    FrameSize crop = w->value();
    if (crop < 0) {
        crop = 0;
    }
    w->value(cropConstaints(crop, limit, step));
}

void ResizeWindow::handleCropAlignChange(Fl_Widget * /* dar */, void *_p) {
    ResizeWindow *p = (static_cast<ResizeWindow*>(_p));

    auto align = FrameSize(std::max(p->mCropAlign->value(), Ratio(1)));
    p->mCropAlign->value(align);

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

void ResizeWindow::handleCropLeftChange(Fl_Widget * /* w */, void *_p) {
    ResizeWindow *p = (static_cast<ResizeWindow*>(_p));

    enforceCropConstaints(
        p->mCropLeft,
        p->mInputWidth->value() - p->mCropRight->value(),
        p->mCropAlign->value()
    );
    p->evaluate();
}

void ResizeWindow::handleCropRightChange(Fl_Widget * /* w */, void *_p) {
    ResizeWindow *p = (static_cast<ResizeWindow*>(_p));

    enforceCropConstaints(
        p->mCropRight,
        p->mInputWidth->value() - p->mCropLeft->value(),
        p->mCropAlign->value()
    );
    p->evaluate();
}

void ResizeWindow::handleCropTopChange(Fl_Widget * /* w */, void *_p) {
    ResizeWindow *p = (static_cast<ResizeWindow*>(_p));

    enforceCropConstaints(
        p->mCropTop,
        p->mInputHeight->value() - p->mCropBottom->value(),
        p->mCropAlign->value()
    );
    p->evaluate();
}

void ResizeWindow::handleCropBottomChange(Fl_Widget * /* w */, void *_p) {
    ResizeWindow *p = (static_cast<ResizeWindow*>(_p));

    enforceCropConstaints(
        p->mCropBottom,
        p->mInputHeight->value() - p->mCropTop->value(),
        p->mCropAlign->value()
    );
    p->evaluate();
}

void ResizeWindow::handleDARTyping(Fl_Widget * /*w */, void *_p) {
    ResizeWindow *p = (static_cast<ResizeWindow*>(_p));
    Ratio dar = p->mDar->value();
    if (dar < p->mDar->minimum() || dar > p->mDar->maximum()) {
        dar = 1.0;
        p->mDar->value(dar);
    }
    p->evaluate();
}

void ResizeWindow::handleTargetWidthChange(Fl_Widget * /*w */, void *_p) {
    ResizeWindow *p = (static_cast<ResizeWindow*>(_p));
    auto targetWidth = p->mTargetWidth->value();
    auto step = p->mTargetWidth->step();
    if (targetWidth < p->mTargetWidth->step()) {
        targetWidth = step;
    }
    p->mTargetWidth->value(nearestMultiple(targetWidth, step));
    p->evaluate();
}

void ResizeWindow::handleWSnapChange(Fl_Widget * /* w */, void *_p) {
    ResizeWindow *p = (static_cast<ResizeWindow*>(_p));
    const auto snap = FrameSize(std::max(p->mWSnap->value(), Ratio(1)));
    p->mWSnap->value(snap);
    p->mTargetWidth->step(snap);
    p->mTargetWidth->value(snapSize(p->mTargetWidth->value(), snap));
    p->evaluate();
}

void ResizeWindow::genericHandler(Fl_Widget * w, void *_p) {
    ResizeWindow *p = (static_cast<ResizeWindow*>(_p));
    auto valuator = dynamic_cast<Fl_Valuator*>(w);
    if (valuator) {
        const auto min = valuator->minimum();
        const auto max = valuator->maximum();
        auto val = valuator->value();
        if (val < min) {
            val = min;
        } else if (max < val) {
            val = max;
        }
        valuator->value(val);
    }
    p->evaluate();
}

void ResizeWindow::handleClose(Fl_Widget * /*w */, void *_p) {
    ResizeWindow *p = (static_cast<ResizeWindow*>(_p));
    p->mOnResize->Close(p);
}

void ResizeWindow::handleAbout(Fl_Widget * /*w */, void * /* _p */) {
    static AboutWindow *about { new AboutWindow() };
    about->set_modal();
    about->show();
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
        // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
        mDar->value(mDarConstants[mDarChoice->value()]);
    }

    // Calculate PAR
    Ratio par = mDar->value() / mInputSar->value();
    mPar->value(par);

    Ratio scaled_h = cropH / par;
    FrameSize target_h = nearestInteger(scaled_h * mTargetWidth->value() / cropW);
    FrameSize target_h_mult = nearestMultiple(target_h, mHSnap->value());
    if (target_h <= 0 || target_h_mult <= 0) {
        target_h = 1;
        target_h_mult = mHSnap->value();
    }
    mResizedWidth->value(mTargetWidth->value());
    mResizedHeight->value(target_h_mult);
    mResizeError->value((Ratio(target_h_mult) - target_h) / target_h);

    const FrameSize delta_h = std::abs(target_h_mult - target_h);
    mPixelDelta->value(delta_h);

#ifdef ENABLE_FILTERGRAPH
    expandFiltergraph();
#endif

    if (mOnResize && doCallback) {
        mOnResize->Callback(this);
    }
}

#ifdef ENABLE_FILTERGRAPH

#if !defined(HAVE_WORDEXP_H)

template <typename T> void ResizeWindow::putInEnv(const char * const name, const T& value) {
    std::ostringstream os;
    os << value;
    mVars.put(name, os.str());
}

void ResizeWindow::expandVar(std::string& varName, std::string& appendHere) {
    const std::string& expansion = mVars.get(varName);
    if (expansion.size() > 0) {
        appendHere += expansion;
    }
    varName.clear();
}

bool ResizeWindow::expandHelper(const std::string& filter, std::string& expansion) {
    enum { X_NOVAR, X_ESC, X_VAR, X_VAR2, X_BRACE } state = X_NOVAR;

    std::string tmp;
    std::string varName;

    for (std::string::const_iterator nextChar = filter.begin(); nextChar != filter.end();) {
        switch (state) {
        case X_NOVAR:
            if (*nextChar == '\\') {
                state = X_ESC;
            } else if (*nextChar == '$') {
                state = X_VAR;
            } else {
                tmp.push_back(*nextChar);
            }
        break;

        case X_ESC:
            if (*nextChar != '\n') {
                tmp.push_back(*nextChar);
            }
            state = X_NOVAR;
        break;

        case X_VAR:
            if (*nextChar == '{') {
                state = X_BRACE;
            } else if (isalnum(*nextChar) || *nextChar == '_') {
                state = X_VAR2;
                varName.push_back(*nextChar);
            } else {
                return false;  // This means a dollar sign followed by neither { nor a valid name char
            }
        break;

        case X_VAR2:
            if (isalnum(*nextChar) || *nextChar == '_') {
                varName.push_back(*nextChar);
            } else {
                expandVar(varName, tmp);
                state = X_NOVAR;
                // Keep nextChar to be processed again in X_NOVAR state
                continue;
            }
        break;

        case X_BRACE:
            if (isalnum(*nextChar) || *nextChar == '_') {
                varName.push_back(*nextChar);
            } else if (*nextChar == '}') {
                expandVar(varName, tmp);
                state = X_NOVAR;
            }
        break;
        }

        nextChar++;
    }

    if (state == X_VAR || state == X_BRACE || state == X_ESC) {
        return false;
    }

    if (state == X_VAR2) {
        expandVar(varName, tmp);
    }

    expansion = tmp;
    return true;
}

#else  // #if !defined(HAVE_WORDEXP_H)

bool ResizeWindow::expandHelper(const std::string& filter, std::string& expansion) {
    wordexp_t we;

    std::string filterSource;
    filterSource.reserve(filter.size() + 2);
    filterSource.push_back('\u0022');
    filterSource += filter;
    filterSource.push_back('\u0022');
    const int weNok = wordexp(filterSource.c_str(), &we, 0);
    if (weNok || we.we_wordc != 1) {
        return false;
    }
    std::unique_ptr<wordexp_t, decltype(&wordfree)> pwe {&we, &wordfree};

    expansion = we.we_wordv[0];
    return true;
}

template <typename T> void ResizeWindow::putInEnv(const char * const name, const T& value) {
    std::ostringstream os;
    os << value;
    setenv(name, os.str().c_str(), 1);
}

#endif


void ResizeWindow::expandFiltergraph() {
    std::ostringstream os;
    // Export resize parameters to the environment for wordexp to use
    putInEnv("MRCL", mCropLeft->value());
    putInEnv("MRCR", mCropRight->value());
    putInEnv("MRCT", mCropTop->value());
    putInEnv("MRCB", mCropBottom->value());
    putInEnv("MRCW", mCroppedWidth->value());
    putInEnv("MRCH", mCroppedHeight->value());
    putInEnv("MRTW", mResizedWidth->value());
    putInEnv("MRTH", mResizedHeight->value());

    const int filterLength = mFgSource->buffer()->length();
    std::string filter;
    filter.reserve(filterLength);
    std::unique_ptr<char, decltype(&free)> filterC {mFgSource->buffer()->text(), &free};
    if (!filterC) {
        throw ResourceAllocationException("Unable to get filtergraph source in textual form for processing");
    }
    filter = filterC.get();

    std::string expansion;
    if (!expandHelper(filter, expansion)) {
        mFgFilter->color(FL_RED);
        mFgFilter->buffer()->text("");
    } else {
        mFgFilter->color(FL_WHITE);
        mFgFilter->buffer()->text(expansion.c_str());
    }
}

void ResizeWindow::saveFiltergraph(Fl_Widget * /*w*/, void *_p) {
    Fl_Preferences prefs(Fl_Preferences::USER, "", PACKAGE_NAME);
    Fl_Preferences defaultGroup(prefs, FLTK_PREFS_GROUP_DEFAULT);
    std::unique_ptr<char, decltype(&free)> fg {
        static_cast<ResizeWindow*>(_p)->mFgSource->buffer()->text(), &free
    };
    if (!fg) {
        throw ResourceAllocationException("Memory allocation failed");
    }
    defaultGroup.set(FLTK_PREFS_DEFAULT_FILTERGRAPH, fg.get());
}

#endif

void ResizeWindow::TriggerUpdate() {
    evaluate(true);
}

Color ResizeWindow::GetBorderColor() const {
    static Color palette[] = {
        fl_rgb_color(0, 0, 0),
        fl_rgb_color(0, 0, 0),
        fl_rgb_color(255, 255, 255)
    };
    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
    return palette[mPreviewBorder->value()];
}

bool ResizeWindow::HasBorder() const {
    return mPreviewBorder->value();
}

Ratio ResizeWindow::GetZoom() const {
    return mZoomMultiplier->value();
}

bool ResizeWindow::IsZoomEnlarging() const {
    return mZoomIn->value();
}

ResizeWindow& ResizeWindow::SetTargetWidth(FrameSize width) {
    mTargetWidth->value(width);
    return *this;
}

FrameSize ResizeWindow::GetWSnap() const {
    return mWSnap->value();
}

FrameSize ResizeWindow::GetHSnap() const {
    return mHSnap->value();
}
