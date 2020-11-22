# This is the cmake file for the test directory of the STAREmaster project.

# Ed Hartnett 7/19/20

add_executable(t1 t1.cpp)

target_link_directories(t1 PUBLIC ${STARE_LIBRARY_DIR})

target_link_libraries(t1 ssc)
target_link_libraries(t1 PUBLIC ${NETCDF_LIBRARIES_C})
target_link_libraries(t1 STARE)
target_link_libraries(t1 ${HDFEOS2})
target_link_libraries(t1 ${MFHDF4} ${DF} ${JPEG_LIB})
target_link_libraries(t1 ${CMD_OUTPUT})

add_test(NAME t1 COMMAND t1)

add_executable(t2 t2.cpp)

target_link_directories(t2 PUBLIC ${STARE_LIBRARY_DIR})

target_link_libraries(t2 ssc)
target_link_libraries(t2 PUBLIC ${NETCDF_LIBRARIES_C})
target_link_libraries(t2 STARE)
target_link_libraries(t2 ${HDFEOS2})
target_link_libraries(t2 ${MFHDF4} ${DF} ${JPEG_LIB})
target_link_libraries(t2 ${CMD_OUTPUT})

add_test(NAME t2 COMMAND t2)

# Make sure the necessary data file is present in the build directory.
configure_file(data/MOD05_L2.A2005349.2125.061.2017294065400.hdf data/MOD05_L2.A2005349.2125.061.2017294065400.hdf COPYONLY)

# Configure the file that runs large file tests.
configure_file(run_large_file_tests.sh.in run_large_file_tests.sh @ONLY)

# Find bash.
find_program(HAVE_BASH bash)

# Macro to add a shell test.
MACRO(add_sh_test F)
  IF(HAVE_BASH)
    ADD_TEST(${F} bash "-c" "export srcdir=${CMAKE_CURRENT_SOURCE_DIR};export TOPSRCDIR=${CMAKE_SOURCE_DIR};${CMAKE_CURRENT_BINARY_DIR}/${F}.sh")
  ENDIF()
ENDMACRO()

# Add the shell test that always runs.
add_sh_test(run_tests)

# This shell test only runs if the ENABLE_LARGE_FILE_TESTS option was
# set.
if (ENABLE_LARGE_FILE_TESTS)
  add_sh_test(run_large_file_tests)
endif()

#FILE(GLOB COPY_FILES ${CMAKE_BINARY_DIR}/test/*.hdf ${CMAKE_CURRENT_SOURCE_DIR}/test/*.sh)
FILE(GLOB COPY_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.sh ${CMAKE_CURRENT_SOURCE_DIR}/ref_*.cdl)
FILE(COPY ${COPY_FILES} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/ FILE_PERMISSIONS OWNER_WRITE OWNER_READ OWNER_EXECUTE)


