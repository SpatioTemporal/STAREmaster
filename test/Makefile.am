# This is an automake file for the test directory of the
# STAREmaster utility.

# Ed Hartnett 3/16/20

AM_CPPFLAGS = -I$(top_srcdir)/include
LDADD = ${top_builddir}/src/libssc.la

SUBDIRS = data

# This is the test program.
check_PROGRAMS = t1 t2
t1_SOURCES = t1.cpp
t2_SOURCES = t2.cpp
TESTS = t1 t2

# The script runs the command line utility and checks results.
TESTS += run_tests.sh

EXTRA_DIST = CMakeLists.txt run_tests.sh

CLEANFILES = *.nc

