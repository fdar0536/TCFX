#pragma once

#include <string>
#include <utility>

#include "../common/basecommon.h"

#define FP_PRECISION 3 // Floating point precision by numbers behind point (for shape points)
#define SUPERSAMPLING 8 // Anti-aliasing precision for shape to pixels conversion

#define UNUSED(x) static_cast<void>(x)

namespace Yutils
{
class SYMBOL_SHOW Common
{
protected:

    Common(void) NOTHROW;

    std::pair<double, double> rotate2d(double, double, double) NOTHROW;

    // math.rad
    double rad(double angle) NOTHROW;

    // math.deg
    double deg(double r) NOTHROW;

    // math.random
    double random(double min, double max) NOTHROW;

    std::string doubleToString(double input) NOTHROW;
};
} // end namespace Yutils
