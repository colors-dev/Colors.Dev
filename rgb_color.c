// rgb_color.c
#include "rgb_color.h"
#include "common.h"             // clampDbl
#include <string.h>             // For strlen, strcpy_s
#include <math.h>               // For fmin, fmax, fabs, round, pow
#include <stdio.h>              // Required for sprintf
#include <objbase.h>            // For malloc

COLORS_DEV_API char* RgbToRgbHex(RgbColor clr, unsigned int includeAlpha)
{
    char rgbHex[16] = { 0 };

    if (includeAlpha)
        snprintf(rgbHex, sizeof(rgbHex), "#%08X", (unsigned)RgbToArgbDec(clr));
    else
        snprintf(rgbHex, sizeof(rgbHex), "#%06X", (unsigned)RgbToRgbDec(clr));

    size_t len = strlen(rgbHex) + 1;
    char* buffer = (char*)malloc(len);

    if(buffer)
        memcpy(buffer, rgbHex, len);

    return buffer;
}

COLORS_DEV_API colors_dev_color32 RgbToRgbDec(RgbColor clr)
{
    // Decimal form: (r, g, b)
    uint32_t rgb = (clr.red << 16) | (clr.green << 8) | clr.blue;
    return rgb;
}

COLORS_DEV_API colors_dev_color32 RgbToArgbDec(RgbColor clr)
{
    // Decimal form: (r, g, b)
    unsigned int rgb = (clr.red << 16) | (clr.green << 8) | clr.blue;
    uint32_t argb = (clr.alpha << 24) | rgb;
    return argb;
}

COLORS_DEV_API LinearColor RgbToLinear(RgbColor clr)
{
    LinearColor out;

    // Normalize to 0.0 - 1.0
    double r = clr.red / 255.0;
    double g = clr.green / 255.0;
    double b = clr.blue / 255.0;

    // Apply transformation
	out.red = SrgbToLinear(r);
	out.green = SrgbToLinear(g);
	out.blue = SrgbToLinear(b);
    out.alpha = clr.alpha / 255.0;

    return out;
}

COLORS_DEV_API RgbColor LinearToRgb(LinearColor lClr)
{
    RgbColor out;

    // Apply Inverse Gamma Correction (Linear -> sRGB)
    double r = LinearToSrgb(lClr.red);
    double g = LinearToSrgb(lClr.green);
    double b = LinearToSrgb(lClr.blue);
    double a = lClr.alpha;

    // Scale to 255, Clamp to prevent overflow, and Round
    out.red = (unsigned char)round(clampDbl(r * 255.0, 0.0, 255.0));
    out.green = (unsigned char)round(clampDbl(g * 255.0, 0.0, 255.0));
    out.blue = (unsigned char)round(clampDbl(b * 255.0, 0.0, 255.0));
    out.alpha = (unsigned char)round(clampDbl(a * 255.0, 0.0, 255.0));

    return out;
}

COLORS_DEV_API colors_dev_float64 SrgbToLinear(colors_dev_float64 srgb)
{
    // Returns a value typically between 0.0 and 1.0
    return (srgb <= 0.04045)
        ? (srgb / 12.92)
        : pow((srgb + 0.055) / 1.055, 2.4);
}

COLORS_DEV_API colors_dev_float64 LinearToSrgb(colors_dev_float64 linear)
{
    // Returns a value typically between 0.0 and 1.0
    return (linear <= 0.0031308)
        ? (linear * 12.92)
        : (1.055 * pow(linear, 1.0 / 2.4) - 0.055);
}