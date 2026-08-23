// luv_conversions.c

#include "luv_space.h"
#include "xyz_space.h"          // For RgbToLch -> XyzToLab and XyzToLab
// #include "common.h"             // For clampInt, clampDbl
// #include <string.h>             // For strlen, strcpy_s
#include <math.h>               // For fmin, fmax, fabs, round, pow

COLORS_DEV_API LuvSpace XyzToLuvEx(XyzSpace xyz, WhitePointType wp)
{
    double wpX, wpY, wpZ = 0.0;

    switch (wp)
    {
    case WPID_D65_FULL:
        wpX = WP_D65_FULL.x;
        wpY = WP_D65_FULL.y;
        wpZ = WP_D65_FULL.z;
        break;
    case WPID_D65:
    default:
        wpX = WP_D65.x;
        wpY = WP_D65.y;
        wpZ = WP_D65.z;
        break;
    }

    //// Calculate reference white point chromaticity coordinates (u'n, v'n)
    double un_prime = (4 * wpX) / (wpX + (15 * wpY) + (3 * wpZ));
    double vn_prime = (9 * wpY) / (wpX + (15 * wpY) + (3 * wpZ));

    //// Calculate sample chromaticity coordinates (u', v')
    double divisor = (xyz.x + (15 * xyz.y) + (3 * xyz.z));
    double u_prime = (divisor == 0) ? 0 : (4 * xyz.x) / divisor;
    double v_prime = (divisor == 0) ? 0 : (9 * xyz.y) / divisor;

    //// Calculate L*
    const double delta = 6.0 / 29.0;
    const double deltaCubed = (delta * delta * delta); // (6/29)^3
    double l, u, v = 0.0;

    if ((xyz.y / wpY) > deltaCubed) {
        l = 116 * pow(xyz.y / wpY, 1.0 / 3.0) - 16;
    } else {
        // changed for dark tests like #010302, #010202, and #1D0000 should line up with Lab's L* and the expected LUV values.
        l = (29.0 / 3.0) * (29.0 / 3.0) * (29.0 / 3.0) * (xyz.y / wpY);
    }

    // Calculate u* and v*
    u = 13 * l * (u_prime - un_prime);
    v = 13 * l * (v_prime - vn_prime);

    LuvSpace luv = { l, u, v };
    return luv;
}

COLORS_DEV_API LuvSpace XyzToLuv(XyzSpace xyz)
{
    return XyzToLuvEx(xyz, WPID_D65_FULL);      // Default: WP_D65_FULL, WPID_D65 is the other option
}

COLORS_DEV_API LuvSpace RgbToLuv(RgbColor	rgb) 
{
	XyzSpace xyz = RgbToXyz(rgb);
    return XyzToLuvEx(xyz, WPID_D65_FULL);      // Default: WP_D65_FULL, WPID_D65 is the other option
}