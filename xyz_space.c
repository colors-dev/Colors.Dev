// xyz_space.c
#include "xyz_space.h"
#include "common.h"
#include <string.h>             // For strlen, strcpy_s
#include <math.h>               // For fmin, fmax, fabs, round, pow
#include <stdio.h>              // Required for sprintf
#include <objbase.h>            // For CoTaskMemAlloc

// CIELAB, CIELCh, and CIELUV, and XYZ conversions.

static inline double lab_f(double t)
{
    if (t < 0.0) t = 0.0;

    const double delta = 6.0 / 29.0;
    const double delta2 = delta * delta;
    const double delta3 = delta2 * delta;

    if (t >= delta3)
        return cbrt(t);

    const double inv_3delta2 = 1.0 / (3.0 * delta2);
    return (t * inv_3delta2) + (4.0 / 29.0);
}

static inline LabSpace XyzToLab_White(XyzSpace xyz, WhitePoint wp)
{
    // Normalize XYZ by reference white (XYZ expected 0..100 scale)
    const double x = xyz.x / wp.x;
    const double y = xyz.y / wp.y;
    const double z = xyz.z / wp.z;

    const double fx = lab_f(x);
    const double fy = lab_f(y);
    const double fz = lab_f(z);

    LabSpace lab = {
        (116.0 * fy) - 16.0,
        500.0 * (fx - fy),
        200.0 * (fy - fz)
    };

    return lab;
}

COLORS_DEV_API LabSpace RgbToLab(RgbColor rgb)
{
    XyzSpace xyz = RgbToXyz(rgb);
	return XyzToLab_White(xyz, WP_D65_FULL);     // WP_D65_FULL - default, WP_D65 is the other options
}

COLORS_DEV_API XyzSpace RgbToXyz(RgbColor rgb)
{
    // Normalize R, G, B to the range [0, 1]
    double r = rgb.red / 255.0;
    double g = rgb.green / 255.0;
    double b = rgb.blue / 255.0;

    // Apply gamma correction (sRGB to linear RGB)
    r = (r > 0.04045) ? pow((r + 0.055) / 1.055, 2.4) : r / 12.92;
    g = (g > 0.04045) ? pow((g + 0.055) / 1.055, 2.4) : g / 12.92;
    b = (b > 0.04045) ? pow((b + 0.055) / 1.055, 2.4) : b / 12.92;

    // Convert linear RGB to XYZ using sRGB-specific transformation matrix
    // These coefficients are for sRGB with D65 illuminant
    double x = r * 0.4124564 + g * 0.3575761 + b * 0.1804375;
    double y = r * 0.2126729 + g * 0.7151522 + b * 0.0721750;
    double z = r * 0.0193339 + g * 0.1191920 + b * 0.9503041;

    // Scale XYZ to 0-100 range for consistency with common representations
    // If Y is expected to be 100 for white, then X and Z should be scaled accordingly.
    // The coefficients already produce values in a range where Y for white is ~1.0,
    // so multiplying by 100 makes Y for white = 100.
    XyzSpace xyz = {
        x * 100.0,
        y * 100.0,
        z * 100.0
    };
    return xyz;
}

COLORS_DEV_API LabSpace XyzToLabEx(XyzSpace xyz, WhitePointType wp)
{
    switch (wp)
    {
    case WPID_D65_FULL:
        return XyzToLab_White(xyz, WP_D65_FULL);
    case WPID_D65:
    default:
        return XyzToLab_White(xyz, WP_D65);
    }
}

COLORS_DEV_API LabSpace XyzToLab(XyzSpace xyz)
{
    return XyzToLab_White(xyz, WP_D65_FULL);     // WP_D65_FULL - default, WP_D65 is the other options
}

COLORS_DEV_API colors_dev_float64 GetRelativeLuminance(RgbColor rgb)
{
    return RgbToXyz(rgb).y;
}

COLORS_DEV_API colors_dev_float64 GetPerceptualBrightness(RgbColor rgb) {
    return RgbToLab(rgb).l;
}

COLORS_DEV_API colors_dev_float64 GetContrastRatio(RgbColor a, RgbColor b)
{
    // Ensure these are normalized to 0.0 - 1.0
    colors_dev_float64 la = GetRelativeLuminance(a);
    colors_dev_float64 lb = GetRelativeLuminance(b);

    // If your library returns 0-100, normalize here:
    if (la > 1.0 || lb > 1.0) {
        la /= 100.0;
        lb /= 100.0;
    }

    colors_dev_float64 l1 = (la > lb) ? la : lb;
    colors_dev_float64 l2 = (la > lb) ? lb : la;

    return (l1 + 0.05) / (l2 + 0.05);
}

COLORS_DEV_API colors_dev_color32 GetBestContrastColor(RgbColor bgColor)
{
    RgbColor white = { 255, 255, 255, 255 };
    RgbColor black = { 255, 0, 0, 0 };

    return (GetContrastRatio(bgColor, white) >= GetContrastRatio(bgColor, black));
}