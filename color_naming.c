/* ################[ NOT READY YET ]################ */


/*
#include <math.h>
#include "xyz_space.h"      // RgbToXyz, XyzToLabEx
#include "common.h"         // COLOR_DICTIONARY
#include "white_points.h"   // WhitePointType
#include "color_types.h"    // Your library's header

const WhitePointType d64_Full_Type = WPID_D65_FULL;
const WhitePointType d64_Type = WPID_D65;

// Simple Euclidean Distance in LAB space (CIE76)
double CalculateDeltaE76(LabSpace c1, const VettedColor* c2) {
    return sqrt(pow(c1.l - c2->l, 2) +
        pow(c1.a - c2->a, 2) +
        pow(c1.b - c2->b, 2));
}

// Scans your dictionary for the perceptually closest name
const char* GetNearestColorName(RgbColor rgb) {
    
    // Leverage your existing structural math!
    XyzSpace xyz = RgbToXyz(rgb);
    LabSpace lab = XyzToLabEx(xyz, d64_Full_Type); // Using your precision white point

    double min_distance = 99999.0;
    int closest_index = 0;
    int dict_size = sizeof(COLOR_DICTIONARY) / sizeof(COLOR_DICTIONARY[0]);

    for (int i = 0; i < dict_size; i++) {
        double distance = CalculateDeltaE76(lab, &COLOR_DICTIONARY[i]);
        if (distance < min_distance) {
            min_distance = distance;
            closest_index = i;
        }
    }

    return COLOR_DICTIONARY[closest_index].name;
}

double CalculateMatchPenalty(HsvSpace input_hsv, double current_percentage, PhysicalColorNode anchor) {
    // Value/Lightness difference
    double value_diff = fabs(current_percentage - anchor.target_percentage);

    // Hue directional difference (accounting for 360-degree wrap-around)
    double hue_diff = fabs(input_hsv.hue - anchor.primary_hue);
    if (hue_diff > 180.0) hue_diff = 360.0 - hue_diff;

    // Saturation (Chroma) mapping
    double sat_diff = fabs(input_hsv.saturation - anchor.min_saturation);

    // Total Weighted Penalty (Adjust weights to prioritize value vs saturation)
    return (value_diff * 1.5) + (hue_diff * 1.0) + (sat_diff * 2.0);
}
*/