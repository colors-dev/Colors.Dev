// import_exports.h
#pragma once

// File Version is based on build Date/Time
// Format: (YEAR-2020).MM.DD.HHmm (UTC)

// The following Date and Time are auto updated during build 
// with update_version.ps1, based on the current UTC date and 
// time, with the year offset from 2020 to keep version 
// numbers manageable.
#define COLORS_DEV_FULL_YEAR 2026
#define COLORS_DEV_YEAR_OFFSET 6
#define COLORS_DEV_MONTH 5
#define COLORS_DEV_DAY 3
#define COLORS_DEV_UTC_TIME 0453
// This release number is incremented manually for each NuGet 
// release, to distinguish between multiple releases on the 
// same day and must be manually updated.
#define COLORS_DEV_NUGET_RELEASE 1

// Stringify helper macros
#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

// File Version uses date/time components
// Example: "1.4.1.1104" or similar based on build timestamp
#define COLORS_DEV_FILE_VERSION TOSTRING(COLORS_DEV_YEAR_OFFSET) "." \
        TOSTRING(COLORS_DEV_MONTH) "." \
        TOSTRING(COLORS_DEV_DAY) "." \
        TOSTRING(COLORS_DEV_UTC_TIME)

// NuGet/Product Version (Major.Minor.Patch.Release)
#define COLORS_DEV_VERSION \
        TOSTRING(COLORS_DEV_YEAR_OFFSET) "." \
        TOSTRING(COLORS_DEV_MONTH) "." \
        TOSTRING(COLORS_DEV_DAY) "." \
        TOSTRING(COLORS_DEV_NUGET_RELEASE)

// Copyright message, with current year based on full year macro
#define COLORS_DEV_COPYRIGHT "Copyright (C) " TOSTRING(COLORS_DEV_FULL_YEAR) " colors.dev"

// --- "DLL Export" Macro ---
// This tells the compiler that these functions should be made public (exported) from the DLL.
#define COLORS_DEV_API COLORS_DEV_DLL

#if defined(_WIN32) || defined(_WIN64)
#ifdef COLORS_DEV_EXPORTS
#define COLORS_DEV_DLL __declspec(dllexport)
#else
#define COLORS_DEV_DLL __declspec(dllimport)
#endif
#else
#define COLORS_DEV_DLL __attribute__((visibility("default")))
#endif
