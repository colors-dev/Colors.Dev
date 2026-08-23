// hsl_space.c
#include "hsl_space.h"
#include <math.h>               // For fmin, fmax, fabs, round, pow
// #include <string.h>             // For strlen, strcpy_s
// #include <stdio.h>              // Required for sprintf

static double HueToRgb(double p, double q, double t) {
    if (t < 0.0) t += 1.0;
    if (t > 1.0) t -= 1.0;
    if (t < 1.0 / 6.0) return p + (q - p) * 6.0 * t;
    if (t < 1.0 / 2.0) return q;
    if (t < 2.0 / 3.0) return p + (q - p) * (2.0 / 3.0 - t) * 6.0;
    return p;
}

COLORS_DEV_API HslSpace RgbToHsl(RgbColor rgb)
{
    // Convert 0-255 to 0.0-1.0
    double r = (double)rgb.red / 255.0;
    double g = (double)rgb.green / 255.0;
    double b = (double)rgb.blue / 255.0;

    double min = fmin(fmin(r, g), b);
    double max = fmax(fmax(r, g), b);
    double delta = max - min;

    double h = 0.0;
    double s = 0.0;         // mathematically correct HSL saturation
    double l = (max + min) / 2.0;
    double raw = l;         // raw value.  More precise when converting back to RGB.


    if (delta != 0.0)
    {
        s = (l <= 0.5) ? (delta / (max + min)) : (delta / (2.0 - max - min));

        if (r == max)
            h = (g - b) / delta;
        else if (g == max)
            h = 2.0 + (b - r) / delta;
        else // b == max
            h = 4.0 + (r - g) / delta;

        h *= 60.0;
        if (h < 0.0)
            h += 360.0;
    }

    double sn = s;          // human-facing adjusted saturation

    if (delta == 0.0) {
        sn = 0.0;
    }
    // Handle the White Singularity gracefully using HSV saturation logic
    else if (l > 0.95 && delta < COLOR_EPSILON) {
        // delta / max is the HSV saturation formula
        sn = delta / max;
    }
    // Handle the Black Singularity gracefully
    else if (l < 0.05 && delta < COLOR_EPSILON) {
        // You can't use HSV here (it equals 100%). 
        // Using plain delta (Chroma) gracefully scales to 0 near black.
        sn = delta;
    }

    HslSpace hsl = { h, s * 100.0, sn * 100.0, l * 100.0, raw };
    return hsl;
}

COLORS_DEV_API RgbColor HslToRgb(HslSpace hsl) 
{
    // Convert 0-100.0 to 0.0-1.0
    double h = hsl.hue / 360.0; // HSL stores 0-360, but math needs 0-1.0
    double s = hsl.saturation / 100.0;
    double l = hsl.lightness / 100.0;
    double raw = hsl.raw_lightness;       //untouched

    // If raw lightness values exists, they are more precise.
    if (raw > 0.0 && raw <= 1.0 && raw != l)
        l = raw;

    double r, g, b;

    if (s == 0.0)
        r = g = b = l; // achromatic (gray)
    else
    {
        // Use lightness to prepair for RGB
        double q = (l < 0.5) ? (l * (1.0 + s)) : (l + s - l * s);
        double p = 2.0 * l - q;
        r = HueToRgb(p, q, h + 1.0 / 3.0);
        g = HueToRgb(p, q, h);
        b = HueToRgb(p, q, h - 1.0 / 3.0);
    }

    // Convert 0.0-1.0 back to 0-255
    RgbColor rgb = {
        (unsigned char)255.0,
        (unsigned char)round(r * 255.0),
        (unsigned char)round(g * 255.0),
        (unsigned char)round(b * 255.0)
    };
    return rgb;
}

/// <summary>
/// Generates a monochromatic variant (a tint or shade) of the provided base color.
/// <code>
/// // Example of how a consumer would use it to build a standard 5-swatch UI palette
/// void GenerateUiPalette(RgbColor baseColor, RgbColor* outBuffer)
/// {
///     outBuffer[0] = GetMonochromaticVariant(baseColor, 0.4);   // Lighter Tint
///     outBuffer[1] = GetMonochromaticVariant(baseColor, 0.2);   // Light Tint
///     outBuffer[2] = baseColor;                                 // Base Color
///     outBuffer[3] = GetMonochromaticVariant(baseColor, -0.2);  // Dark Shade
///     outBuffer[4] = GetMonochromaticVariant(baseColor, -0.4);  // Darker Shade
/// }
/// </code>
/// </summary>
/// <param name="rgb">The base RGB color to modify.</param>
/// <param name="lightnessShift">
/// A relative delta to apply to the color's lightness, ranging from -1.0 to 1.0. 
/// Positive values create lighter Tints (e.g., 0.15 makes the color 15% lighter).
/// Negative values create darker Shades (e.g., -0.20 makes the color 20% darker).
/// The resulting lightness is safely clamped between 0.0 (pure black) and 1.0 (pure white).
/// </param>
/// <returns>A new RgbColor shifted by the specified lightness amount.</returns>
COLORS_DEV_API RgbColor GetMonochromaticVariant(RgbColor rgb, double lightnessShift)
{
    // HSL is superior to HSV for monochromatic shifts because altering 
    // HSL Lightness creates true tints and shades without skewing saturation.
    HslSpace hsl = RgbToHsl(rgb);

    // Apply the shift to the raw lightness (-1.0 to 1.0 range)
    hsl.raw_lightness += lightnessShift;

    // Hard clamp to valid boundaries to prevent math overflow in conversion
    if (hsl.raw_lightness > 1.0) hsl.raw_lightness = 1.0;
    if (hsl.raw_lightness < 0.0) hsl.raw_lightness = 0.0;

    // Sync the percentage-based property to maintain struct integrity
    hsl.lightness = hsl.raw_lightness * 100.0;

    return HslToRgb(hsl);
}

