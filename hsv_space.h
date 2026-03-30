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
/// Calculates the Hue component of the HSV/HSL from an RGB color.  
/// Hue from both HSV and HSL are always the same value, so this function can be used for both color spaces.
/// </summary>
/// <param name="rgb">The RGB color from which to extract the hue.</param>
/// <returns>The hue value as a 64-bit floating-point number, typically in the range [0, 360) degrees.</returns>
COLORS_DEV_API colors_dev_float64 GetHue(RgbColor rgb);

/// <summary>
/// Calculates the Saturation component of the HSV (Hue, Saturation, Value) color model from an RGB color.
/// </summary>
/// <param name="rgb">The RGB color to extract the saturation from.</param>
/// <returns>The saturation value in the HSV color space, typically in the range [0.0, 1.0].</returns>
COLORS_DEV_API colors_dev_float64 GetHsvSaturation(RgbColor rgb);

/// <summary>
/// Calculates the Brightness of an RGB color from the HSV (Hue, Saturation, Value, Raw_Value) color model.
/// This is the "Raw_Value" component of HSV, which represents the unchanged value "brightness" of the color.
/// </summary>
/// <param name="rgb">The RGB color to calculate brightness for.</param>
/// <returns>The brightness value as a 64-bit floating point number.</returns>
COLORS_DEV_API colors_dev_float64 GetHsvBrightness(RgbColor rgb);

/// <summary>
/// NOTE: Remember to free the returned string using FreeAllocPtr when done.  
/// This function analyzes the HSV components of the RGB color to determine its tone, 
/// which can be described as light, dark, muted, vibrant, etc., based on the 'value'
/// and 'saturation levels. The specific criteria for determining the tone may 
/// vary, but generally:
/// </summary>
/// <param name="rgb">The RGB color value to analyze.</param>
/// <returns>A string describing the tone of the color.</returns>
COLORS_DEV_API char* GetTone(RgbColor clr);

/// <summary>
/// Gets the temperature classification of an RGB color.
/// </summary>
/// <param name="clr">The RGB color to analyze.</param>
/// <returns>A string representing the 16 temperature classifications of the color (e.g., 'warm', 'cool').</returns>
COLORS_DEV_API char* GetTemperature(RgbColor clr);

// --- End of "extern C" block ---
#ifdef __cplusplus
}
#endif
#endif