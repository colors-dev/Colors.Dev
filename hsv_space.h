// hsv_space.h

#pragma once

#ifndef HSV_SPACE_H
#define HSV_SPACE_H

// --- Start of "extern C" block ---
#ifdef __cplusplus
extern "C" {
#endif

#include "import_exports.h"
#include "color_types.h"

/// <summary>
/// Converts an RGB color to its HSV (Hue, Saturation, Value) equivalent.
/// </summary>
/// <param name="rgb">RGB Color</param>
/// <returns>HSV struct</returns>
COLORS_DEV_API HsvSpace RgbToHsv(RgbColor rgb);

/// <summary>
/// Convert HSV to RGB Color.  -- NOTICE: There are 16,777,216 colors and only 3,600,000 HSV possible values.
/// This means HSV to RGB will only convert to one of it's possible colors and will not convert to them all.
/// </summary>
/// <param name="hsv">HSV struct</param>
/// <returns>RGB Color</returns>
COLORS_DEV_API RgbColor HsvToRgb(HsvSpace hsv);

/// <summary>
/// NOTE: Remember to free the returned string using FreeAllocPtr when done.  
/// This function analyzes the HSV components of the RGB color to determine its tone, 
/// which can be described as light, dark, muted, vibrant, etc., based on the 'value'
/// and 'saturation levels. The specific criteria for determining the tone may 
/// vary, but generally:
/// </summary>
/// <param name="rgb">The RGB color value to analyze.</param>
/// <returns>A string describing the tone of the color.</returns>
COLORS_DEV_API char* GetTone(RgbColor rgb);

/// <summary>
/// Gets the temperature classification of an RGB color.
/// </summary>
/// <param name="clr">The RGB color to analyze.</param>
/// <returns>A string representing the 16 temperature classifications of the color (e.g., 'warm', 'cool').</returns>
COLORS_DEV_API char* GetTemperature(RgbColor rgb);

/// <summary>
/// Gets the complementary color of the specified RGB color.
/// </summary>
/// <param name="clr">The RGB color for which to find the complementary color.</param>
/// <returns>The complementary RGB color.</returns>
COLORS_DEV_API RgbColor GetComplementary(RgbColor rgb);

/// <summary>
/// Generates an analogous color scheme from the given RGB color.
/// </summary>
/// <param name="rgb">The RGB color to generate an analogous color scheme from.</param>
/// <returns>An AnalogousResults object containing the analogous color scheme.</returns>
COLORS_DEV_API AnalogousResults GetAnalogous(RgbColor rgb);

/// <summary>
/// 
/// </summary>
/// <param name="rgb"></param>
/// <returns></returns>
COLORS_DEV_API SplitComplementaryResults GetSplitComplementary(RgbColor rgb);

/// <summary>
/// Computes the triadic color scheme for a given RGB color.
/// </summary>
/// <param name="clr">The RGB color to compute the triadic scheme for.</param>
/// <returns>A TriadicResults object containing the triadic color scheme colors.</returns>
COLORS_DEV_API TriadicResults GetTriadic(RgbColor rgb);

/// <summary>
/// Computes a tetradic (three-color) color scheme, without original, based on the given RGB color.
/// </summary>
/// <param name="rgb">The RGB color to use as the basis for the tetradic color scheme.</param>
/// <returns>A TetradicResults object containing the three colors that form a tetradic relationship with the input color.</returns>
COLORS_DEV_API TetradicResults GetTetradic(RgbColor rgb);

/// <summary>
/// Calculates a specific, colored contrast match that meets a target WCAG relative luminance ratio 
/// against the base color, bypassing the standard binary black-or-white fallback.
/// </summary>
/// <param name="base">The original RGB color to evaluate against.</param>
/// <param name="targetRatio">The desired contrast ratio to achieve (e.g., 4.5 for WCAG AA normal text).</param>
/// <param name="targetHue">The hue angle (0.0-360.0) to lock in for the resulting contrast color.</param>
/// <param name="targetSat">The saturation percentage (0.0-100.0) to lock in for the resulting contrast color.</param>
/// <returns>An RGB color mathematically adjusted along the lightness axis to achieve the target contrast ratio.</returns>
COLORS_DEV_API RgbColor GenerateContrastColor(RgbColor base, double targetRatio, double targetHue, double targetSat);

// --- End of "extern C" block ---
#ifdef __cplusplus
}
#endif
#endif