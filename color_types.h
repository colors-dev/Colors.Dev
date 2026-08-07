#pragma once

#ifndef COLORS_DEV_TYPES_H
#define COLORS_DEV_TYPES_H

#include <stdint.h>             // For uint32_t type
typedef uint32_t colors_dev_color32;
typedef double colors_dev_float64;

// This file defines the public data structures for the Colors library.

/// <summary>
/// Color struct definition.
/// </summary>
typedef struct {
    /// <summary>
    /// Represents an alpha (transparency) value. 
    /// 0 - 255, Transparency: 0 = invisible, 255 = solid.
    /// </summary>
    unsigned char alpha;
    /// <summary>
    /// Represents the red component of a color.
    /// 0 - 255, Dark = 0, Bright Red = 255
    /// </summary>
    unsigned char red;
    /// <summary>
    /// Represents the green component of a color.
    /// 0 - 255, Dark = 0, Bright Green = 255
    /// </summary>
    unsigned char green;
    /// <summary>
    /// Represents the blue color component.
    /// 0 - 255, Dark = 0, Bright Blue = 255
    /// </summary>
    unsigned char blue;
} RgbColor;

/// <summary>
/// Contains the results of a triadic color harmony calculation.
/// </summary>
typedef struct {
    /// <summary>
    /// An array of two RgbColor objects representing the triadic colors.
    /// </summary>
    RgbColor colors[2];
} TriadicResults;

/// <summary>
/// Represents the results of a tetradic color scheme calculation 
/// containing three complementary colors.
/// </summary>
typedef struct {
    /// <summary>
    /// An array of four RgbColor objects representing the tetradic colors.
    /// </summary>
    RgbColor colors[3];
} TetradicResults;

/// <summary>
/// Contains the results of an analogous color scheme calculation.
/// </summary>
typedef struct {
    /// <summary>
    /// An array of two RgbColor objects.
    /// </summary>
    RgbColor colors[2]; // [0] is +30, [1] is -30
} AnalogousResults;

/// <summary>
/// Represents a color in linear color space with alpha, red, green, and blue channels.
/// </summary>
typedef struct {
    /// <summary>
    /// A floating-point variable representing the alpha component.
    /// </summary>
    double alpha;
    /// <summary>
    /// A floating-point variable representing a red color component.
    /// </summary>
    double red;
    /// <summary>
    /// A floating-point variable representing a green color component.
    /// </summary>
    double green;
    /// <summary>
    /// A floating-point variable representing a blue color component.
    /// </summary>
    double blue;
} LinearColor;

/// <summary>
/// Represents a color in the CMYK (Cyan, Magenta, Yellow, Key/Black) color space.
/// </summary>
typedef struct {
	/// <summary>
	/// Cyan: representing a cyan color component or value. 
    /// The value typically ranges from 0.0 to 100.0, where 0.0 represents no cyan and 100.0 represents full cyan saturation.
	/// </summary>
	double cyan;
	/// <summary>
	/// Magenta: Representing a magenta value.  
    /// The value typically ranges from 0.0 to 100.0, where 0.0 represents no magenta and 100.0 represents full magenta saturation.
	/// </summary>
	double magenta;
	/// <summary>
	/// Yellow: Absorbs blue light, reflecting red and green. 
    /// The value typically ranges from 0.0 to 100.0, where 0.0 represents no yellow and 100.0 represents full yellow saturation.
	/// </summary>
	double yellow;
	/// <summary>
	/// Key (Black): Adds depth, contrast, and detail to the printed image. The "K" stands for "key" because the 
	/// black plate is crucial for aligning the other color plates during the printing process. 
    /// The value typically ranges from 0.0 to 100.0, where 0.0 represents no black and 100.0 represents full black saturation.
	/// </summary>
	double key;
	/// <summary>
	/// This is the RAW (0.0-1.0) version of key, instead of auto multiplying 100.  
    /// Raw key provides better accuracy when converting to RGB.
	/// </summary>
	double raw_key;
} CmykSpace;

/// <summary>
/// The HSV (Hue, Saturation, Value) aka HSB (Hue, Saturation, Brightness)
/// HSV is also sometimes referred to as HSB, where B stands for Brightness instead of Value, but the meaning and value is the same.
/// This color model is a way of representing colors that is more intuitive for human perception than the RGB (Red, Green, Blue) model.
/// HSV divides color into three components:
/// Hue, which represents the color's base, like red, green, or blue.  The value represented the angle or degree on a RGB color wheel, ranging from 0.00 to 360.00 degrees.
/// Saturation, which indicates the of purity or intensity percentage (%) of the color.  A fully saturated color will be a pure, vibrant shade, while a lower saturation will make the color appear muted or grayscale.
/// Value aka Brightness, represents the brightness percentage (%) of the color, ranging from black(0%) to white(100%).
/// HSV is often represented as a cylinder or cone, with:
/// </summary>
typedef struct {
    /// <summary>
	/// Hue: Represents a hue value. Around the circular base of the RGB (0.0-360.0°) color wheel.
    /// The hue is typically represented as an angle in degrees, ranging from 0.00 to 360.00.
    /// </summary>
    double hue;
    /// <summary>
	/// Saturation: Represents a saturation value. Radius from center to edge of the RGB (0.0-100.0)% color 
    /// wheel and checks raw value, aka "black", unlike HSL satruaction that checks white.
    /// The saturation is typically represented as a percentage, ranging from 0.0% to 100.0%.
    /// </summary>
    double saturation;
    /// <summary>
    /// Value: Height from bottom (black) to top (brightest color) (0.0-100.0%).
    /// </summary>
    double value;
    /// <summary>
    /// Raw_Value: This is the RAW (0.0-1.0) version of value, instead of auto multiplying 100.
    /// Raw value provides better accuracy when converting to RGB.
    /// </summary>
    double raw_value;
} HsvSpace;

/// <summary>
/// Defines a color using the HSL (Hue, Saturation, Lightness) color model.  
/// HSL is a cylindrical-coordinate representation of the RGB color space designed to be more intuitive for human perception than RGB.  
/// * Components:  
/// - Hue (H): The type of color (e.g., red, green) on a 0-360° wheel.  
/// - Saturation (S): The "purity" or intensity of the color (0-100%).  
/// - Lightness (L): The "brightness" or "luminance" of the color (0-100%).  
/// * Interpretation:  
/// HSL's primary advantage is its symmetrical Lightness scale, which aligns with the artistic concepts of tints (adding white) and shades (adding black).  
/// - Lightness = 0%: Always results in black.  
/// - Lightness = 50%: Represents the "purest," most saturated version of the hue.  
/// - Lightness = 100%: Always results in white.  
/// - Saturation = 0%: Results in a shade of gray (from black to white, determined by Lightness).  
/// * This model is ideal for creating color palettes by adjusting Lightness to generate predictable tints and shades, or by adjusting Saturation to create tones.
/// </summary>
typedef struct {
    /// <summary>
    /// Hue: Represents a hue value. Around the circular base of the RGB (0.0-360.0°) color wheel.
    /// The hue is typically represented as an angle in degrees, ranging from 0.00 to 360.00.
    /// </summary>
    double hue;
    /// <summary>
    /// Saturation: The "purity" or intensity of the color (0-100%) and checks raw lightness, 
    /// aka "white", unlink HSV Saturation that checks black.
    /// </summary>
    double saturation;
    /// <summary>
    /// Lightness: The "brightness" or "luminance" of the color (0-100%).
	/// The lightness component in HSL ranges from 0% to 100%, where 0% represents black, 50% represents the pure hue, and 100% represents white.
    /// </summary>
    double lightness;
    /// <summary>
	/// Stores the raw lightness value. This is the unscaled version of lightness, typically ranging from 0.0 to 1.0, 
    /// where 0.0 corresponds to 0% lightness (black) and 1.0 corresponds to 100% lightness (white).
    /// </summary>
    double raw_lightness;
} HslSpace;

/// <summary>
/// The CIE XYZ color space is one of the earliest mathematical representation of all colors visible to the human eye, serving as a standard reference for other color spaces. 
/// It's defined by three imaginary primary colors (X, Y, and Z), where Y represents luminance (brightness), and X and Z represent chromaticity (color). 
/// This space is device-independent, meaning it can be used to describe colors without being tied to a specific display or printing technology. 
/// The XYZ color space serves as a fundamental reference for many other color spaces, but distances in this space do not directly reflect perceived color differences as accurately as LAB or LUV. 
/// The XYZ color space is often used as a reference to convert from one color space to another, making it a crucial tool in color management.
/// </summary>
typedef struct {
    /// <summary>
    /// The 'X' component is associated with the red-green axis, while the 'Z' component is 
    /// associated with the blue-yellow axis. Both 'X' and 'Z' values typically range from 0.0 to 100.0, 
    /// where higher values indicate more intense colors along their respective axes.
    /// </summary>
    double x;
    /// <summary>
	/// 'Y' specifically represents the luminance or perceived brightness of the color. 
    /// The 'Y' component is crucial for determining how light or dark a color appears to 
    /// the human eye, and it ranges from 0.0 (black) to 100.0 (white).
    /// </summary>
    double y;
    /// <summary>
    /// 'Z' is associated and more sensitive to blue light. Higher 'Y' values (for brightness) relative 
    /// to 'X' values are characteristic of light blue colors while higher 'Z' values indicate a 
    /// stronger blue component in the color.
    /// </summary>
    double z;
} XyzSpace;

/// <summary>
/// Represents a color in the CIELAB (L*a*b*) color space.
/// </summary>
typedef struct {
    /// <summary>
    /// 'L' (Lightness): Similar to LCH, this confirms the color is very light. The 'L' component in LAB ranges from 0 (black) to 100 (white).
    /// </summary>
    double l;
    /// <summary>
	/// 'A' (Green–Red axis): This component indicates the position of the color between green and red.  Negative values indicate green, 
    /// while positive values indicate red. The 'A' component 
    /// typically ranges from -128 to 127.
    /// </summary>
    double a;
    /// <summary>
    /// 'B' (Blue–Yellow axis): This component indicates the position of the color between blue and yellow.  Negative values indicates a 
    /// shift towards blue. A small negative value here means a very subtle blue tint.  The combination of these small negative 'a' and 'b' 
    /// values suggests a color that is very close to neutral gray, but with a slight leaning towards the blue-green side, which is 
    /// characteristic of a pale azure.
    /// </summary>
    double b;
} LabSpace;

/// <summary>
/// LUV is derived from the CIE 1931 XYZ color space through specific mathematical transformations 
/// LUV represents color using three components: L* (lightness), u* (chromaticity), and v* (chromaticity).
/// </summary>
typedef struct {
    /// <summary>
    /// 'L' (Lightness): Represents the perceived brightness or lightness of a color, ranging from 0 (black) to 100 (white).
    /// </summary>
    double l;
    /// <summary>
    /// 'U' (Red-Green Chromaticity): Represents the color’s position on a red-green axis, where positive values indicate a 
	/// shift towards red and negative values indicate a shift towards green. 
    /// The 'U' component typically ranges from -100 to 100, with values near zero indicating a more neutral color.
    /// </summary>
    double u;
    /// <summary>
    /// 'V' (Blue-Yellow Chromaticity): Represents the color’s position on a blue-yellow axis, where positive values indicate a shift 
	/// towards yellow and negative values indicate a shift towards blue. 
    /// The 'V' component typically ranges from -100 to 100, with values near zero indicating a more neutral color.
    /// </summary>
    double v;
} LuvSpace;

/// <summary>
/// LCH is a color space that represents colors using three coordinates: Lightness, Chroma, and Hue.  It's a cylindrical transformation of 
/// the CIELAB color space, meaning it uses polar coordinates (chroma and hue) instead of the Cartesian coordinates(a * and b*) of CIELAB.LCH 
/// is designed to be more perceptually uniform, meaning that changes in the coordinate values correspond more closely to how humans perceive 
/// color differences.
/// </summary>
typedef struct {
    /// <summary>
	/// 'L' (Lightness): This indicates that Azure is an extremely light color, very close to white. This aligns with its visual appearance as a pale blue. 
    /// The 'L' component in LCH ranges from 0 (black) to 100 (white), and a value of 97.5 suggests that Azure is almost at the maximum lightness, 
    /// contributing to its soft and airy look.
    /// </summary>
    double l;
    /// <summary>
	/// 'C' (Chroma): This value signifies very low chroma or intensity. It means the color is not vivid or saturated, contributing to its soft and muted look. 
    /// A chroma of 0 would be a pure gray. A chroma of 2.5 indicates that Azure has only a slight hint of color, which is consistent with its 
    /// appearance as a pale blue.
    /// </summary>
    double c;
    /// <summary>
	/// 'H' (Hue): This angle places Azure in the blue-cyan region of the color wheel. Hue is an angle from 0 to 360 degrees, where blue is 
    /// approximately 270 degrees and cyan is around 180 degrees. So, 199 degrees accurately reflects Azure as a light blue with a hint of cyan. 
    /// This hue value confirms that Azure is indeed a cool color, leaning towards the blue-green side of the spectrum.<br/>
    /// LCH Hue values are derived from the CIELAB color space, which is based on human color perception and the XYZ reference color space.<br/>
    /// HSL and HSV Hue values are derived from the RGB color wheel.  Example: HSL's hue starts at pure red, while LCH's hue starts at a pinkish red.
    /// </summary>
    double h;
} LchSpace;

/// <summary>
/// Structured enum/array of a few hundred universally standardized anchor colors (combining standard W3C Web colors and ISCC-NBS blocks):
/// Represents a color in the CIELAB (L*a*b*) color space.
/// </summary>
typedef struct {
    /// <summary>
    /// Working for a color name.
    /// </summary>
    const char* name;
    /// <summary>
    /// 'L' (Lightness): Similar to LCH, this confirms the color is very light. The 'L' component in LAB ranges from 0 (black) to 100 (white).
    /// </summary>
    double l;
    /// <summary>
    /// 'A' (Green–Red axis): This component indicates the position of the color between green and red.  Negative values indicate green, 
    /// while positive values indicate red. The 'A' component 
    /// typically ranges from -128 to 127.
    /// </summary>
    double a;
    /// <summary>
    /// 'B' (Blue–Yellow axis): This component indicates the position of the color between blue and yellow.  Negative values indicates a 
    /// shift towards blue. A small negative value here means a very subtle blue tint.  The combination of these small negative 'a' and 'b' 
    /// values suggests a color that is very close to neutral gray, but with a slight leaning towards the blue-green side, which is 
    /// characteristic of a pale azure.
    /// </summary>
    double b;
} VettedColor;

/// <summary>
/// real-world entities (like plants, animals, and consumer products), colors are rarely a single, flat hex code. 
/// A "Tomato" or "Lava" has a spectrum. We can calculate the perfect linguistic name by treating the dataset 
/// as scored probability coordinates instead of fixed lookups.
/// </summary>
typedef struct {
    /// <summary>
    /// Linguistic name
    /// </summary>
    const char* name;
    /// <summary>
    /// e.g., your percentage column (38.98% for Chili Pepper)
    /// </summary>
    double target_value;
    /// <summary>
    /// The lowest acceptable bound for this name
    /// </summary>
    double range_min;
    /// <summary>
    /// The highest acceptable bound for this name
    /// </summary>
    double range_max;
    /// <summary>
    /// Real-world intensity score (Chili is high chroma, Barn Silo is low)
    /// </summary>
    double typical_chroma;
} PhysicalAnchor;

/// <summary>
/// Define the 4 structural buckets matching your document
/// </summary>
typedef enum {
    BUCKET_LIGHTEST_PINKS,
    BUCKET_BRIGHT_CLASSIC,
    BUCKET_MID_TO_DEEP,
    BUCKET_DARKEST_CRIMSON
} ColorBucket;

typedef struct {
    const char* name;
    ColorBucket bucket;
    double target_percentage; // The sorted position from your spreadsheet
    double primary_hue;       // Expecting ~0°/360° for red, ~120° for green, etc.
    double min_saturation;    // Low for "Barn Silo", Max for "Poison Arrow Frog"
} PhysicalColorNode;


#endif // COLORS_DEV_TYPES_H