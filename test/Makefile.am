# This is an automake file for the test directory of the
# STAREmaster utility.

# Ed Hartnett 3/16/20

AM_CPPFLAGS = -I$(top_srcdir)/include
LDADD = ${top_builddir}/src/libstaremaster.la

SUBDIRS = data

# These tests require HDF4 and the HDFEOS2 library.
if USE_HDF4
# This is the test program.
check_PROGRAMS = t1 t2
t1_SOURCES = t1.cpp
t2_SOURCES = t2.cpp

# The script runs the t1 and also the createSidecarFile command line
# utility and checks results.
TESTS = t2 run_tests.sh

# If large test files are available this will run those tests.
if LARGE_FILE_TESTS
TESTS += run_large_file_tests.sh 
check_PROGRAMS += tst_interp
tst_interp_SOURCES = tst_interp.cpp
endif
endif # USE_HDF4

# These files also must be included with distribution.
EXTRA_DIST = CMakeLists.txt run_tests.sh run_large_file_tests.sh	\
ref_MOD05_L2.A2005349.2125.061.2017294065400_stare.cdl			\
ref_MYD09.A2020058.1515.006.2020060020205_stare.cdl			\
ref_MOD09GA.A2020009.h00v08.006.2020011025435_stare.cdl			\
ref_t1_sidecar.cdl

CLEANFILES = *.nc *_out.cdl
