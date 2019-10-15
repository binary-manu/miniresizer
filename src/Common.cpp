#include <math.h>

#include "Common.hpp"

FrameSize nearestInteger(Ratio d) {
	d = abs(d);
	return (d - floor(d) <= 0.5) ? floor(d) : ceil(d);
}
	
FrameSize nearestMultiple(FrameSize i, FrameSize step) {
	FrameSize min = i / step * step;
	FrameSize max = min + step;
	Ratio mid = min + Ratio(step) / 2;
	return (i > mid) ? max : min;
}

FrameSize snapSize(FrameSize size, FrameSize snap) {
	const FrameSize newSize = nearestMultiple(size, snap);
	return (newSize < snap) ? snap : newSize;
}
