# This is an automake file for the src directory of the
# STAREmaster utility.

# Ed Hartnett 3/16/20

AM_CPPFLAGS = -I$(top_srcdir)/include

# Create a library for STARE sidecar functionality.
lib_LTLIBRARIES = libstaremaster.la
libstaremaster_la_SOURCES = SidecarFile.cpp GeoFile.cpp

# HDF4 and HDFEOS2 is required for these.
if USE_HDF4
libstaremaster_la_SOURCES += Modis05L2GeoFile.cpp		\
Modis09L2GeoFile.cpp Modis09GAGeoFile.cpp ModisGeoFile.cpp	\
STAREmaster.c

# This is the command line utility to create STARE sidecar files for
# data files. 
bin_PROGRAMS = mk_stare
 # print_stare
LDADD = libstaremaster.la
mk_stare_SOURCES = createSidecarFile.cpp
endif # USE_HDF4

EXTRA_DIST = CMakeLists.txt


