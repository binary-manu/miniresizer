#ifndef COMMON_HPP
#define	COMMON_HPP

#include <stdexcept>
#include <FL/Enumerations.H>

template <typename Base, typename tag>
class Exception : public Base {
public:
	Exception(const char* what): Base(what) {}
};

typedef Exception<std::logic_error, struct MRExceptionTag> MRException;
typedef Exception<MRException, struct MRNonSevereExceptiongTag> MRNonSevereException;
typedef Exception<MRException, struct MRSevereExceptiongTag> MRSevereException;
typedef Exception<MRSevereException, struct InvalidValueExceptionTag> InvalidValueException;
typedef Exception<MRNonSevereException, struct ResourceAllocationTag> ResourceAllocationException;
typedef Exception<MRNonSevereException, struct AVExceptionTag> AVException;

// Type used to store frame width/height
typedef int FrameSize;
// Type used to represent ratios
typedef double Ratio;
// Type to represent positions/frame numbers
typedef int Position;
// Type used for colors
typedef Fl_Color Color;

#endif // COMMON_HPP
