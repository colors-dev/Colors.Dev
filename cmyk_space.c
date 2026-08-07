// cmyk_conversions.c
#include "cmyk_space.h"
#include "common.h"             // For clampInt, clampDbl
// #include <string.h>             // For strlen, strcpy_s
#include <math.h>               // For fmin, fmax, fabs, round, pow
#include <stdbool.h>            // bool

COLORS_DEV_API CmykSpace RgbToCmyk(RgbColor rgb)
{
    double r = rgb.red / 255.0;
    double g = rgb.green / 255.0;
    double b = rgb.blue / 255.0;

    double rawK = 1.0 - fmax(r, fmax(g, b));
    double c = 0, m = 0, y = 0;

    //can not divid by 0, so zero it all out.
    if ((1.0 - rawK) > 1e-12)
    {
        c = (1.0 - r - rawK) / (1.0 - rawK);
        m = (1.0 - g - rawK) / (1.0 - rawK);
        y = (1.0 - b - rawK) / (1.0 - rawK);

        // clamp for safety vs tiny negatives from floating point
        c = fmax(0.0, fmin(1.0, c));
        m = fmax(0.0, fmin(1.0, m));
        y = fmax(0.0, fmin(1.0, y));
    }

	// Convert to 0-100.0 range for CMYK struct, but keep rawK in 0.0-1.0 for better accuracy when converting back to RGB.
    CmykSpace cmyk = {
        c * 100, 
        m * 100, 
        y * 100, 
        rawK * 100,
        rawK 
    };
    return cmyk;
}

COLORS_DEV_API RgbColor CmykToRgb(CmykSpace cmyk)
{
    double c = clampDbl(cmyk.cyan / 100.0, 0.0, 1.0);
    double m = clampDbl(cmyk.magenta / 100.0, 0.0, 1.0);
    double y = clampDbl(cmyk.yellow / 100.0, 0.0, 1.0);

    double k = (cmyk.raw_key >= 0.0 && cmyk.raw_key <= 1.0)
        ? cmyk.raw_key
        : clampDbl(cmyk.key / 100.0, 0.0, 1.0);

    // Convert CMYK to RGB
    int r = (int)lround(255.0 * (1.0 - c) * (1.0 - k));
    int g = (int)lround(255.0 * (1.0 - m) * (1.0 - k));
    int b = (int)lround(255.0 * (1.0 - y) * (1.0 - k));

    // Ensure RGB values are within the valid range [0, 255]
    r = clampInt(r, 0, 255);
    g = clampInt(g, 0, 255);
    b = clampInt(b, 0, 255);

	RgbColor rgb = { 
        255, 
        (unsigned char)r, 
        (unsigned char)g, 
        (unsigned char)b 
    };
    return rgb;
}

COLORS_DEV_API char* GetCmykMod(CmykSpace cmyk)
{
    char intensity[32] = { 0 };
    char modifier[32] = { 0 };

    double c = cmyk.cyan;
    double m = cmyk.magenta;
    double y = cmyk.yellow;
    double k = cmyk.key;

    double diffCM = fabs(c - m);    // Blue-family balance
    double diffMY = fabs(m - y);    // Red-family balance
    double diffCY = fabs(c - y);    // Green-family balance
    double maxCmy = fmax(c, fmax(m, y));
    double minCmy = fmin(c, fmin(m, y));
    double cmySpread = maxCmy - minCmy;

    bool hasStrongPigment = maxCmy > 70.0;
    bool isBalancedMix = cmySpread < 15.0;
    bool hasStrongDominance = cmySpread >= 35.0;
    bool warmPigment = c < 35.0 && m > 50.0 && y > 55.0;

    // PAPER WHITE (Top priority)
    if (c < 5.0 && m < 5.0 && y < 5.0 && k < 5.0) return createBuffer("Paper White");
    // BLACKED OUT (Top priority)
    if (k > 95.0) return createBuffer("Deep Inky Black");
    
    if (k < 8.0)
        sprintf_s(intensity, sizeof(intensity), hasStrongPigment ? "Vivid" : "Light"); 
    else if (k < 18.0)
        sprintf_s(intensity, sizeof(intensity), hasStrongPigment ? "Strong" : "Soft");
    else if (k < 30.0)
        sprintf_s(intensity, sizeof(intensity), (warmPigment && hasStrongPigment ? "Fiery" : (hasStrongPigment ? "Bold" : "Muted")));
    else if (k < 45.0)
        sprintf_s(intensity, sizeof(intensity), "Rich");
    else if (k < 60.0)
        sprintf_s(intensity, sizeof(intensity), (hasStrongDominance ? "Deep" : "Dull"));
    else if (k < 73.0)
        sprintf_s(intensity, sizeof(intensity), (hasStrongDominance ? "Darkened" : "Smoked"));
    else if (k <= 95.0)
        sprintf_s(intensity, sizeof(intensity), "Blackened");

    // Primary CMY pigments
    if (c > 85.0 && m < 20.0 && y < 20.0) sprintf_s(modifier, sizeof(modifier), "Cyan");
    else if (m > 85.0 && c < 20.0 && y < 20.0) sprintf_s(modifier, sizeof(modifier), "Magenta");
    else if (y > 85.0 && c < 20.0 && m < 20.0) sprintf_s(modifier, sizeof(modifier), "Yellow");
    // Red: Magenta and Yellow balanced, Cyan low
    else if (c < 30.0 && m > 70.0 && y > 70.0 && diffMY < 20.0) sprintf_s(modifier, sizeof(modifier), "Red");
    // Orange: Yellow stronger than Magenta
    else if (c < 40.0 && y > 70.0 && m > 35.0 && y > m + 15.0) sprintf_s(modifier, sizeof(modifier), "Orange");
    // Blue: Cyan and Magenta balanced, Yellow low
    else if (c > 65.0 && m > 65.0 && y < 25.0 && diffCM < 20.0) sprintf_s(modifier, sizeof(modifier), "Blue");
    // Navy / blue-cyan: Cyan stronger than Magenta
    else if (c > 60.0 && m > 35.0 && y < 20.0 && c > m + 10.0) sprintf_s(modifier, sizeof(modifier), "Navy");
    // Specialized mixes Olive (High Yellow, Moderate Cyan, Moderate Key)
    else if (y > 60.0 && c > 20.0 && m < 30.0 && y > c && diffCY >= 10.0 && k > 20.0) sprintf_s(modifier, sizeof(modifier), "Olive Drab");
    // Green: Cyan and Yellow balanced, Magenta low
    else if (c > 50.0 && y > 50.0 && m < 30.0 && diffCY <= 10.0) sprintf_s(modifier, sizeof(modifier), "Green");
    // Teal: Cyan stronger than Yellow
    else if (c > 60.0 && y > 30.0 && m < 30.0 && c > y + 15.0) sprintf_s(modifier, sizeof(modifier), "Teal");
    // Plum / Eggplant (High Magenta, Moderate Cyan/Key)
    else if (m > 60.0 && c > 30.0 && y < 30.0 && k > 35.0) sprintf_s(modifier, sizeof(modifier), "Plum");
    // Violet: Magenta stronger than Cyan
    else if (m > 60.0 && c > 40.0 && y < 30.0 && m > c + 10.0) sprintf_s(modifier, sizeof(modifier), "Violet");
    // Muted / Tones 
    else if (isBalancedMix) {
        sprintf_s(modifier, sizeof(modifier), k < 35.0 ? "Neutral Gray" : "Smoky Taupe");
        intensity[0] = '\0';    // clear
    }
    // Light / Pastel / Tints
    else if (maxCmy < 40.0 && k < 35.0) {
        sprintf_s(modifier, sizeof(modifier), "Pale Tint");
        intensity[0] = '\0';    // clear
    }
    // Complex Mixes
    else if (c > 60.0 && m > 60.0 && y > 60.0 && k < 20.0) sprintf_s(modifier, sizeof(modifier), "Composite Hue");
    else if (m > 40.0 && y > 55.0 && y > m + 10.0 && c >= 10.0 && c < 40.0 && k > 25.0) sprintf_s(modifier, sizeof(modifier), "Burnished Umber");

    // modifier and intensity exists
    if (modifier[0] && intensity[0])
        return combineBuffers(intensity, modifier);
    // modifier only exists.
    if (modifier[0])
        return createBuffer(modifier);
    // intensity only exists.
    if (intensity[0])
        return createBuffer(intensity);

    // Fallback, "Composite" instead ?
    return createBuffer("Multi-Ink Hue");
}

