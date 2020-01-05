#ifndef COMMON_HPP
#define COMMON_HPP

#include <cstdint>
#include <stdexcept>

#include <FL/Enumerations.H>

template <typename Base, typename tag>
class Exception : public Base {
public:
    explicit Exception(const char* what): Base(what) {}
};

using MRException = Exception<std::logic_error, struct MRExceptionTag>;
using MRNonSevereException = Exception<MRException, struct MRNonSevereExceptiongTag>;
using MRSevereException = Exception<MRException, struct MRSevereExceptiongTag>;
using InvalidValueException = Exception<MRSevereException, struct InvalidValueExceptionTag>;
using ResourceAllocationException = Exception<MRNonSevereException, struct ResourceAllocationTag>;
using AVException = Exception<MRNonSevereException, struct AVExceptionTag>;

// Type used to store frame width/height
using FrameSize = int;
// Type used to represent ratios
using Ratio = double;
// Type to represent positions/frame numbers
using Position = int;
// Type used for colors
using Color = Fl_Color;

FrameSize nearestInteger(Ratio d);
FrameSize nearestMultiple(FrameSize i, FrameSize step);
FrameSize snapSize(FrameSize size, FrameSize snap);

// FLTK preferences
const char * const FLTK_PREFS_GROUP_DEFAULT { "default" };
const char * const FLTK_PREFS_DEFAULT_FILTERGRAPH { "filtergraph" };

#endif // COMMON_HPP
