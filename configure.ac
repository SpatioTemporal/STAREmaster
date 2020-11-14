# This is the main configure file for the STAREmaster utility.

# Ed Hartnett 3/16/20

AC_PREREQ([2.59])

# Initialize with name, version, and support email address.
AC_INIT([STAREmaster], [1.0.0], [edwardjameshartnett@gmail.com])

AM_INIT_AUTOMAKE([foreign dist-zip subdir-objects])

# Keep libtool macros in an m4 directory.
AC_CONFIG_MACRO_DIR([m4])

# Set up libtool.
LT_PREREQ([2.4])
LT_INIT()

# Find the C compiler.
AC_PROG_CC
AM_PROG_CC_C_O
AC_C_CONST
AC_PROG_CPP

# Find the C++ compiler.
AC_PROG_CXX

# Find the install program.
AC_PROG_INSTALL

# Check to see if any macros must be set to enable large (>2GB) files.
AC_SYS_LARGEFILE

# Does the user want to run tests based on large data files? (Files
# must be obtained separately.)
AC_MSG_CHECKING([whether extra large files tests should be run (requires STAREmaster large test files)])
AC_ARG_ENABLE([large-file-tests],
              [AS_HELP_STRING([--enable-large-file-tests],
                              [enable running of tests based on large data files.])])
test "x$enable_large_file_tests" = xyes || enable_large_file_tests=no
AC_MSG_RESULT([$enable_large_file_tests])
AM_CONDITIONAL(LARGE_FILE_TESTS, [test "x$enable_large_file_tests" = xyes])

# If the env. variable TEST_LARGE is set, or if
# --with-temp-large=<directory>, use it as a place for the large
# (i.e. > 2 GiB) files created during the large file testing.
AC_MSG_CHECKING([where to find large test files])
AC_ARG_WITH([large-test-files],
            [AS_HELP_STRING([--with-large-test-files=<directory>],
                            [specify directory where large test files \
                            can be found, and run extra tests on those files \
			    (defaults to data).])],
            [TEST_LARGE=$with_large_test_files])
TEST_LARGE=${TEST_LARGE-data}
AC_MSG_RESULT($TEST_LARGE)
AC_SUBST(TEST_LARGE)
AC_DEFINE_UNQUOTED([TEST_LARGE], ["$TEST_LARGE"], [Place to find large test files.])

# Does the user want to build documentation?
AC_MSG_CHECKING([whether documentation should be build (requires doxygen)])
AC_ARG_ENABLE([docs],
              [AS_HELP_STRING([--enable-docs],
                              [enable building of documentation with doxygen.])])
test "x$enable_docs" = xyes || enable_docs=no
AC_MSG_RESULT([$enable_docs])
AM_CONDITIONAL(BUILD_DOCS, [test "x$enable_docs" = xyes])

# Is doxygen installed?
AC_CHECK_PROGS([DOXYGEN], [doxygen])
if test -z "$DOXYGEN" -a "x$enable_docs" = xyes; then
   AC_MSG_ERROR([Doxygen not found but --enable-docs used.])
fi

# If building docs, process Doxyfile.in into Doxyfile.
if test "x$enable_docs" = xyes; then
   AC_CONFIG_FILES([docs/Doxyfile])
fi

# Switch to C++.
AC_LANG_PUSH(C++)

# Find the STARE header.
AC_CHECK_HEADERS("STARE.h")

# Is STARE library present?
LIBS="$LIBS -lSTARE"
AC_MSG_CHECKING([whether STARE library is present])
AC_LINK_IFELSE([
  AC_LANG_SOURCE(
    [[#include "STARE.h"
    int main() { return 0; }]]
  )
], [], [AC_MSG_ERROR([STARE library not present.])])
AC_MSG_RESULT([yes])

AC_MSG_CHECKING([LIBS])
AC_MSG_RESULT([$LIBS])

# Switch back to C.
AC_LANG_POP()

# We need the math library
#AC_CHECK_LIB([m], [floor], [],
#[AC_MSG_ERROR([Can't find or link to the math library.])])

# Check for netCDF C library and header. It is required.
AC_SEARCH_LIBS([nc_create], [netcdf], [USE_NETCDF=yes],
                            [AC_MSG_ERROR([Cannot link to the netcdf C library, set LDFLAGS.])])
AC_CHECK_HEADERS([netcdf.h], [], [AC_MSG_ERROR([Cannot find netcdf.h, set CPPFLAGS.])])			    

# Check for HDF5 library.
USE_HDF5=no
AC_SEARCH_LIBS([H5Fopen], [hdf5], [USE_HDF5=no],
                            [AC_MSG_WARN([Can't find or link to the HDF5 C library; will build without HDF5 capability.])])
AC_MSG_CHECKING([whether to use HDF5])			    
AC_MSG_RESULT([$USE_HDF5])
if test "$USE_HDF5" = yes; then
   AC_DEFINE([USE_HDF5], [1], [If true, use HDF5.])
fi

# Find HDF4 and all the trimmings.
AC_CHECK_HEADERS([mfhdf.h], [], [AC_MSG_ERROR([Cannot find mfhdf.h, set CPPFLAGS.])])
AC_CHECK_HEADERS([hdf.h], [], [AC_MSG_ERROR([Cannot find hdf.h, set CPPFLAGS.])])
AC_CHECK_LIB([z], [deflate], [], [])
AC_CHECK_LIB([jpeg], [jpeg_set_quality], [], [])
AC_CHECK_LIB([df], [Hclose], [], [])
AC_CHECK_LIB([mfhdf], [NC_arrayfill], [AC_MSG_ERROR([HDF4 library must be built with --disable-netcdf.])])
AC_SEARCH_LIBS([SDcreate], [mfhdf], [USE_HDF4=yes], [USE_HDF4=no])

AC_MSG_CHECKING([whether to use HDF4])			    
AC_MSG_RESULT([$USE_HDF4])
if test "$USE_HDF4" = yes; then
   AC_DEFINE([USE_HDF4], [1], [If true, use HDF4.])
else
   AC_MSG_ERROR([HDF4 is required]) # temporarily true
fi

AC_SEARCH_LIBS([cos], [m], [], [AC_MSG_ERROR([math library required])])
AC_SEARCH_LIBS([SWopen], [hdfeos], [], [AC_MSG_ERROR([hdfeos library required])])
AC_CHECK_HEADERS([HdfEosDef.h], [], [AC_MSG_ERROR([Cannot find HdfEosDef.h, set CPPFLAGS.])], [#include "mfhdf.h"])

# We need ncdump for testing.
AC_CHECK_PROG(NCDUMP,[ncdump],[ncdump],[no])
test "$NCDUMP" == "no" && AC_MSG_ERROR([Required program 'ncdump' not found.])

# We need sed for testing.
AC_CHECK_PROG(SED,[sed],[sed],[no])
test "$SED" == "no" && AC_MSG_ERROR([Required program 'sed' not found.])

# Is open_mp available?
AX_OPENMP([have_openmp=yes], [have_openmp=no])
if test "x$have_openmp" = xyes; then
   AC_MSG_WARN([OpenMP is available.])
fi

# Move data files into build directory so it can be found by tests.
AC_CONFIG_LINKS([test/data/MOD05_L2.A2005349.2125.061.2017294065400.hdf:test/data/MOD05_L2.A2005349.2125.061.2017294065400.hdf])
AC_CONFIG_LINKS([test/ref_MOD05_L2.A2005349.2125.061.2017294065400_stare.cdl:test/ref_MOD05_L2.A2005349.2125.061.2017294065400_stare.cdl])
AC_CONFIG_LINKS([test/ref_MOD05_L2.A2005349.2125.061.2017294065400_inst_stare.cdl:test/ref_MOD05_L2.A2005349.2125.061.2017294065400_inst_stare.cdl])
AC_CONFIG_LINKS([test/ref_MYD09.A2020058.1515.006.2020060020205_stare.cdl:test/ref_MYD09.A2020058.1515.006.2020060020205_stare.cdl])
AC_CONFIG_LINKS([test/data/MOD09GA.A2020009.h00v08.006.2020011025435.hdf:test/data/MOD09GA.A2020009.h00v08.006.2020011025435.hdf])

# Create test script.
AC_CONFIG_FILES([test/run_large_file_tests.sh], [chmod ugo+x test/run_large_file_tests.sh])

# Create the config.h file.
AC_CONFIG_HEADERS([config.h])

# These files will be created when the configure script is run.
AC_CONFIG_FILES([Makefile
        include/Makefile
        src/Makefile
	test/Makefile
	test/data/Makefile
	docs/Makefile
	cmake/Makefile
        ])
AC_OUTPUT()
