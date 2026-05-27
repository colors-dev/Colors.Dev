# Makefile — Colors.Dev cross-platform shared library
# Targets: Linux (libcolors_dev.so) and macOS (libcolors_dev.dylib)
#
# Usage:
#   make                  # build for the current platform (release)
#   make DEBUG=1          # debug build
#   make ARCH=arm64       # override target architecture (default: x86_64)
#   make install          # install to PREFIX (default /usr/local)
#   make clean            # remove build artefacts
#
# Before building, run update_version.ps1 (Windows/PowerShell) or
# update_version.sh (Linux/macOS — see BUILD.md) to stamp import_exports.h
# and regenerate colors_dev.map.

# ---------------------------------------------------------------------------
# Toolchain
# ---------------------------------------------------------------------------
CC      ?= gcc
INSTALL ?= install
PREFIX  ?= /usr/local

# ---------------------------------------------------------------------------
# Version — read directly from import_exports.h so there is one source of truth
# ---------------------------------------------------------------------------
YEAR_OFFSET := $(shell grep 'COLORS_DEV_YEAR_OFFSET' import_exports.h | awk '{print $$3}')
MONTH       := $(shell grep 'COLORS_DEV_MONTH '      import_exports.h | awk '{print $$3}')
DAY         := $(shell grep 'COLORS_DEV_DAY '        import_exports.h | awk '{print $$3}')
NUGET_REL   := $(shell grep 'COLORS_DEV_NUGET_RELEASE' import_exports.h | awk '{print $$3}')

VERSION_MAJOR := $(YEAR_OFFSET)
VERSION_FULL  := $(YEAR_OFFSET).$(MONTH).$(DAY).$(NUGET_REL)

LIB_BASE   := libcolors_dev
LIB_NAME   := $(LIB_BASE).so
LIB_SONAME := $(LIB_BASE).so.$(VERSION_MAJOR)
LIB_REAL   := $(LIB_BASE).so.$(VERSION_FULL)

# ---------------------------------------------------------------------------
# Architecture
# ---------------------------------------------------------------------------
ARCH ?= x86_64

# ---------------------------------------------------------------------------
# Source files
# ---------------------------------------------------------------------------
SRCS := ansi_printing.c \
		cmyk_space.c    \
		color_support.c \
		hsl_space.c     \
		hsv_space.c     \
		lch_space.c     \
		luv_space.c     \
		rgb_color.c     \
		white_points.c  \
		xyz_space.c

OBJS := $(SRCS:.c=.o)

# ---------------------------------------------------------------------------
# Flags
# ---------------------------------------------------------------------------
CFLAGS_COMMON := -fPIC -Wall -Wextra -std=c11 \
				 -DCOLORS_DEV_EXPORTS \
				 -fvisibility=hidden \
				 -march=$(ARCH)

ifdef DEBUG
  CFLAGS := $(CFLAGS_COMMON) -g -O0 -DDEBUG
  OUTDIR := build/debug
else
  CFLAGS := $(CFLAGS_COMMON) -O2 -DNDEBUG
  OUTDIR := build/release
endif

OBJDIR := $(OUTDIR)/obj

# ---------------------------------------------------------------------------
# Platform detection
# ---------------------------------------------------------------------------
UNAME := $(shell uname -s)

ifeq ($(UNAME), Darwin)
  # macOS — Mach-O dylib
  LIB_MAC       := $(LIB_BASE).dylib
  LIB_MAC_VER   := $(LIB_BASE).$(VERSION_FULL).dylib
  LIB_MAC_COMPAT:= $(LIB_BASE).$(VERSION_MAJOR).dylib

  LDFLAGS := -dynamiclib \
			 -install_name @rpath/$(LIB_MAC_COMPAT) \
			 -current_version $(VERSION_FULL) \
			 -compatibility_version $(VERSION_MAJOR).0.0

  TARGET      := $(OUTDIR)/$(LIB_MAC_VER)
  LINK_COMPAT := $(OUTDIR)/$(LIB_MAC_COMPAT)
  LINK_BASE   := $(OUTDIR)/$(LIB_MAC)
  PLATFORM    := macOS
else
  # Linux — ELF shared object with versioned symbol map
  LDFLAGS := -shared \
			 -Wl,-soname,$(LIB_SONAME) \
			 -Wl,--version-script,colors_dev.map

  TARGET      := $(OUTDIR)/$(LIB_REAL)
  LINK_SONAME := $(OUTDIR)/$(LIB_SONAME)
  LINK_BASE   := $(OUTDIR)/$(LIB_NAME)
  PLATFORM    := Linux
endif

# ---------------------------------------------------------------------------
# Build rules
# ---------------------------------------------------------------------------
.PHONY: all clean install dirs info

all: dirs info $(TARGET) symlinks
	@echo ""
	@echo "  Build complete: $(TARGET)"

info:
	@echo "  Platform : $(PLATFORM)"
	@echo "  Arch     : $(ARCH)"
	@echo "  Version  : $(VERSION_FULL)  (major $(VERSION_MAJOR))"
	@echo "  Output   : $(OUTDIR)/"
	@echo ""

dirs:
	@mkdir -p $(OBJDIR)

$(OBJDIR)/%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(addprefix $(OBJDIR)/,$(OBJS))
ifeq ($(UNAME), Darwin)
	$(CC) $(LDFLAGS) -o $@ $^
else
	$(CC) $(LDFLAGS) -o $@ $^
endif

# Symlinks
.PHONY: symlinks
ifeq ($(UNAME), Darwin)
symlinks: $(TARGET)
	ln -sf $(notdir $(TARGET))      $(LINK_COMPAT)
	ln -sf $(notdir $(LIB_MAC_VER)) $(LINK_BASE)
else
symlinks: $(TARGET)
	ln -sf $(notdir $(TARGET))      $(LINK_SONAME)
	ln -sf $(notdir $(LIB_SONAME))  $(LINK_BASE)
endif

# ---------------------------------------------------------------------------
# Install
# ---------------------------------------------------------------------------
install: all
	$(INSTALL) -d $(DESTDIR)$(PREFIX)/lib
	$(INSTALL) -d $(DESTDIR)$(PREFIX)/include/colors_dev
	$(INSTALL) -m 755 $(TARGET) $(DESTDIR)$(PREFIX)/lib/
ifeq ($(UNAME), Darwin)
	ln -sf $(notdir $(TARGET))      $(DESTDIR)$(PREFIX)/lib/$(LIB_MAC_COMPAT)
	ln -sf $(notdir $(LIB_MAC_VER)) $(DESTDIR)$(PREFIX)/lib/$(LIB_MAC)
else
	ln -sf $(notdir $(TARGET))      $(DESTDIR)$(PREFIX)/lib/$(LIB_SONAME)
	ln -sf $(notdir $(LIB_SONAME))  $(DESTDIR)$(PREFIX)/lib/$(LIB_NAME)
	ldconfig $(DESTDIR)$(PREFIX)/lib
endif
	$(INSTALL) -m 644 *.h $(DESTDIR)$(PREFIX)/include/colors_dev/
	@echo "Installed to $(DESTDIR)$(PREFIX)"

# ---------------------------------------------------------------------------
# Clean
# ---------------------------------------------------------------------------
clean:
	rm -rf build/
	@echo "Clean complete."
