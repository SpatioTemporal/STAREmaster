# This is an automake file for the src directory of the
# STAREmaster utility.

# Ed Hartnett 3/16/20

AM_CPPFLAGS = -I$(top_srcdir)/include

# Create a library for STARE sidecar functionality.
lib_LTLIBRARIES = libstaremaster.la
libstaremaster_la_SOURCES = SidecarFile.cpp GeoFile.cpp

bin_PROGRAMS =

# HDF4 and HDFEOS2 is required for these.
if USE_HDF4
libstaremaster_la_SOURCES += Modis05L2GeoFile.cpp		\
Modis09L2GeoFile.cpp Modis09GAGeoFile.cpp ModisGeoFile.cpp	\
STAREmaster.c

# This is the command line utility to create STARE sidecar files for
# data files, and another to check sidecar files. 
bin_PROGRAMS += mk_stare  check_sidecar
check_sidecar_SOURCES = check_sidecar.cpp

LDADD = libstaremaster.la
mk_stare_SOURCES = mk_stare.cpp
endif # USE_HDF4

bin_PROGRAMS += print_stare
print_stare_SOURCES = print_stare.cpp

EXTRA_DIST = CMakeLists.txt


