// cmyk_conversions.c
#include "cmyk_space.h"
#include "hsl_space.h"
#include "common.h"             // For clampInt, clampDbl
#include <math.h>               // For fmin, fmax, fabs, round, pow
#include <stdbool.h>            // bool

COLORS_DEV_API CmykSpace RgbToCmyk(RgbColor rgb)
{
    double r = rgb.red / 255.0;
    double g = rgb.green / 255.0;
    double b = rgb.blue / 255.0;

    double maxVal = fmax(r, fmax(g, b));
    double rawK = 1.0 - maxVal;
    double c = 0.0, m = 0.0, y = 0.0;

    if (maxVal > 1e-12)
    {
        c = (maxVal - r) / maxVal;
        m = (maxVal - g) / maxVal;
        y = (maxVal - b) / maxVal;

        // Safety clamp against precision jitter
        c = fmax(0.0, fmin(1.0, c));
        m = fmax(0.0, fmin(1.0, m));
        y = fmax(0.0, fmin(1.0, y));
    }

    CmykSpace cmyk = {
        c * 100.0,
        m * 100.0,
        y * 100.0,
        rawK * 100.0,
        rawK
    };
    return cmyk;
}

COLORS_DEV_API RgbColor CmykToRgb(CmykSpace cmyk)
{
    double c = clampDbl(cmyk.cyan / 100.0, 0.0, 1.0);
    double m = clampDbl(cmyk.magenta / 100.0, 0.0, 1.0);
    double y = clampDbl(cmyk.yellow / 100.0, 0.0, 1.0);

    // If raw_key matches key / 100 within a tiny epsilon, it was set by RgbToCmyk!
    // Otherwise, raw_key was omitted/zeroed, so trust key / 100.0.
    double kNorm = clampDbl(cmyk.key / 100.0, 0.0, 1.0);
    double k = (fabs((cmyk.raw_key * 100.0) - cmyk.key) < 1e-4 && cmyk.raw_key >= 0.0 && cmyk.raw_key <= 1.0)
        ? cmyk.raw_key
        : kNorm;

    int r = clampInt((int)lround(255.0 * (1.0 - c) * (1.0 - k)), 0, 255);
    int g = clampInt((int)lround(255.0 * (1.0 - m) * (1.0 - k)), 0, 255);
    int b = clampInt((int)lround(255.0 * (1.0 - y) * (1.0 - k)), 0, 255);

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
    char keyTone[32] = { 0 };
    char modifier[32] = { 0 };

    double c = cmyk.cyan;
    double m = cmyk.magenta;
    double y = cmyk.yellow;
    double k = cmyk.key;

    double cmykVals[4];
    cmykVals[0] = c;
    cmykVals[1] = m;
    cmykVals[2] = y;
    cmykVals[3] = k;

    double maxCmy = fmax(c, fmax(m, y));    // aka PigmentLevel
    double minCmy = fmin(c, fmin(m, y));
    double cmySpread = maxCmy - minCmy;     // aka ChromaStrength

    // Which Color has Dominance or the Lack of.
    bool cyanLack = c == minCmy && y != minCmy && m != minCmy;
    bool yellowDom = y == maxCmy && c != maxCmy && m != maxCmy;

    // Pigment
    bool hasStrongPigment = maxCmy > 70.0;
    bool hasSoftPigment = maxCmy < 40.0;

    // bool isBalancedMix = cmySpread < 15.0;
    bool hasStrongDominance = cmySpread >= 35.0;
    bool warmPigment = c < 35.0 && m > 50.0 && y > 55.0;
    bool isNearNeutral = cmySpread < 15.0 || (k >= 30.0 && maxCmy < 35.0 && cmySpread < 25.0);

    // PAPER WHITE (Top priority)
    if (c < 5.0 && m < 5.0 && y < 5.0 && k < 5.0) return createBuffer("Paper White");
    // BLACKED OUT (Top priority)
    if (k > 95.0) return createBuffer("Deep Inky Black");
    
    // COLOR KEY TONES, designed to be slightly different than HSV->GetTone().
    if (k < 8.0)
    {
        if (hasSoftPigment) sprintf_s(keyTone, sizeof(keyTone), "Pale Tint");
        else sprintf_s(keyTone, sizeof(keyTone), hasStrongPigment ? "Vivid" : "Light");
    }
    else if (k < 18.0) sprintf_s(keyTone, sizeof(keyTone), hasStrongPigment ? "Strong" : "Soft");
    else if (k < 30.0) sprintf_s(keyTone, sizeof(keyTone), (warmPigment && hasStrongPigment ? "Fiery" : (hasStrongPigment ? "Bold" : "Muted")));
    else if (k < 45.0) sprintf_s(keyTone, sizeof(keyTone), "Dusky");
    else if (k < 60.0) sprintf_s(keyTone, sizeof(keyTone), (hasStrongDominance ? "Deep" : "Smoky"));  //Drab
    else if (k < 73.0) sprintf_s(keyTone, sizeof(keyTone), (hasStrongDominance ? "Darkened" : "Smoked"));
    else if (k <= 95.0) sprintf_s(keyTone, sizeof(keyTone), "Charcoal");

    // Convert CMYK to RGB
    RgbColor rgb = CmykToRgb(cmyk);
    // Convert RGB to HSL
    HslSpace hsl = RgbToHsl(rgb);

    double h = hsl.hue;
    while (h < 0.0) h += 360.0;
    while (h > 360.0) h -= 360.0;

    // COLOR FAMILY - 24-Step Hue Wheel (Primary, Secondary, Tertiary & Compound Intermediates)
    if (h >= 352.5 || h < 7.5)          sprintf_s(modifier, sizeof(modifier), "Red");                // 01) 0°   - Primary
    else if (h >= 7.5 && h < 22.5)      sprintf_s(modifier, sizeof(modifier), "Red-Orange");         // 02) 15°  - Intermediate
    else if (h >= 22.5 && h < 37.5)     sprintf_s(modifier, sizeof(modifier), "Orange");             // 03) 30°  - Tertiary
    else if (h >= 37.5 && h < 52.5)     sprintf_s(modifier, sizeof(modifier), "Amber");              // 04) 45°  - Standard
    else if (h >= 52.5 && h < 67.5)     sprintf_s(modifier, sizeof(modifier), "Yellow");             // 05) 60°  - Secondary
    else if (h >= 67.5 && h < 82.5)     sprintf_s(modifier, sizeof(modifier), "Yellow-Green");       // 06) 75°  - Intermediate
    else if (h >= 82.5 && h < 97.5)     sprintf_s(modifier, sizeof(modifier), "Chartreuse");         // 07) 90°  - Tertiary
    else if (h >= 97.5 && h < 112.5)    sprintf_s(modifier, sizeof(modifier), "Lime Green");         // 08) 105° - Standard
    else if (h >= 112.5 && h < 127.5)   sprintf_s(modifier, sizeof(modifier), "Green");              // 09) 120° - Primary
    else if (h >= 127.5 && h < 142.5)   sprintf_s(modifier, sizeof(modifier), "Spring Green");       // 10) 135° - Intermediate
    else if (h >= 142.5 && h < 157.5)   sprintf_s(modifier, sizeof(modifier), "Mint Green");         // 11) 150° - Tertiary
    else if (h >= 157.5 && h < 172.5)   sprintf_s(modifier, sizeof(modifier), "Blue-Green");         // 12) 165° - Intermediate
    else if (h >= 172.5 && h < 187.5)   sprintf_s(modifier, sizeof(modifier), "Cyan");               // 13) 180° - Secondary
    else if (h >= 187.5 && h < 202.5)   sprintf_s(modifier, sizeof(modifier), "Sky Blue");           // 14) 195° - Standard
    else if (h >= 202.5 && h < 217.5)   sprintf_s(modifier, sizeof(modifier), "Azure");              // 15) 210° - Tertiary
    else if (h >= 217.5 && h < 232.5)   sprintf_s(modifier, sizeof(modifier), "Cobalt Blue");        // 16) 225° - Standard
    else if (h >= 232.5 && h < 247.5)   sprintf_s(modifier, sizeof(modifier), "Blue");               // 17) 240° - Primary
    else if (h >= 247.5 && h < 262.5)   sprintf_s(modifier, sizeof(modifier), "Blue-Violet");        // 18) 255° - Intermediate
    else if (h >= 262.5 && h < 277.5)   sprintf_s(modifier, sizeof(modifier), "Violet");             // 19) 270° - Tertiary
    else if (h >= 277.5 && h < 292.5)   sprintf_s(modifier, sizeof(modifier), "Purple");             // 20) 285° - Standard
    else if (h >= 292.5 && h < 307.5)   sprintf_s(modifier, sizeof(modifier), "Magenta");            // 21) 300° - Secondary
    else if (h >= 307.5 && h < 322.5)   sprintf_s(modifier, sizeof(modifier), "Pink");               // 22) 315° - Standard
    else if (h >= 322.5 && h < 337.5)   sprintf_s(modifier, sizeof(modifier), "Rose");               // 23) 330° - Tertiary
    else                                sprintf_s(modifier, sizeof(modifier), "Crimson");            // 24) 345° - Intermediate (337.5° - 352.5°)

    // Muted / Tones 
    if (isNearNeutral)
    {
        // override based on neutrual color.
        bool isSilver = maxCmy == 0.0 && k < 25.0;
        bool isTaupe = yellowDom && cyanLack && (m > (y * .25)) && (k > (y * .75));
        bool isGray = !isSilver && !isTaupe && cmySpread <= 3.0;

        // TODO (GWL): tune this threshold --- for later.
        // bool isGray = !isSilver && !isTaupe && cmySpread < 5.0;   
        // bool isGray = cmySpread < 5.0 && hsl.saturation < 8.0;

        if (isSilver)
            sprintf_s(modifier, sizeof(modifier), "Silver");
        else if (isTaupe)
            sprintf_s(modifier, sizeof(modifier), "Taupe");
        else if (isGray)
            sprintf_s(modifier, sizeof(modifier), "Gray");
        // else use the original modifier, the keyTone will cover Smokey Red or Charcoal Red, for example..
    }

    int isSolid = 1;

    for (int d = 0; d < 4; d++) {
        double val = cmykVals[d];
        // Allow 0, 100, and the digital center (49.5 to 50.5), for the next level, like Amber, we need to look at 25 range.
        if (val != 0.0 && val != 100.0 && !(val >= 49.5 && val <= 50.5) && !(val >= 24.5 && val <= 25.5))
        {
            isSolid = 0;
            break;
        }
    }

    // if all are max or min, then it's a solid and we don't need Vivid Red and Vivid Cyan.
    if (isSolid) keyTone[0] = '\0';

    // modifier and keyTone exists
    if (modifier[0] && keyTone[0])
        return combineBuffers(keyTone, modifier);
    // modifier only exists.
    if (modifier[0])
        return createBuffer(modifier);
    // keyTone only exists.
    if (keyTone[0])
        return createBuffer(keyTone);

    // returning "\0", "Composite", or "Multi-Ink Hue".  Still weighing options.
    return createBuffer("\0");   // \0 empty string
}

