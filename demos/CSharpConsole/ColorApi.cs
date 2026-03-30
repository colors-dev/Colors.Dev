using System.Drawing;
using System.Runtime.ConstrainedExecution;
using System.Runtime.InteropServices;

[StructLayout(LayoutKind.Sequential)]
internal struct RgbColor
{
    public byte alpha;
    public byte red;
    public byte green;
    public byte blue;
}

[StructLayout(LayoutKind.Sequential)]
internal struct LinearColor
{
    public double alpha;
    public double red;
    public double green;
    public double blue;
}

[StructLayout(LayoutKind.Sequential)]
internal struct HsvSpace
{
    public double hue;
    public double saturation;
    public double value;
    public double raw_value;
}

[StructLayout(LayoutKind.Sequential)]
internal struct HslSpace
{
    public double hue;
    public double saturation;
    public double lightness;
    public double raw_lightness;
}

[StructLayout(LayoutKind.Sequential)]
internal struct XyzSpace
{
    public double x;
    public double y;
    public double z;
}

[StructLayout(LayoutKind.Sequential)]
internal struct CmykSpace
{
    public double cyan;
    public double magenta;
    public double yellow;
    public double key;
    public double raw_key;
}

[StructLayout(LayoutKind.Sequential)]
public struct LabSpace
{
    public double l;
    public double a;
    public double b;
}

[StructLayout(LayoutKind.Sequential)]
public struct LchSpace
{
    public double l;
    public double c;
    public double h;
}

[StructLayout(LayoutKind.Sequential)]
public struct LuvSpace
{
    public double l;
    public double u;
    public double v;
}

[StructLayout(LayoutKind.Sequential)]
public struct WhitePoint
{
    public double x;
    public double y;
    public double z;
}

public enum WhitePointType : int
{
    WPID_D65 = 0,
    WPID_D65_FULL = 1
}

internal static class ColorApi
{
    private const string DllName = "colors.dev";

    // FUTURE* - Consider using NativeLibrary.SetDllImportResolver to avoid the need for a static constructor
    //           and to allow for more flexible library loading (e.g., from a specific path or with versioning).
    // static ColorApi() => NativeBootstrap.LibaryName = DllName;

    // --- HSV Conversions ---

    [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
    public static extern HsvSpace RgbToHsv(RgbColor rgb);

    [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
    public static extern RgbColor HsvToRgb(HsvSpace hsv);

    [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
    private static extern nint GetTone(RgbColor rgb);
    internal static string GetColorTone(RgbColor rgb)
    {
        nint p = GetTone(rgb);
        try
        {
            //p = GetTone(rgb);
            if (p == 0) return string.Empty;
            return Marshal.PtrToStringUTF8(p) ?? string.Empty;
        }
        finally
        {
            FreeAllocPtr(p);
        }
    }

    [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
    private static extern nint GetTemperature(RgbColor rgb);
    internal static string GetColorTemp(RgbColor rgb)
    {
        nint p = GetTemperature(rgb);
        try
        {
            //p = GetTone(rgb);
            if (p == 0) return string.Empty;
            return Marshal.PtrToStringUTF8(p) ?? string.Empty;
        }
        finally
        {
            FreeAllocPtr(p);
        }
    }

    // --- HSL Conversions ---

    [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
    public static extern HslSpace RgbToHsl(RgbColor rgb);

    [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
    public static extern RgbColor HslToRgb(HslSpace hsl);

    // --- Xyz / Lab / Luv Conversions ---

    [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
    public static extern XyzSpace RgbToXyz(RgbColor rgb);

    [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
    public static extern LabSpace XyzToLabEx(XyzSpace xyz, WhitePointType wp);

    [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
    public static extern LabSpace XyzToLab(XyzSpace xyz);

    [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
    public static extern LuvSpace XyzToLuv(XyzSpace rgb);

    [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
    public static extern LuvSpace XyzToLuvEx(XyzSpace xyz, WhitePointType wp);

    [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
    public static extern LuvSpace RgbToLuv(RgbColor rgb);

    [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
    public static extern LabSpace RgbToLab(RgbColor rgb);

    // -- Linear RGB Conversions ---

    [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
    public static extern LinearColor RgbToLinear(RgbColor rgb);
    
    [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
    public static extern RgbColor LinearToRgb(LinearColor linear);

    // --- Lch Conversions ---

    [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
    public static extern LchSpace LabToLch(LabSpace rgb);

    [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
    public static extern LchSpace RgbToLch(RgbColor rgb);

    // --- Cmyk Conversions ---

    [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
    public static extern CmykSpace RgbToCmyk(RgbColor rgb);

    [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
    public static extern RgbColor CmykToRgb(CmykSpace rgb);

    [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
    private static extern nint GetCmykMod(CmykSpace cmyk);
    internal static string GetCmykModifier(CmykSpace cmyk)
    {
        nint p = GetCmykMod(cmyk);
        try
        {
            if (p == 0) return string.Empty;
            return Marshal.PtrToStringUTF8(p) ?? string.Empty;
        }
        finally
        {
            FreeAllocPtr(p);
        }
    }

    // --- Integer / Decimal Conversions ---

    [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
    public static extern uint RgbToArgbDec(RgbColor rgb);

    [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
    public static extern uint RgbToRgbDec(RgbColor rgb);

    // --- Hex String Conversion (The special one!) ---

    //[return: MarshalAs(UnmanagedType.LPStr)]
    [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
    private static extern nint RgbToRgbHex(RgbColor rgb, [MarshalAs(UnmanagedType.I1)] bool includeAlpha);
    internal static string GetRgbHex(RgbColor rgb, bool includeAlpha)
    {
        nint p = 0;
        try
        {
            p = RgbToRgbHex(rgb, includeAlpha);
            if (p == 0) return string.Empty;
            return Marshal.PtrToStringUTF8(p) ?? string.Empty;
        }
        finally
        {
            FreeAllocPtr(p);
        }
    }

    // --- Set Console Colors by struct ---

    [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
    public static extern void SetColorsEx(RgbColor bg, RgbColor fg);
    [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
    public static extern void SetFgColorEx(RgbColor fg);
    [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
    public static extern void SetBgColorEx(RgbColor bg);

    // --- Set Console Colors by int/uint ---

    [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
    public static extern void SetFgColor(uint red, uint green, uint blue);
    [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
    public static extern void SetBgColor(uint red, uint green, uint blue);

    // --- Reset Foreground and Background Colors ---

    [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
    public static extern void FreeAllocPtr(IntPtr ptr);

    [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
    public static extern void ResetColor();

    [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
    public static extern void ClearBuffer();

    // --- Get Specific Property Methods ---

    [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
    public static extern double GetHue(RgbColor rgb);

    [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
    public static extern double GetHsvSaturation(RgbColor rgb);

    [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
    public static extern double GetHslSaturation(RgbColor rgb);

    [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
    public static extern double GetHslLightness(RgbColor rgb);

    [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
    public static extern double GetHsvBrightness(RgbColor rgb);

    [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
    public static extern double GetRelativeLuminance(RgbColor rgb);

    [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
    public static extern double GetPerceptualBrightness(RgbColor rgb);

    [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
    public static extern double GetContrastRatio(RgbColor a, RgbColor b);

    [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
    private static extern uint GetBestContrastColor(RgbColor background);
    public static Color GetIdealTextColor(RgbColor bg)
    {
        var argb = unchecked((int)GetBestContrastColor(bg));
        if (argb == 0)
            return Color.Black; //.FromArgb(255, 0, 0, 0);
        else
            return Color.White; //.FromArgb(255, 255, 255, 255);
    }
}
