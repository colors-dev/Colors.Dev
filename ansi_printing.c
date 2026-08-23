// ansi_printing.c
// #include <string.h>             // For strlen, strcpy_s
#include <stdio.h>              // Required for sprintf
#include "ansi_printing.h"
#include "common.h"

static void PrintAnsiColor(int isFg, unsigned char r, unsigned char g, unsigned char b)
{
    char code[50];
    
    sprintf_s(code, sizeof(code), "\x1b[%d;2;%u;%u;%um", 
        isFg ? 38 : 48, 
        clampUChr(r, 0, 255), 
        clampUChr(g, 0, 255), 
        clampUChr(b, 0, 255));

    printf("%s", code);
    fflush(stdout);
}

COLORS_DEV_API void SetColorsEx(RgbColor bg, RgbColor fg) 
{ 
    SetBgColorEx(bg); SetFgColorEx(fg); 
}

COLORS_DEV_API void SetFgColorEx(RgbColor fg) 
{ 
    SetFgColor(fg.red, fg.green, fg.blue); 
}

COLORS_DEV_API void SetBgColorEx(RgbColor bg) 
{ 
    SetBgColor(bg.red, bg.green, bg.blue); 
}

COLORS_DEV_API void SetFgColor(unsigned char red, unsigned char green, unsigned char blue) 
{ 
    PrintAnsiColor(1, red, green, blue); 
}

COLORS_DEV_API void SetBgColor(unsigned char red, unsigned char green, unsigned char blue) 
{ 
    PrintAnsiColor(0, red, green, blue); 
}

COLORS_DEV_API char* GetFgColor(RgbColor fg)
{
    char code[50];
    sprintf_s(code, sizeof(code), "\x1b[%d;2;%u;%u;%um", 38, fg.red, fg.green, fg.blue);
    return createBuffer(code);
}

COLORS_DEV_API char* GetBgColor(RgbColor bg)
{
    char code[50];
    sprintf_s(code, sizeof(code), "\x1b[%d;2;%u;%u;%um", 48, bg.red, bg.green, bg.blue);
    return createBuffer(code);
}

COLORS_DEV_API void ResetColor(void) 
{ 
    printf("\x1b[0m"); 
    fflush(stdout); 
}

COLORS_DEV_API void ClearBuffer(void) 
{ 
    printf("\x1b%c\x1b[3J", 'c'); 
    fflush(stdout); 
}

