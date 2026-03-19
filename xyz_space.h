// xyz_conversions.h

#pragma once

#ifndef XYZ_CONVERSIONS_H
#define XYZ_CONVERSIONS_H

// --- Start of "extern C" block ---
#ifdef __cplusplus
extern "C" {
#endif

#include "import_exports.h"
#include "color_types.h"
#include "white_points.h"       // For white point definitions used in XYZ to Lab conversions.

/// <summary>
/// Converts an RGB color to the Lab color space.<br/>
/// (Default: WP_D65_FULL white point is used for the conversion.)
/// </summary>
/// <param name="rgb">The RGB color to convert.</param>
/// <returns>The color represented in Lab color space.</returns>
COLORS_DEV_API LabSpace RgbToLab(RgbColor rgb);
	
/// <summary>
/// Converts an RGB color to the CIE XYZ color space.
/// </summary>
/// <param name="rgb">The RGB color to convert.</param>
/// <returns>The color represented in the CIE XYZ color space.</returns>
COLORS_DEV_API XyzSpace RgbToXyz(RgbColor rgb);

/// <summary>
/// Converts a color from XYZ color space to Lab color space.<br/>
/// (Default: WP_D65_FULL white point is used for the conversion.)
/// </summary>
/// <param name="xyz">The color in XYZ color space to convert.</param>
/// <returns>The color converted to Lab color space.</returns>
COLORS_DEV_API LabSpace XyzToLab(XyzSpace xyz);

/// <summary>
/// Converts a color from XYZ color space to LAB color space using a specified white point.
/// </summary>
/// <param name="xyz">The color in XYZ color space to convert.</param>
/// <param name="wp">The white point type to use for the conversion.</param>
/// <returns>The color converted to LAB color space.</returns>
COLORS_DEV_API LabSpace XyzToLabEx(XyzSpace xyz, WhitePointType wp);

/// <summary>
/// Calculates the relative luminance of an RGB color. (The "Physics" version)
/// </summary>
/// <param name="clr">The RGB color for which to calculate relative luminance.</param>
/// <returns>The relative luminance value as a 64-bit floating-point number, typically in the range [0.0, 1.0].</returns>
COLORS_DEV_API colors_dev_float64 GetRelativeLuminance(RgbColor rgb);

/// <summary>
/// Calculates the perceptual brightness of an RGB color. (The "Human" version)
/// </summary>
/// <param name="clr">The RGB color to evaluate.</param>
/// <returns>A floating-point value representing the perceptual brightness of the color.</returns>
COLORS_DEV_API colors_dev_float64 GetPerceptualBrightness(RgbColor rgb);

/// <summary>
/// Calculates the contrast ratio between two RGB colors.
/// </summary>
/// <param name="a">The first RGB color.</param>
/// <param name="b">The second RGB color.</param>
/// <returns>The contrast ratio between the two colors as a 64-bit floating-point value.</returns>
COLORS_DEV_API colors_dev_float64 GetContrastRatio(RgbColor a, RgbColor b);

/// <summary>
/// Determines the ideal text color for optimal contrast against a given background color.
/// </summary>
/// <param name="background">The background color to evaluate for text contrast.</param>
/// <returns>Returns 1 if White text is better, 0 if Black text is better.</returns>
COLORS_DEV_API colors_dev_color32 GetBestContrastColor(RgbColor bgColor);

// --- End of "extern C" block ---
#ifdef __cplusplus
}
#endif
#endif