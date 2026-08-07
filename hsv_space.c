// hsv_space.c
#include "hsv_space.h"
#include "common.h"             // For clampDbl
#include <math.h>               // For fmin, fmax, fabs, round, pow
// CIELAB, CIELCh, and CIELUV, and XYZ conversions.

COLORS_DEV_API RgbColor HsvToRgb(HsvSpace hsv)
{
    // Whole number
    double h = clampDbl(hsv.hue, 0.0, 360.0);
    // Convert 0-100.0 to 0.0-1.0
    double s = clampDbl(hsv.saturation, 0.0, 100.0) / 100.0;
    double v = clampDbl(hsv.value, 0.0, 100.0) / 100.0;
    double raw = hsv.raw_value;       //untouched

    // If raw values exists, they are more precise.
    if (raw > 0.0 && raw <= 1.0 && raw != v)
        v = raw;

    double r = 0.0, g = 0.0, b = 0.0;

    int i = (int)floor(h / 60.0) % 6;
    double f = (h / 60.0) - floor(h / 60.0);
    double p = v * (1.0 - s);
    double q = v * (1.0 - f * s);
    double t = v * (1.0 - (1.0 - f) * s);

    switch (i)
    {
    case 0: r = v; g = t; b = p; break;
    case 1: r = q; g = v; b = p; break;
    case 2: r = p; g = v; b = t; break;
    case 3: r = p; g = q; b = v; break;
    case 4: r = t; g = p; b = v; break;
    case 5: r = v; g = p; b = q; break;
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

COLORS_DEV_API HsvSpace RgbToHsv(RgbColor rgb)
{
    // Convert 0-255 to 0.0-1.0
    double r = clampDbl(rgb.red, 0.0, 255.0) / 255.0;
    double g = clampDbl(rgb.green, 0.0, 255.0) / 255.0;
    double b = clampDbl(rgb.blue, 0.0, 255.0) / 255.0;

    double min = fmin(fmin(r, g), b);
    double max = fmax(fmax(r, g), b);
    double delta = max - min;

    double h = 0.0;
    double s = 0.0;
    double v = max;     // Value is just the max component
    double raw = max;   // raw value, before conversion.  More precise when converting back to RGB.

    if (delta != 0.0)
    {
        s = (max == 0.0) ? 0.0 : (delta / max);

        if (r == max)
            h = (g - b) / delta;
        else if (g == max)
            h = 2.0 + (b - r) / delta;
        else // b == max
            h = 4.0 + (r - g) / delta;

        h *= 60.0; // convert to degrees
        if (h < 0.0)
            h += 360.0;
    }

    HsvSpace hsv = { h, s * 100.0, v * 100.0, raw };
    return hsv;
}

COLORS_DEV_API colors_dev_float64 GetHue(RgbColor rgb) 
{
    return RgbToHsv(rgb).hue;
}

COLORS_DEV_API colors_dev_float64 GetHsvSaturation(RgbColor rgb)
{
    return RgbToHsv(rgb).saturation;
}

COLORS_DEV_API colors_dev_float64 GetHsvBrightness(RgbColor rgb) 
{
	return RgbToHsv(rgb).raw_value;
}

COLORS_DEV_API char* GetTone(RgbColor rgb)
{
    HsvSpace hsv = RgbToHsv(rgb);
    double h = hsv.hue;
    double v = hsv.value;
    double s = hsv.saturation;

    // NEUTRALS (Top priority for desaturated colors)
    if (s < 5.0 && v >= 95.0)  return createBuffer("Near White");
    if (s < 20.0 && v > 85.0) return createBuffer("Light Neutral");
    if (s < 20.0 && v > 55.0) return createBuffer("Mid Neutral");
    if (s < 20.0 && v > 20.0) return createBuffer("Dark Neutral");

    // VERY DARK (Value <= 20%) - Hue Weighted
    if (v <= 5.0)  return createBuffer("Black");
    if (v <= 10.0 && s < 20.0)  return createBuffer("Near Black");
    if (v <= 20.0) {
        double threshold = 20.0;
        if (h >= 45.0 && h <= 100.0) threshold = 25.0; // Yellow-Greens
        if (h >= 200.0 && h <= 280.0) threshold = 15.0; // Blues-Purples

        return (s <= threshold) ? createBuffer("Neutral") : createBuffer("Very Dark");
    }

    // BRIGHTNESS BLOCKS (Remaining colors have s >= 20.0)
    if (v > 85.0) {
        if (s > 75.0) return createBuffer("Vivid");
        if (s >= 20.0) return createBuffer("Pastel");
        return createBuffer("Bright");
    }

    if (v > 55.0) {
        if (s > 75.0) return createBuffer("Strong");
        if (s >= 20.0) return createBuffer("Moderate");
        return createBuffer("Muted Light");
    }

    // LOW BRIGHTNESS
    if (s > 60.0) return createBuffer("Deep");
    if (s >= 20.0) return createBuffer("Dull");

    return createBuffer("Washed Out"); // Accessible if top Neutral thresholds are ever adjusted
}

COLORS_DEV_API char* GetTemperature(RgbColor rgb)
{

    // Using rgb instead of just hue, allows us to handle edge cases 
    // like pure white (0 saturation) and near-neutral colors more effectively.
    HsvSpace hsv = RgbToHsv(rgb);
    double h = hsv.hue;
    double s = hsv.saturation;
    double v = hsv.value;

    // 1. GLOBAL NEUTRAL GUARDS (Top Priority)
    if (v < 10.0) return createBuffer("Achromatic Dark");   // Too dark to see color
    if (s < 2.0)  return createBuffer("Achromatic Light");  // Pure gray/silver
    if (s < 8.0)  return createBuffer("Neutral");           // Near-neutral silver/ash

    int isMuted = (s < 35.0);

    // Normalize hue
    while (h < 0) h += 360.0;
    while (h >= 360.0) h -= 360.0;

    // Red / Red-Magenta
    if (h >= 315.0 || h < 15.0) return createBuffer(isMuted ? "Muted Hot" : "Hot");

    // Warm: Orange/Yellow
    if (h < 75.0) return createBuffer(isMuted ? "Muted Warm" : "Warm");

    // Neutral-Warm: Yellow-Green
    if (h < 105.0) return isMuted ? createBuffer("Muted Neutral-Warm") : createBuffer("Neutral-Warm");

    // Neutral: Green
    if (h < 135.0) return isMuted ? createBuffer("Muted Neutral") : createBuffer("Neutral");

    // Neutral-Cool: Cyan-Green
    if (h < 165.0) return isMuted ? createBuffer("Muted Neutral-Cool") : createBuffer("Neutral-Cool");

    // Cool: Cyan/Blue-Cyan
    if (h < 225.0) return isMuted ? createBuffer("Muted Cool") : createBuffer("Cool");

    // Cold: Blue/Violet
    if (h < 285.0) return isMuted ? createBuffer("Muted Cold") : createBuffer("Cold");

    // Cool-Hot transition: Violet/Magenta
    return createBuffer(isMuted ? "Muted Cool-Hot" : "Neutral-Cool-Hot");
}

COLORS_DEV_API RgbColor GetComplementary(RgbColor rgb)
{
    // Using rgb instead of just hue, allows us to handle edge cases 
    // like pure white (0 saturation) and near-neutral colors more effectively.
    HsvSpace hsv = RgbToHsv(rgb);
    double newHue = hsv.hue + 180.0;        // Shift hue by 180 degrees to get the complementary color
	if (newHue >= 360.0) newHue -= 360.0;   // Wrap around if hue exceeds 360 degrees

    HsvSpace hsvRet = { newHue, hsv.saturation, hsv.value, hsv.raw_value };
    return HsvToRgb(hsvRet);
}

COLORS_DEV_API TriadicResults GetTriadic(RgbColor rgb)
{
    HsvSpace hsv = RgbToHsv(rgb);
    TriadicResults result;

    double angles[2] = { 120.0, 240.0 };

    for (int i = 0; i < 2; i++) {
        double newHue = fmod(hsv.hue + angles[i], 360.0);
        HsvSpace hsvNext = { newHue, hsv.saturation, hsv.value, hsv.raw_value };
        result.colors[i] = HsvToRgb(hsvNext);
    }

    return result;
}

COLORS_DEV_API AnalogousResults GetAnalogous(RgbColor rgb)
{
    HsvSpace hsv = RgbToHsv(rgb);
    AnalogousResults res;

    // Clockwise (+30)
    double hPlus = fmod(hsv.hue + 30.0, 360.0);
    res.colors[0] = HsvToRgb((HsvSpace) { hPlus, hsv.saturation, hsv.value, hsv.raw_value });

    // Counter-Clockwise (-30)
    double hMinus = hsv.hue - 30.0;
    if (hMinus < 0) hMinus += 360.0;
    res.colors[1] = HsvToRgb((HsvSpace) { hMinus, hsv.saturation, hsv.value, hsv.raw_value });

    return res;
}

COLORS_DEV_API TetradicResults GetTetradic(RgbColor rgb)
{
    HsvSpace hsv = RgbToHsv(rgb);
    TetradicResults result;

    double angles[3] = { 90.0, 180.0, 270.0 };

    for (int i = 0; i < 3; i++) {
        double newHue = fmod(hsv.hue + angles[i], 360.0);
        HsvSpace hsvNext = { newHue, hsv.saturation, hsv.value, hsv.raw_value };
        result.colors[i] = HsvToRgb(hsvNext);
    }

    return result;
}
