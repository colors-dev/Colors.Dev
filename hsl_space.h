// hsl_space.h

#pragma once

#ifndef HSL_SPACE_H
#define HSL_SPACE_H

// --- Start of "extern C" block ---
#ifdef __cplusplus
extern "C" {
#endif

#include "import_exports.h"
#include "color_types.h"

// Tune this to change how close to white it triggers
static const double COLOR_EPSILON = 0.005f;

/// <summary>
/// Converts an RGB color to its HSL (Hue, Saturation, SaturationNormalized, Lightness, Raw_Lightness) equivalent.
/// </summary>
/// <param name="rgb">RGB Color</param>
/// <returns>HSL struct</returns>
COLORS_DEV_API HslSpace RgbToHsl(RgbColor rgb);

/// <summary>
/// Converts HSL to RGB Color.  -- NOTICE: There are 16,777,216 colors and only 3,600,000 HSL possible values.  
/// This means HSL to RGB will only convert to one of it's possible colors and will not convert to them all.
/// </summary>
/// <param name="hsv">HSV struct</param>
/// <returns>RGB Color</returns>
COLORS_DEV_API RgbColor HslToRgb(HslSpace hsl);

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
COLORS_DEV_API RgbColor GetMonochromaticVariant(RgbColor rgb, double lightnessShift);

// --- End of "extern C" block ---
#ifdef __cplusplus
}
#endif
#endif
