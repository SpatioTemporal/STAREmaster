# This is an automake file for the test directory of the
# STAREmaster utility.

# Ed Hartnett 3/16/20

AM_CPPFLAGS = -I$(top_srcdir)/include
LDADD = ${top_builddir}/src/libssc.la

SUBDIRS = data

# This is the test program.
check_PROGRAMS = t1 t2 t_MOD043K
t1_SOURCES = t1.cpp
t2_SOURCES = t2.cpp
t_MOD043K_SOURCES = t_MOD043K.cpp
TESTS = t1 t2 t_MOD043K

# The script runs the command line utility and checks results.
TESTS += run_tests.sh

# If large test files are available this will run those tests.
if LARGE_FILE_TESTS
TESTS += run_large_file_tests.sh
endif

EXTRA_DIST = CMakeLists.txt run_tests.sh run_large_file_tests.sh

CLEANFILES = *.nc

