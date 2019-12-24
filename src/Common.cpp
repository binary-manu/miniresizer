// Copyright 2020 Emanuele Giacomelli

#include <cmath>

#include "Common.hpp"

FrameSize nearestInteger(Ratio d) {
    static constexpr auto HALFWAY = 0.5;
    d = std::abs(d);
    return (d - std::floor(d) <= HALFWAY) ? std::floor(d) : std::ceil(d);
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
