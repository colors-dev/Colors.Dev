// lch_conversions.c

#include "lch_space.h"
#include "xyz_space.h"          // For RgbToLch -> XyzToLab and XyzToLab
#include "common.h"             // For clampInt, clampDbl
#include <string.h>             // For strlen, strcpy_s
#include <math.h>               // For fmin, fmax, fabs, round, pow

COLORS_DEV_API LchSpace RgbToLch(RgbColor rgb)
{
    XyzSpace xyz = RgbToXyz(rgb);
    LabSpace lab = XyzToLab(xyz);
	return LabToLch(lab);
}

COLORS_DEV_API LchSpace LabToLch(LabSpace lab)
{
    double a = lab.a;
    double b = lab.b;

    double C = sqrt(a * a + b * b);
    double H = 0.0;

    if (C < COLORS_LCH_CHROMA_EPS) {
        C = 0.0;
        H = 0.0;
    }
    else {
        H = atan2(b, a) * (180.0 / COLORS_PI);
        if (H < 0.0) H += 360.0; // Faster than fmod()
    }

    LchSpace lch = { lab.l, C, H };
    return lch;
}