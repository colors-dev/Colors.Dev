// common.h
#pragma once

#ifndef COMMON_H
#define COMMON_H

#include <string.h>             // For strlen, strcpy_s
#include <stdio.h>              // Required for sprintf
#include <objbase.h>            // For malloc

//#define TRUE 1
//#define FALSE 0
//#define BOOL int

static const double COLORS_PI_F = 3.14159265358979323846f;					// float
static const double COLORS_PI = 3.14159265358979323846;						// double
static const double COLORS_PI_L = 3.14159265358979323846264338327950288L;	// long double
static const double COLORS_LCH_CHROMA_EPS = 0.003;							// 1e-6 - A small epsilon value to treat very low chroma as zero in LCH conversions. 
																			// Perceptual stability: treat near-neutral colors as neutral.
																			// 0.003 clamps your observed D65Full gray drift (C=0.002).

static inline char* createBuffer(const char* str) {
	if (!str) return NULL;

	size_t len = strlen(str) + 1;
	char* buffer = (char*)malloc(len);

	if (buffer) {
		memcpy(buffer, str, len);
	}

	return buffer;
}
static inline unsigned char clampUChr(unsigned char v, unsigned char min, unsigned char max) { return v < min ? min : (v > max ? max : v); }
static inline int clampInt(int v, int min, int max) { return v < min ? min : (v > max ? max : v); }
static inline double clampDbl(double v, double min, double max) { return v < min ? min : (v > max ? max : v); }
static inline double normalize01_to_0max(double v, double max) { return (v > 0.0 && v < 1.0) ? (v * max) : v; }
static inline unsigned char clampIntToUChr(int v)
{
	v = clampInt(v, 0, 255);
	return (unsigned char)v;
}
static inline double makeWholeClamp(double v, double min, double max)
{
	if (v > 0.0 && v < 1.0)
		v *= max;

	return clampDbl(v, min, max);
}
static inline double chkDbleClamp(double v, double min, double max, double div)
{
	v = makeWholeClamp(v, min, max);

	if (div != 0.0)
		v /= div;

	return clampDbl(v, min, max);
}

#endif