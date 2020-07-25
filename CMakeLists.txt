# This is the main cmake file for the STAREmaster utility.
#
# Ed Hartnett 3/16/20

# This will use any cmake between 3.1 and 3.15, preferint later
# versions with updated policies.
cmake_minimum_required(VERSION 3.1...3.15)
if (${CMAKE_VERSION} VERSION_LESS 3.12)
  cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif()

# set the project name
project(STAREmaster VERSION 1.0)

# This provides cmake_print_variables() function for debugging.
include(CMakePrintHelpers)

#Add custom CMake Module
SET(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/"
  CACHE INTERNAL "Location of our custom CMake modules.")

# Find netCDF.
include(FindNetCDF)
find_package(NetCDF REQUIRED)
cmake_print_variables(NETCDF_LIBRARIES NETCDF_INCLUDES)
include_directories("${NETCDF_INCLUDES}")

# Find HDF5. CMake already includes FindHDF5.
find_package(HDF5 1.8.15)
cmake_print_variables(HDF5_FOUND HDF5_INCLUDE_DIRS HDF5_LIBRARIES)

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
cmake_print_variables(STARE_LIBRARY STARE_INCLUDE_DIR)

# STARE is required.
if (NOT STARE_LIBRARY)
  message(fatal_error " STARE library is required.")
endif()

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
include_directories("${CMAKE_CURRENT_BINARY_DIR}"
  "${CMAKE_CURRENT_SOURCE_DIR}/include"
  "${HDF4_INCLUDE_DIR}"
  "${HDFEOS2_INCLUDE_DIR}"
  "${STARE_INCLUDE_DIR}")

# Turn on testing.
enable_testing()
include(CTest)

# Build in this subdirectory.
add_subdirectory(src)
add_subdirectory(test)
