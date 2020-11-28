# This is the main cmake file for the STAREmaster utility.
#
# Ed Hartnett 3/16/20

# This will use any cmake between 3.1 and 3.15, preferint later
# versions with updated policies.
cmake_minimum_required(VERSION 3.1...3.15)
if (${CMAKE_VERSION} VERSION_LESS 3.12)
  cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif()

# This provides cmake_print_variables() function for debugging.
include(CMakePrintHelpers)

# set the project name
project(STAREmaster VERSION 1.0.0)
set(PACKAGE_VERSION ${PROJECT_VERSION})
cmake_print_variables(PACKAGE_VERSION PROJECT_VERSION)

#Add custom CMake Module
SET(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/"
  CACHE INTERNAL "Location of our custom CMake modules.")

# Handle user build options.
OPTION(ENABLE_LARGE_FILE_TESTS "Enable large file tests." OFF)
if (NOT TEST_LARGE)
  set(TEST_LARGE "data")
endif ()
cmake_print_variables(ENABLE_LARGE_FILE_TESTS TEST_LARGE)

# Find netCDF.
include(FindNetCDF)
#find_package (NetCDF REQUIRED COMPONENTS C)
find_package(NetCDF REQUIRED)
cmake_print_variables(NETCDF_LIBRARIES NETCDF_INCLUDES)
include_directories("${NETCDF_INCLUDES}")

###
# Allow the user to specify libraries
# to link against, similar to automakes 'LIBS' variable.
###
SET(SM_EXTRA_DEPS "" CACHE STRING "Additional libraries to link against.")
IF(SM_EXTRA_DEPS)
  STRING(REPLACE " " ";" DEPS_LIST ${SM_EXTRA_DEPS})
  FOREACH(_DEP ${DEPS_LIST})
    STRING(REGEX REPLACE "^-l" "" _LIB ${_DEP})
    FIND_LIBRARY("${_LIB}_DEP" NAMES "${_LIB}" "lib${_LIB}")
    # MESSAGE(${${_LIB}_DEP})
    IF(${_LIB}_DEP STREQUAL "")
      MESSAGE(FATAL_ERROR "Error finding ${_LIB}.")
    ELSE()
      MESSAGE(STATUS "Found ${_LIB}: ${${_LIB}_DEP}")
    ENDIF()
    SET(EXTRA_DEPS ${EXTRA_DEPS} "${${_LIB}_DEP}")
  ENDFOREACH()
  MESSAGE("Extra deps: ${EXTRA_DEPS}")
  LIST(REMOVE_DUPLICATES EXTRA_DEPS)
ENDIF()
###
# End user-specified dependent libraries.
###

# Get extra netCDF libraries.
execute_process(COMMAND nc-config --libs OUTPUT_VARIABLE CMD_OUTPUT)

# Unfortunately, this requires a more recent netcdf install.
#execute_process(COMMAND nc-config --libs --static OUTPUT_VARIABLE CMD_OUTPUT)
IF(CMD_OUTPUT)
	string(STRIP ${CMD_OUTPUT} CMD_OUTPUT)
ENDIF()
cmake_print_variables(CMD_OUTPUT)

# Find HDF5. CMake already includes FindHDF5.
find_package(HDF5 1.8.15)
cmake_print_variables(HDF5_FOUND HDF5_INCLUDE_DIRS HDF5_LIBRARIES)
find_library(Z NAMES "z")
cmake_print_variables(HDF5_INCLUDE_DIR HDF5_DEFINITIONS HDF5_LIBRARIES Z_LIB)

# Find HDF4.
#include(FindHDF4)
#find_package(HDF4 4.2.5)
#cmake_print_variables(HDF4_FOUND HDF4_INCLUDE_DIRS HDF4_LIBRARIES)
find_path(HDF4_INCLUDE_DIR NAMES hdf.h PATH_SUFFIXES hdf)
find_library(MFHDF4 NAMES "mfhdf")
find_library(DF NAMES "df")
find_library(JPEG_LIB NAMES jpeg libjpeg)
cmake_print_variables(HDF4_INCLUDE_DIR MFHDF4 JPEG_LIB)

# Find HDFEOS2
find_path(HDFEOS2_INCLUDE_DIR NAMES HdfEosDef.h) 
find_library(HDFEOS2
  NAMES "hdfeos")
cmake_print_variables(HDFEOS2_INCLUDE_DIR HDFEOS2)

# Find STARE.
include(FindSTARE)
cmake_print_variables(STARE_LIBRARY_DIR STARE_INCLUDE_DIR)

# STARE is required.
if (NOT STARE_FOUND)
  message(fatal_error " STARE library is required.")
endif()

# STARE requires C++11
add_compile_options( -std=c++11 )

# Ensure that ncdump is available for testing.
find_program(NCDUMP NAMES ncdump)
cmake_print_variables(NCDUMP)
if (NOT NCDUMP)
  MESSAGE(FATAL_ERROR "Error finding ncdump")  
endif ()

# FindOpenMP()
find_package( OpenMP )
if(OPENMP_FOUND)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
set(HAVE_OPENMP 1)
message("CMAKE_CXX_FLAGS        ${CMAKE_CXX_FLAGS}")
message("CMAKE_EXE_LINKER_FLAGS ${CMAKE_EXE_LINKER_FLAGS}")
else()
message("OpenMP not found")
endif()

# Set the include directories.
include_directories("${CMAKE_CURRENT_BINARY_DIR}"
  "${CMAKE_CURRENT_SOURCE_DIR}/include"
  "${HDF4_INCLUDE_DIR}"
  "${HDFEOS2_INCLUDE_DIR}"
  "${STARE_INCLUDE_DIR}")

# Check STARE version, must be 0.16.3 or later.
set(CMAKE_REQUIRED_INCLUDES "${STARE_INCLUDE_DIR}")
set(CMAKE_REQUIRED_FLAGS "-std=c++11")
cmake_print_variables(STARE_LIBRARIES)
set(CMAKE_REQUIRED_LINK_OPTIONS "-L${STARE_LIBRARIES}")
set(CMAKE_REQUIRED_LIBRARIES "STARE")
INCLUDE(CheckCXXSourceCompiles)
CHECK_CXX_SOURCE_COMPILES("
#include <STARE.h>
#if STARE_VERSION_MAJOR == 0
#if STARE_VERSION_MINOR < 16
#error
#endif
#if STARE_VERSION_MINOR == 16
#if STARE_VERSION_PATCH < 3
#error
#endif
#endif
#endif
int main() {return 0;}" GOOD_STARE_VERSION)
cmake_print_variables(GOOD_STARE_VERSION)
file(READ /home/runner/work/STAREmaster/STAREmaster/build/CMakeFiles/CMakeOutput.log LOGFILE)
cmake_print_variables(LOGFILE)
if (NOT GOOD_STARE_VERSION)
  MESSAGE(FATAL_ERROR "STARE version 0.16.3 or later is required")  
endif ()

#TARGET_LINK_LIBRARIES ( myDSO PUBLIC ${HDF5_LIBRARIES} )
#TARGET_INCLUDE_DIRECTORIES ( myDSO PUBLIC ${HDF5_INCLUDE_DIRS} )

# This will be used for shell script tests.
MACRO(add_sh_test prefix F)
  IF(HAVE_BASH)
    ADD_TEST(${prefix}_${F} bash "-c" "export srcdir=${CMAKE_CURRENT_SOURCE_DIR};export TOPSRCDIR=${CMAKE_SOURCE_DIR};${CMAKE_CURRENT_BINARY_DIR}/${F}.sh")
  ENDIF()
ENDMACRO()

# Create a config.h.
configure_file(config.h.cmake.in config.h)

# Turn on testing.
enable_testing()
include(CTest)

# Build in this subdirectory.
add_subdirectory(src)
add_subdirectory(test)
