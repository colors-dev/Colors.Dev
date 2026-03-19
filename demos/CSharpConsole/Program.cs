using System.Drawing;
using System.Runtime.InteropServices;
using System.Text;
using static ColorApi;

namespace CSharpConsole
{
    internal class Program
    {
        const string welcomeToDemo = " Welcome to the .NET Console Color Demo! ";
        // some stress test colors, and some common ones.
        static readonly RgbColor rgbNearPureBlack = new RgbColor { alpha = 255, red = 1, blue = 2, green = 3 };
        static readonly RgbColor rgbDeepNavy = new RgbColor { alpha = 255, red = 18, blue = 86, green = 52 };
        static readonly RgbColor rgbNearPureWhite = new RgbColor { alpha = 255, red = 254, blue = 253, green = 252 };

        static readonly RgbColor rgbEmpty = new RgbColor { alpha = 0, red = 0, green = 0, blue = 0 };
        static readonly RgbColor rgbRed = new RgbColor { alpha = 255, red = 255, green = 0, blue = 0 };
        static readonly RgbColor rgbBlue = new RgbColor { alpha = 255, red = 0, green = 0, blue = 255 };
        static readonly RgbColor rgbViolet = new RgbColor { alpha = 255, red = 127, green = 0, blue = 255 };
        static readonly RgbColor rgbYellow = new RgbColor { alpha = 255, red = 255, green = 255, blue = 0 };
        static readonly RgbColor rgbCyan = new RgbColor { alpha = 255, red = 0, green = 255, blue = 255 };
        static readonly RgbColor rgbAshRose = new RgbColor { alpha = 255, red = 181, green = 129, blue = 125 };
        static readonly RgbColor rgbBlack = new RgbColor { alpha = 255, red = 0, green = 0, blue = 0 };
        static readonly RgbColor rgbWhite = new RgbColor { alpha = 255, red = 255, green = 255, blue = 255 };
        static readonly RgbColor rgbConsoleBlack = new RgbColor { alpha = 255, red = 12, green = 12, blue = 12 };

        static void Main(string[] args)
        {
            AnyKey("This console buffer will be clear on any key being pressed.", rgbCyan);
            ClearBuffer();

            ConsoleColorPrintDemo();
            ColorConversionDemo();
            
            // with color, use NULL instead for no color.
            AnyKey("Press any key to exit...", rgbRed);
            ClearBuffer();
        }

        static void SetColors(RgbColor bg, RgbColor fg)
        {
            if (fg.alpha != 0 && bg.alpha != 0)
                SetColorsEx(bg, fg);
            else if (fg.alpha != 0)
                SetFgColorEx(fg);
            else if (bg.alpha != 0)
                SetBgColorEx(bg);
            else
                ResetColor();
        }

        static void ColorConversionDemo()
        {
            ShowColorInfo(rgbBlue, rgbWhite, "Blue");
            ShowColorInfo(rgbViolet, rgbWhite, "Violet");
            ShowColorInfo(rgbCyan, rgbBlack, "Cyan");
            ShowColorInfo(rgbAshRose, rgbWhite, "Ash Rose");
            ShowColorInfo(rgbNearPureBlack, rgbWhite, "Near Pure Black");
            ShowColorInfo(rgbDeepNavy, rgbWhite, "Deep Navy");
            ShowColorInfo(rgbNearPureWhite, rgbBlack, "Near Pure White");
            
            AnyKey("Press any key to continue...", rgbYellow);
        }

        static void ShowColorInfo(RgbColor clr, RgbColor textClr, string title)
        {
            var pad = " ";
            var sb = new StringBuilder();

            var hsv = RgbToHsv(clr);
            var hsl = RgbToHsl(clr);
            var xyz = RgbToXyz(clr);
            var cmyk = RgbToCmyk(clr);

            var labFull = XyzToLabEx(xyz, WhitePointType.WPID_D65_FULL);
            var lab64 = XyzToLabEx(xyz, WhitePointType.WPID_D65);

            var luvFull = XyzToLuvEx(xyz, WhitePointType.WPID_D65_FULL);
            var luv64 = XyzToLuvEx(xyz, WhitePointType.WPID_D65);

            var lchFull = LabToLch(labFull);
            var lch64 = LabToLch(lab64);

            var ahex = PtrToUtf8AndFree(RgbToRgbHex(clr, true));     //converts nint to string and frees the pointer, REQUIRED.
            var hex = PtrToUtf8AndFree(RgbToRgbHex(clr, false));     //converts nint to string and frees the pointer, REQUIRED.

            int dec = unchecked((int)RgbToRgbDec(clr));
            int aDec = unchecked((int)RgbToArgbDec(clr));

            var hsv_rev = HsvToRgb(hsv);
            var hsl_rev = HslToRgb(hsl);
            var cmyk_rev = CmykToRgb(cmyk);

            var background = Color.FromArgb(30, 30, 30);
            var text = Color.White;

            double ratio = GetContrastRatio(clr, textClr);
            var suggestTextClr = GetIdealText(clr);

            sb.AppendLine($"{pad} --- Testing {title} - {hex} Conversions ---");

            sb.AppendLine($"{pad}'{title}' Color: (R:{clr.red}, G:{clr.green}, B:{clr.blue})");
            sb.AppendLine($"{pad} - CMYK: C:{cmyk.cyan:0.00}, M:{cmyk.magenta:0.00}, Y:{cmyk.yellow:0.00}, K:{cmyk.key:0.00}, Raw:{cmyk.raw_key:0.000000}");
            sb.AppendLine($"{pad} - HSV: H:{hsv.hue:0.00}, S:{hsv.saturation:0.00}, V:{hsv.value:0.00}, Raw:{hsv.raw_value:0.000000}");
            sb.AppendLine($"{pad} - HSL: H:{hsl.hue:0.00}, S:{hsl.saturation:0.00}, L:{hsl.lightness:0.00}, Raw:{hsl.raw_lightness:0.000000}");
            sb.AppendLine($"{pad} - XYZ: X:{xyz.x:0.00}, Y:{xyz.y:0.00}, Z:{xyz.z:0.00}");
            sb.AppendLine($"{pad} - LAB_D64: L:{lab64.l:0.0000}, A:{lab64.a:0.0000}, B:{lab64.b:0.0000}");
            sb.AppendLine($"{pad} - LABFull: L:{labFull.l:0.0000}, A:{labFull.a:0.0000}, B:{labFull.b:0.0000}");
            sb.AppendLine($"{pad} - LUV_D64: L:{luv64.l:0.0000}, U:{luv64.u:0.0000}, V:{luv64.v:0.0000}");
            sb.AppendLine($"{pad} - LUVFull: L:{luvFull.l:0.0000}, U:{luvFull.u:0.0000}, V:{luvFull.v:0.0000}");
            sb.AppendLine($"{pad} - LCH_D64: L:{lch64.l:0.0000}, U:{lch64.c:0.0000}, V:{lch64.h:0.0000}");
            sb.AppendLine($"{pad} - LCHFull: L:{lchFull.l:0.0000}, U:{lchFull.c:0.0000}, V:{lchFull.h:0.0000}");
            sb.AppendLine($"{pad} - ContrastRatio: {ratio:0.00} - {((ratio < 4.5)?"Alert":"Good")}");
            sb.AppendLine($"{pad} - Suggested Text Color: {suggestTextClr}");
            sb.AppendLine($"{pad} - HEX8: {ahex}, Dec: {aDec}");
            sb.AppendLine($"{pad} - HEX6: {hex},   Dec: {dec}");

            sb.AppendLine($"{pad} - HSV Roundtrip -> RGB: (R:{hsv_rev.red}, G:{hsv_rev.green}, B:{hsv_rev.blue})");
            sb.AppendLine($"{pad} - HSL Roundtrip -> RGB: (R:{hsl_rev.red}, G:{hsl_rev.green}, B:{hsl_rev.blue})");
            sb.AppendLine($"{pad} - CMYK Roundtrip -> RGB: (R:{hsl_rev.red}, G:{hsl_rev.green}, B:{hsl_rev.blue})");

            WriteLines(clr, textClr, sb.ToString().Replace("\r", "").Split('\n'), 60);
        }

        static void ConsoleColorPrintDemo()
        {
            int num_spaces = (int)(Console.BufferWidth * 0.5) - (int)(welcomeToDemo.Length * 0.5);
            Write(rgbEmpty, rgbEmpty, new string(' ', num_spaces));
            WriteLine(rgbRed, rgbYellow, welcomeToDemo);

            // Set no color.
            WriteLine(rgbEmpty, rgbEmpty, " - showing default color - ");
            // foreground color only.
            WriteLine(rgbEmpty, rgbCyan, "Cyan text only.");
            // foreground color only.
            AnyKey("Press any key to continue...", rgbYellow);
        }

        static void Write(RgbColor bg, RgbColor fg, string msg, params object[] args) 
            => PrintWithColor(bg, fg, string.Format(msg, args));

        static void WriteLine(RgbColor bg, RgbColor fg, string msg, params object[] args)
            => PrintWithColor(bg, fg, string.Format($"{msg}\n", args));

        static int WriteLines(RgbColor bg, RgbColor fg, string[] msgs, int minWidth=0)
        {
            var maxLen = 0;
            foreach (var msg in msgs)
                maxLen = Math.Max(maxLen, msg.Length);

            maxLen++;   // add space to end            
            if (maxLen < minWidth)
                maxLen = minWidth;

            foreach (var msg in msgs)
            {
                var newMsg = msg + new string(' ', maxLen - msg.Length);
                PrintWithColor(bg, fg, newMsg);
                if (msgs.Length > 1)
                    Console.WriteLine();
            }

            return maxLen;
        }

        static void PrintWithColor(RgbColor bg, RgbColor fg, string msg)
        {
            if (string.IsNullOrEmpty(msg))
                return;

            SetColors(bg, fg);
            Console.Write(msg);      // Use threadsafe, and insure not extra % was uncaught.
            ResetColor();   // resets foreground and background color to console default colors.
        }

        static void AnyKey(string msg, RgbColor fg)
        {
            if (!string.IsNullOrWhiteSpace(msg))
            {
                var curLoc = new int[] { Console.CursorLeft, Console.CursorTop };
                WriteLine(rgbEmpty, fg, msg);

                Console.ReadKey(true);
                try
                {
                    //set cursor back to start of message.
                    Console.CursorTop = curLoc[1];
                    Console.CursorLeft = curLoc[0];
                    //write message, but in black, overwriting and keeping CrLf that might exists.
                    WriteLine(rgbEmpty, rgbConsoleBlack, msg);
                    //set cursor back to start of message.
                    Console.CursorTop = curLoc[1];
                    Console.CursorLeft = curLoc[0];
                }
                catch { /*On exit, Console.CursorTop/Left might not be available to set */ }
            }
        }

        static string PtrToUtf8AndFree(nint p)
        {
            if (p == 0) return string.Empty;

            try
            {
                return Marshal.PtrToStringUTF8(p) ?? string.Empty;
            }
            finally
            {
                FreeAllocPtr(p);
            }
        }
    }
}
