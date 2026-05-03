using Chizl.ConsoleSupport;
using System.Text;
using static ColorApi;

namespace CSharpConsole
{
    internal class Program
    {
        const string welcomeToDemo = " Welcome to the .NET Console Color Demo! ";
        // column min width, this is adding the column number at the start of each line with padding.
        const int _columnMinWidth = 64;

        // some stress test colors, and some common ones.
        static readonly RgbColor rgbNearPureBlack = new RgbColor { alpha = 255, red = 1, green = 3, blue = 2 };
        static readonly RgbColor rgbNearPureWhite = new RgbColor { alpha = 255, red = 254, green = 252, blue = 253 };
        static readonly RgbColor rgbVividMagenta = new RgbColor { alpha = 255, red = 255, green = 0, blue = 255 };
        static readonly RgbColor rgbSmokyTaupe = new RgbColor { alpha = 255, red = 127, green = 129, blue = 128 };
        static readonly RgbColor rgbDeepNavy = new RgbColor { alpha = 255, red = 18, green = 52, blue = 86 };
        static readonly RgbColor rgbOrange = new RgbColor { alpha = 255, red = 255, green = 128, blue = 0};

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

        static readonly (RgbColor bgColor, RgbColor fgColor, string title)[] _listOfColors = {
           (rgbOrange, rgbBlack, "Pure Orange"),
           (rgbBlue, rgbWhite, "Pure Blue"),
           (rgbViolet, rgbWhite, "Violet"),
           (rgbCyan, rgbBlack, "Cyan"),
           (rgbAshRose, rgbWhite, "Ash Rose"),
           (rgbNearPureBlack, rgbWhite, "Near Pure Black"),
           (rgbDeepNavy, rgbWhite, "Deep Navy"),
           (rgbNearPureWhite, rgbBlack, "Near Pure White")
        };

        static void Main(string[] args)
        {
            VT.Enable();
            AnyKey("This console buffer will be clear on any key being pressed.", rgbCyan);
            ClearBuffer();

            ConsoleColorPrintDemo();
            ColorConversionDemo();

            // with color, use NULL instead for no color.
            AnyKey("Press any key to continue...", rgbYellow);
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
            ClearBuffer();

            (int row, int col) setRowCol = (0, 0);
            (int row, int col) lastRowCol = (0, 0);

            var maxBufferHeight = _listOfColors.Length * 60;

            if (Console.BufferWidth < 240)
            {
                Console.WindowWidth = 240;
                if (OperatingSystem.IsWindows())
                    Console.BufferWidth = 240;
            }
            if (Console.BufferHeight < maxBufferHeight)
            {
                Console.WindowHeight = maxBufferHeight;
                if (OperatingSystem.IsWindows())
                    Console.BufferHeight = maxBufferHeight;
            }

            foreach (var (bgColor, fgColor, title) in _listOfColors)
                ShowColorInfo(bgColor, fgColor, title);
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

            var ahex = GetRgbHex(clr, true);
            var hex = GetRgbHex(clr, false);

            int dec = unchecked((int)RgbToRgbDec(clr));
            int aDec = unchecked((int)RgbToArgbDec(clr));

            var hsv_rev = HsvToRgb(hsv);
            var hsl_rev = HslToRgb(hsl);
            var cmyk_rev = CmykToRgb(cmyk);

            var cc = GetComplementary(clr);

            double ratio = GetContrastRatio(clr, textClr);
            var suggestTextClr = GetIdealTextColor(clr);
            var colorTone = GetColorTone(clr);
            var cmykModifier = GetCmykModifier(cmyk);
            var colorTemp = GetColorTemp(clr);
            var triad = GetTriadicColors(clr);
            var tetrad = GetTetradicColors(clr);
            var analogous = GetAnalogousColors(clr);
            var colNum = 1;

            sb.AppendLine($" {colNum++:00}:{pad} --- Testing {title} - {hex} ---"); // c({col}), r({row})
            sb.AppendLine($" {colNum++:00}:{pad}'{title}' Color: (R:{clr.red}, G:{clr.green}, B:{clr.blue})");
            sb.AppendLine($" {colNum++:00}:{pad} - HSV: H:{hsv.hue:0.00}, S:{hsv.saturation:0.00}, V:{hsv.value:0.00}, Raw:{hsv.raw_value:0.000000}");
            sb.AppendLine($" {colNum++:00}:{pad} - HSL: H:{hsl.hue:0.00}, S:{hsl.saturation:0.00}, L:{hsl.lightness:0.00}, Raw:{hsl.raw_lightness:0.000000}");
            sb.AppendLine($" {colNum++:00}:{pad} - CMYK: C:{cmyk.cyan:0.00}, M:{cmyk.magenta:0.00}, Y:{cmyk.yellow:0.00}, K:{cmyk.key:0.00}, Raw:{cmyk.raw_key:0.000000}");
            sb.AppendLine($" {colNum++:00}:{pad} - CMYK Modifier: {cmykModifier}");
            sb.AppendLine($" {colNum++:00}:{pad} - XYZ: X:{xyz.x:0.00}, Y:{xyz.y:0.00}, Z:{xyz.z:0.00}");
            sb.AppendLine($" {colNum++:00}:{pad} - LAB_D64: L:{lab64.l:0.0000}, A:{lab64.a:0.0000}, B:{lab64.b:0.0000}");
            sb.AppendLine($" {colNum++:00}:{pad} - LABFull: L:{labFull.l:0.0000}, A:{labFull.a:0.0000}, B:{labFull.b:0.0000}");
            sb.AppendLine($" {colNum++:00}:{pad} - LUV_D64: L:{luv64.l:0.0000}, U:{luv64.u:0.0000}, V:{luv64.v:0.0000}");
            sb.AppendLine($" {colNum++:00}:{pad} - LUVFull: L:{luvFull.l:0.0000}, U:{luvFull.u:0.0000}, V:{luvFull.v:0.0000}");
            sb.AppendLine($" {colNum++:00}:{pad} - LCH_D64: L:{lch64.l:0.0000}, U:{lch64.c:0.0000}, V:{lch64.h:0.0000}");
            sb.AppendLine($" {colNum++:00}:{pad} - LCHFull: L:{lchFull.l:0.0000}, U:{lchFull.c:0.0000}, V:{lchFull.h:0.0000}");
            sb.AppendLine($" {colNum++:00}:{pad} - Complementary Color: (R:{cc.red}, G:{cc.green}, B:{cc.blue})");
            sb.AppendLine($" {colNum++:00}:{pad} - ContrastRatio: {ratio:0.00} - {((ratio < 4.5)?"Alert":"Good")}");
            sb.AppendLine($" {colNum++:00}:{pad} - Suggested Text Color: {suggestTextClr}");
            sb.AppendLine($" {colNum++:00}:{pad} - Tone: {colorTone}");
            sb.AppendLine($" {colNum++:00}:{pad} - Temperature: {colorTemp}"); 
            
            sb.AppendLine($" {colNum++:00}:{pad} - HEX8: {ahex}, Dec: {aDec}");
            sb.AppendLine($" {colNum++:00}:{pad} - HEX6: {hex},   Dec: {dec}");

            sb.AppendLine($" {colNum++:00}:{pad} - HSV Roundtrip -> RGB: (R:{hsv_rev.red}, G:{hsv_rev.green}, B:{hsv_rev.blue})");
            sb.AppendLine($" {colNum++:00}:{pad} - HSL Roundtrip -> RGB: (R:{hsl_rev.red}, G:{hsl_rev.green}, B:{hsl_rev.blue})");
            sb.AppendLine($" {colNum++:00}:{pad} - CMYK Roundtrip -> RGB: (R:{cmyk_rev.red}, G:{cmyk_rev.green}, B:{cmyk_rev.blue})");

            CreateString($" {colNum++:00}:{pad} - Analogous: (R:{clr.red}, G:{clr.green}, B:{clr.blue})", analogous, ref sb);
            colNum += analogous.Length;
            CreateString($" {colNum++:00}:{pad} - Triadic: (R:{clr.red}, G:{clr.green}, B:{clr.blue})", triad, ref sb);
            colNum += triad.Length;
            CreateString($" {colNum++:00}:{pad} - Tetradic: (R:{clr.red}, G:{clr.green}, B:{clr.blue})", tetrad, ref sb);
            colNum += tetrad.Length;

            var allLines = sb.ToString().Replace("\r", "").Split('\n');
            var width = GetWidth(allLines, _columnMinWidth, 1);

            WriteLines(clr, textClr, sb.ToString().Replace("\r", "").Split('\n'), width);
        }

        static void CreateString(string baseStr, RgbColor[] colors, ref StringBuilder sb)
        {
            sb.AppendLine(baseStr);
            var spacePad = new string(' ', Math.Max(baseStr.IndexOf("("), 0));
            foreach (var val in colors)
                sb.AppendLine($"{spacePad}(R:{val.red}, G:{val.green}, B:{val.blue})");
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

        static int GetWidth(string[] msgs, int minWidth, int addToLen = 0)
        {
            var maxLen = 0;

            // get the max length of each trimmed string array.
            foreach (var msg in msgs)
                maxLen = Math.Max(maxLen, msg.Trim().Length);

            // add before checking minWidth
            maxLen += addToLen;

            // ensure we meet specified minimum width.
            if (maxLen < minWidth) maxLen = minWidth;

            // return the max length of the string array or the
            // specified minimum width, whichever is greater
            return maxLen;
        }

        static void Write(RgbColor bg, RgbColor fg, string msg, params object[] args) 
            => PrintWithColor(bg, fg, string.Format(msg, args));

        static void WriteLine(RgbColor bg, RgbColor fg, string msg, params object[] args)
            => PrintWithColor(bg, fg, string.Format($"{msg}\n", args));

        static int WriteLines(RgbColor bg, RgbColor fg, string[] msgs, int minWidth = 0)
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
    }
}
