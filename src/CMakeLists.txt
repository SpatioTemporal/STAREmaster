# This is the cmake file for the src directory of the STAREmaster project.

# Ed Hartnett 7/18/20

# This is the library we create.
add_library(ssc SidecarFile.cpp GeoFile.cpp Modis05L2GeoFile.cpp Modis09L2GeoFile.cpp
  Modis09GAGeoFile.cpp ModisGeoFile.cpp STAREmaster.c)

# This is the executable we create.
add_executable(mk_stare mk_stare.cpp)

# Add these directories to those searched during linking.
target_link_directories(mk_stare PUBLIC ${STARE_LIBRARY_DIR})

# The executable depends on these libraries.
target_link_libraries(mk_stare ssc)
target_link_libraries(mk_stare ${NETCDF_LIBRARIES_C})
target_link_libraries(mk_stare STARE)
target_link_libraries(mk_stare ${HDFEOS2})
target_link_libraries(mk_stare ${MFHDF4} ${DF} ${JPEG_LIB})
target_link_libraries(mk_stare ${HDF5_LIBRARIES} ${HDF5_HL_LIBRARIES})
target_link_libraries(mk_stare ${CMD_OUTPUT})

# Install the execuatable in the bin directory.
install(TARGETS mk_stare RUNTIME DESTINATION bin)

