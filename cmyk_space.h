// cmyk_conversions.h

#pragma once

#ifndef CMYK_CONVERSIONS_H
#define CMYK_CONVERSIONS_H

// --- Start of "extern C" block ---
#ifdef __cplusplus
extern "C" {
#endif

#include "import_exports.h"
#include "color_types.h"

/// <summary>
/// Converts an RGB color to CMYK color space.
/// </summary>
/// <param name="clr">The RGB color to convert.</param>
/// <returns>The color represented in CMYK color space.</returns>
COLORS_DEV_API CmykSpace RgbToCmyk(RgbColor clr);

/// <summary>
/// Converts a CMYK color to an RGB color.
/// </summary>
/// <param name="cmyk">The CMYK color to convert.</param>
/// <returns>The converted RGB color.</returns>
COLORS_DEV_API RgbColor CmykToRgb(CmykSpace cmyk);

/// <summary>
/// Gets the CMYK modification as a string representation.
/// </summary>
/// <param name="cmyk">The CMYK color space value to convert.</param>
/// <returns>A pointer to a character string representing the CMYK modification.<br/>
/// The caller is responsible for calling FreeAllocPtr to free memory.</returns>
COLORS_DEV_API char* GetCmykMod(CmykSpace cmyk);

// --- End of "extern C" block ---
#ifdef __cplusplus
}
#endif
#endif