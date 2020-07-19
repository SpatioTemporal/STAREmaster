# This is the cmake file for the test directory of the STAREmaster project.

# Ed Hartnett 7/19/20

add_executable(t1 t1.cpp)
target_link_libraries(t1 ssc)
target_link_directories(t1 PUBLIC ${NETCDF_LIBRARIES})
target_link_libraries(t1 netcdf)
target_link_directories(t1 PUBLIC ${STARE_LIBRARY})
target_link_libraries(t1 STARE)
target_link_libraries(t1 ${HDFEOS2})
target_link_libraries(t1 ${DF} ${MFHDF4})

add_test(NAME t1 COMMAND t1)

add_executable(t2 t2.cpp)
target_link_libraries(t2 ssc)
target_link_directories(t2 PUBLIC ${NETCDF_LIBRARIES})
target_link_libraries(t2 netcdf)
target_link_directories(t2 PUBLIC ${STARE_LIBRARY})
target_link_libraries(t2 STARE)
target_link_libraries(t2 ${HDFEOS2})
target_link_libraries(t2 ${DF} ${MFHDF4})

configure_file(data/MOD05_L2.A2005349.2125.061.2017294065400.hdf data/MOD05_L2.A2005349.2125.061.2017294065400.hdf COPYONLY)

#FILE(GLOB COPY_FILES ${CMAKE_BINARY_DIR}/test/*.hdf ${CMAKE_CURRENT_SOURCE_DIR}/test/*.sh)
#FILE(COPY ${COPY_FILES} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/ FILE_PERMISSIONS OWNER_WRITE OWNER_READ OWNER_EXECUTE)


