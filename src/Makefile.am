# This is an automake file for the src directory of the
# STAREmaster utility.

# Ed Hartnett 3/16/20

AM_CPPFLAGS = -I$(top_srcdir)/include

# Create a library for STARE sidecar functionality.
noinst_LTLIBRARIES = libssc.la
libssc_la_SOURCES = SidecarFile.cpp GeoFile.cpp ModisL2GeoFile.cpp	\
NetcdfSidecarFile.cpp Modis09L2GeoFile.cpp Modis09GAGeoFile.cpp

# This is the command line utility to create STARE sidecar files for
# data files.
bin_PROGRAMS = createSidecarFile
LDADD = libssc.la
createSidecarFile_SOURCES = createSidecarFile.cpp 

EXTRA_DIST = CMakeLists.txt


