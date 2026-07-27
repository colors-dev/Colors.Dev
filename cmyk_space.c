// cmyk_conversions.c
#include "cmyk_space.h"
#include "common.h"             // For clampInt, clampDbl
#include <string.h>             // For strlen, strcpy_s
#include <math.h>               // For fmin, fmax, fabs, round, pow

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
    double c = cmyk.cyan;
    double m = cmyk.magenta;
    double y = cmyk.yellow;
    double k = cmyk.key;

    char keyName[10] = { 0 };
    char modifier[22] = { 0 };

    // 1. PAPER WHITE (Top priority)
    if (c < 5.0 && m < 5.0 && y < 5.0 && k < 5.0)
        return createBuffer("Paper White");

    // 2. High Black (K) values
    if (k > 95.0) return createBuffer("Deep Inky Black");
    if (k > 85.0) sprintf_s(keyName, sizeof(keyName), " Blackened");
    else if (k > 60.0) sprintf_s(keyName, sizeof(keyName), " Darkened");

    // 3. Vivids 
    if (c > 85.0 && m < 40.0 && y < 40.0) sprintf_s(modifier, sizeof(modifier), "Vivid Cyan");
    else if (m > 85.0 && c < 60.0 && y < 40.0) sprintf_s(modifier, sizeof(modifier), "Vivid Magenta");
    else if (c > 85.0 && m < 50.0 && y < 40.0) sprintf_s(modifier, sizeof(modifier), "Vivid Navy");
    else if (y > 85.0 && c < 40.0 && m < 40.0) sprintf_s(modifier, sizeof(modifier), "Vivid Yellow");

    // 4. Strong single CMY
    else if (c > 70.0 && m < 30.0 && y < 30.0) sprintf_s(modifier, sizeof(modifier), "Strong Cyan");
    else if (m > 70.0 && c < 30.0 && y < 30.0) sprintf_s(modifier, sizeof(modifier), "Strong Magenta");
    else if (y > 70.0 && c < 30.0 && m < 30.0) sprintf_s(modifier, sizeof(modifier), "Strong Yellow");

    // 5. Dominant CMY combinations
    else if (c > 75.0 && m > 75.0 && y < 15.0) sprintf_s(modifier, sizeof(modifier), "Deep Blue");
    else if (c > 50.0 && m > 50.0 && y < 30.0) sprintf_s(modifier, sizeof(modifier), "Deep Blue Violet");
    else if (c > 50.0 && m > 25.0 && y == 0.0) sprintf_s(modifier, sizeof(modifier), "Deep Navy");
    else if (c > 50.0 && y > 50.0 && m < 30.0) sprintf_s(modifier, sizeof(modifier), "Rich Green");
    else if (m > 50.0 && y > 50.0 && c < 30.0) sprintf_s(modifier, sizeof(modifier), "Fiery Red Orange");

    if (strlen(keyName) > 0 && strlen(modifier) > 0)
        return combineBuffers(modifier, keyName);
    // Fall back to generic rich blackened if no strong color dominance
    else if (strlen(keyName) > 0)
        return createBuffer(keyName);
    else if (strlen(modifier) > 0)
        return createBuffer(modifier);

    // 6. Deep Teal / Forest (High Cyan & Yellow, Low Magenta, Moderate Key)
    if (c > 60.0 && y > 40.0 && m < 30.0 && k > 20.0)
        return createBuffer("Deep Teal");

    // 7. Olive (High Yellow, Moderate Cyan, Moderate Key)
    if (y > 60.0 && c > 20.0 && m < 30.0 && k > 20.0)
        return createBuffer("Olive Drab");

    // 8. Plum / Eggplant (High Magenta, Moderate Cyan/Key)
    if (m > 60.0 && c > 30.0 && y < 30.0 && k > 20.0)
        return createBuffer("Deep Plum");

    // 9. Muted / Tones 
    double diffCM = fabs(c - m);
    double diffMY = fabs(m - y);
    if (diffCM < 15.0 && diffMY < 15.0) {
        if (k < 35.0) return createBuffer("Neutral Gray");
        return createBuffer("Smoky Taupe");
    }

    // 10. Light / Pastel / Tints
    double maxCMY = fmax(c, fmax(m, y));
    if (maxCMY < 40.0 && k < 35.0) {
        if (diffCM < 15.0 && diffMY < 15.0) return createBuffer("Pale Gray Tint");
        return createBuffer("Pale Tint");
    }

    // 11. Complex Mixes
    if (c > 60.0 && m > 60.0 && y > 60.0 && k < 20.0) return createBuffer("Bright Composite Hue");
    if (m > 40.0 && y > 40.0 && c < 40.0 && k > 25.0) return createBuffer("Burnished Umber");

    // 12. Fallthrough
    return createBuffer("Multi-Ink Hue");
}

