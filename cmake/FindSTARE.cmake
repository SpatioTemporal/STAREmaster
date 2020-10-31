# Find STARE library.
# Ed Hartnett 2020

#[=======================================================================[.rst:
FindSTARE
-------

Finds the STARE library.

Imported Targets
^^^^^^^^^^^^^^^^

This module provides the following imported targets, if found:

``STARE::STARE``
The STARE library

Result Variables
^^^^^^^^^^^^^^^^

This will define the following variables:

``STARE_FOUND``
True if the system has the STARE library.
``STARE_VERSION``
The version of the STARE library which was found.
``STARE_INCLUDE_DIRS``
Include directories needed to use STARE.
``STARE_LIBRARIES``
Libraries needed to link to STARE.

Cache Variables
^^^^^^^^^^^^^^^

The following cache variables may also be set:

``STARE_INCLUDE_DIR``
The directory containing ``STARE.h``.
``STARE_LIBRARY_DIR``
The path to the STARE library.

#]=======================================================================]

# This provides cmake_print_variables() function for debugging.
include(CMakePrintHelpers)

find_package(PkgConfig)
pkg_check_modules(PC_STARE QUIET STARE)

find_path(STARE_INCLUDE_DIR
  NAMES STARE.h
  PATHS ${PC_STARE_INCLUDE_DIRS}
  PATH_SUFFIXES STARE
  )
find_library(STARE_LIBRARY
  NAMES STARE
  PATHS ${PC_STARE_LIBRARY_DIRS}
  )

set(STARE_VERSION ${PC_STARE_VERSION})

# Print out values for debugging.
cmake_print_variables(STARE_LIBRARY STARE_INCLUDE_DIR STARE_VERSION)

# Handle the arguments to find_package()
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(STARE
  FOUND_VAR STARE_FOUND
  REQUIRED_VARS STARE_LIBRARY STARE_INCLUDE_DIR
  VERSION_VAR STARE_VERSION
  )

if(STARE_FOUND)
  set(STARE_LIBRARIES ${STARE_LIBRARY})
  set(STARE_INCLUDE_DIRS ${STARE_INCLUDE_DIR})
  set(STARE_DEFINITIONS ${PC_STARE_CFLAGS_OTHER})
endif()

if(STARE_FOUND AND NOT TARGET STARE::STARE)
  add_library(STARE::STARE UNKNOWN IMPORTED)
  set_target_properties(STARE::STARE PROPERTIES
    IMPORTED_LOCATION "${STARE_LIBRARY}"
    INTERFACE_COMPILE_OPTIONS "${PC_STARE_CFLAGS_OTHER}"
    INTERFACE_INCLUDE_DIRECTORIES "${STARE_INCLUDE_DIR}"
    )
endif()

if(STARE_FOUND)
  if (NOT TARGET STARE::STARE)
    add_library(STARE::STARE UNKNOWN IMPORTED)
  endif()
  if (STARE_LIBRARY_RELEASE)
    set_property(TARGET STARE::STARE APPEND PROPERTY
      IMPORTED_CONFIGURATIONS RELEASE
      )
    set_target_properties(STARE::STARE PROPERTIES
      IMPORTED_LOCATION_RELEASE "${STARE_LIBRARY_RELEASE}"
      )
  endif()
  if (STARE_LIBRARY_DEBUG)
    set_property(TARGET STARE::STARE APPEND PROPERTY
      IMPORTED_CONFIGURATIONS DEBUG
      )
    set_target_properties(STARE::STARE PROPERTIES
      IMPORTED_LOCATION_DEBUG "${STARE_LIBRARY_DEBUG}"
      )
  endif()
  set_target_properties(STARE::STARE PROPERTIES
    INTERFACE_COMPILE_OPTIONS "${PC_STARE_CFLAGS_OTHER}"
    INTERFACE_INCLUDE_DIRECTORIES "${STARE_INCLUDE_DIR}"
    )
endif()

mark_as_advanced(
  STARE_INCLUDE_DIR
  STARE_LIBRARY
  )
