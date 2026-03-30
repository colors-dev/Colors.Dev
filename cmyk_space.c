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

//*
COLORS_DEV_API char* GetCymkMod(CmykSpace cmyk)
{
    double c = cmyk.cyan;
    double m = cmyk.magenta;
    double y = cmyk.yellow;
    double k = cmyk.key;

    // 1. PAPER WHITE (Top priority)
    if (c < 5.0 && m < 5.0 && y < 5.0 && k < 5.0)
        return createBuffer("Paper White");

    // 2. High Black (K) values
    if (k > 95.0) return createBuffer("Deep Inky Black");
    if (k > 85.0) return createBuffer("Rich Blackened");
    if (k > 60.0) return createBuffer("Darkened");

    // 3. Vivids 
    if (c > 85.0 && m < 40.0 && y < 40.0) return createBuffer("Vivid Cyan");
    if (m > 85.0 && c < 60.0 && y < 40.0) return createBuffer("Vivid Magenta");
    if (y > 85.0 && c < 40.0 && m < 40.0) return createBuffer("Vivid Yellow");

    // 4. Strong single CMY
    if (c > 70.0 && m < 30.0 && y < 30.0) return createBuffer("Strong Cyan");
    if (m > 70.0 && c < 30.0 && y < 30.0) return createBuffer("Strong Magenta");
    if (y > 70.0 && c < 30.0 && m < 30.0) return createBuffer("Strong Yellow");

    // 5. Dominant CMY combinations
    if (c > 50.0 && m > 50.0 && y < 30.0) return createBuffer("Deep Blue Violet");
    if (c > 50.0 && y > 50.0 && m < 30.0) return createBuffer("Rich Green");
    if (m > 50.0 && y > 50.0 && c < 30.0) return createBuffer("Fiery Red Orange");

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
/**/

/*
COLORS_DEV_API char* GetCmykMod(CmykSpace cmyk)
{
    double cyan = cmyk.cyan;
    double magenta = cmyk.magenta;
    double yellow = cmyk.yellow;
    double key = cmyk.key;

    // High Black (K) values
    if (key > 95.0f)
        return createBuffer("Deep Inky Black");
    if (key > 85.0f)
        return createBuffer("Rich Blackened");
    if (key > 70.0f)
        return createBuffer("Darkened");

    // High single CMY values with low others
    if (cyan > 90.0f && magenta < 15.0f && yellow < 15.0f)
        return createBuffer("Vivid Cyan");
    if (magenta > 90.0f && cyan < 15.0f && yellow < 15.0f)
        return createBuffer("Vivid Magenta");
    if (yellow > 90.0f && cyan < 15.0f && magenta < 15.0f)
        return createBuffer("Vivid Yellow");

    if (cyan > 70.0f && magenta < 20.0f && yellow < 20.0f)
        return createBuffer("Strong Cyan");
    if (magenta > 70.0f && cyan < 20.0f && yellow < 20.0f)
        return createBuffer("Strong Magenta");
    if (yellow > 70.0f && cyan < 20.0f && magenta < 20.0f)
        return createBuffer("Strong Yellow");

    // Dominant CMY combinations
    if (cyan > 60.0f && magenta > 60.0f && yellow < 20.0f)
        return createBuffer("Deep Blue Violet");
    if (cyan > 60.0f && yellow > 60.0f && magenta < 20.0f)
        return createBuffer("Rich Green");
    if (magenta > 60.0f && yellow > 60.0f && cyan < 20.0f)
        return createBuffer("Fiery Red Orange");

    // Muted or balanced tones
    double diffCM = fabs(cyan - magenta);
    double diffMY = fabs(magenta - yellow);

    if (diffCM < 15.0 && diffMY < 15.0) {
        if (key < 35.0) return createBuffer("Neutral Gray"); // Bumped to 35
        if (key >= 35.0) return createBuffer("Smoky Taupe"); // Catch the rest
    }

    // Light or pastel tones (low CMY, low K)
    if (fmax(cyan, fmax(magenta, yellow)) < 30 && key < 10 && fabs(cyan - magenta) < 10 && fabs(magenta - yellow) < 10)
        return createBuffer("Pale Gray Tint");
    if (fmax(cyan, fmax(magenta, yellow)) < 30 && key < 10 && (cyan > 5 || magenta > 5 || yellow > 5))
        return createBuffer("Pale Tint");

    // Almost white, full CMY mix
    if (cyan > 60.0f && magenta > 60.0f && yellow > 60.0f && key < 20.0f)
        return createBuffer("Bright Composite Hue");
    // Or low-K browns
    if (magenta > 40.0f && yellow > 40.0f && cyan < 30.0f && key > 30.0f)
        return createBuffer("Burnished Umber");

    // NEW CATCH-ALL FOR NEAR-ZERO INK
    if (cyan < 5.0 && magenta < 5.0 && yellow < 5.0 && key < 5.0)
        return createBuffer("Paper White");

    return createBuffer("");
}
/**/