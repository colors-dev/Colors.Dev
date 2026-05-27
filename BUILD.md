# Colors.Dev - Cross-Platform Build Guide

## Overview

| Platform | Output | Versioning mechanism |
|----------|--------|----------------------|
| Windows | Colors.Dev.dll | PE resource (version.rc) |
| Linux | libcolors_dev.so | ELF soname + colors_dev.map symbol version script |
| macOS | libcolors_dev.dylib | Mach-O current_version / compatibility_version |

Version numbers are driven by a single source of truth: **import_exports.h**.
The pre-build script stamps that file, and both the Windows resource compiler
and the Linux/macOS Makefile read from it.

---

## Prerequisites

### Windows
- Visual Studio 2022 or later (v143+ toolset)
- Windows SDK (provides rc.exe and windows.h)
- PowerShell 5.1+ (pre-installed on Windows 10/11)

### Linux
    sudo apt install build-essential   # Debian/Ubuntu
    sudo dnf install gcc make          # Fedora/RHEL

### macOS
    xcode-select --install

---

## Step 1 - Stamp the version

### Windows (run automatically by Visual Studio pre-build event)
    .\update_version.ps1

### Linux / macOS

PowerShell Core option:
    pwsh -File update_version.ps1

Bash equivalent (save as update_version.sh, chmod +x):
    #!/usr/bin/env bash
    set -euo pipefail
    HEADER=import_exports.h
    FULL_YEAR=2026
    YEAR_OFFSET=
    MONTH=5
    DAY=26
    TIME=2032
    sed -i.bak -e 's/(#define COLORS_DEV_FULL_YEAR\s+)[0-9]+/\1''/' import_exports.h
    sed -i.bak -e 's/(#define COLORS_DEV_YEAR_OFFSET\s+)[0-9]+/\1''/' import_exports.h
    sed -i.bak -e 's/(#define COLORS_DEV_MONTH\s+)[0-9]+/\1''/' import_exports.h
    sed -i.bak -e 's/(#define COLORS_DEV_DAY\s+)[0-9]+/\1''/' import_exports.h
    sed -i.bak -e 's/(#define COLORS_DEV_UTC_TIME\s+)[0-9]+/\1''/' import_exports.h
    echo Version updated: ...

> macOS note: BSD sed -i requires a backup extension. Delete with: rm import_exports.h.bak
> Or use GNU sed: brew install gnu-sed then replace sed with gsed.

---

## Step 2 - Fix portability issue: objbase.h in common.h

common.h includes <objbase.h> which is Windows SDK only.
Before building on Linux or macOS, change that line:

  Before: #include <objbase.h>    // For malloc
  After:  #include <stdlib.h>     // For malloc

---

## Step 3 - Build

### Windows
Open Colors.Dev.sln in Visual Studio and build normally.
The pre-build event runs update_version.ps1 automatically.

| Configuration | Platform | Output directory |
|---------------|----------|------------------|
| Debug         | Win32    | Win32\Debug\     |
| Release       | Win32    | Win32\Release\   |
| Debug         | x64      | x64\Debug\       |
| Release       | x64      | x64\Release\     |

### Linux
    make                        # release build
    make DEBUG=1                # debug build
    make ARCH=aarch64           # ARM64 cross-compile
    make install PREFIX=~/.local

Output (example version 6.5.3.1):
    build/release/libcolors_dev.so.6.5.3.1   <- real binary
    build/release/libcolors_dev.so.6          -> symlink (soname)
    build/release/libcolors_dev.so            -> symlink (link-time)

Verify:
    readelf -d build/release/libcolors_dev.so.6.5.3.1 | grep -E 'SONAME|VERDEF'
    objdump -T build/release/libcolors_dev.so.6.5.3.1 | grep COLORS_DEV

### macOS
    make                # release build
    make DEBUG=1        # debug build
    make ARCH=arm64     # Apple Silicon

Output (example version 6.5.3.1):
    build/release/libcolors_dev.6.5.3.1.dylib   <- real binary
    build/release/libcolors_dev.6.dylib          -> symlink (compat)
    build/release/libcolors_dev.dylib            -> symlink (link-time)

Verify:
    otool -L build/release/libcolors_dev.dylib

---

## Step 4 - Install
    sudo make install                  # to /usr/local
    make install PREFIX=~/.local       # user-local, no sudo needed

Installed layout:
    PREFIX/lib/libcolors_dev.so.6.5.3.1
    PREFIX/lib/libcolors_dev.so.6  -> symlink
    PREFIX/lib/libcolors_dev.so    -> symlink
    PREFIX/include/colors_dev/*.h

---

## Versioning summary

| Component | Where set | Who updates it |
|-----------|-----------|----------------|
| COLORS_DEV_YEAR_OFFSET | import_exports.h | update_version.ps1 / .sh |
| COLORS_DEV_MONTH | import_exports.h | same script |
| COLORS_DEV_DAY | import_exports.h | same script |
| COLORS_DEV_UTC_TIME | import_exports.h | same script |
| COLORS_DEV_NUGET_RELEASE | import_exports.h | MANUAL - increment per NuGet publish |
| Windows FileVersion | version.rc | auto via RC compiler |
| Linux soname | Makefile | reads import_exports.h |
| Linux symbol tag | colors_dev.map | update_version.ps1 |
| macOS current_version | Makefile | reads import_exports.h |

---

## CI / cross-compilation notes

- ARM64 cross-compile on x86_64 Linux: install gcc-aarch64-linux-gnu, then
  make CC=aarch64-linux-gnu-gcc ARCH=aarch64
- colors_dev.map is Linux-only; the Makefile does not pass it on macOS.
- version.rc is Windows-only; it is not referenced by the Makefile.
