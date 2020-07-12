# This is an automake file for the STAREmaster utility.

# Ed Hartnett 3/16/20

# This directory stores libtool macros, put there by aclocal.
ACLOCAL_AMFLAGS = -I m4

# Does the user want to build documentation?
if BUILD_DOCS
DOCS = docs
endif

SUBDIRS = cmake include src test ${DOCS}

EXTRA_DIST = README.md config.h.cmake.in CMakeLists.txt
