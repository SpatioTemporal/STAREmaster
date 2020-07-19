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


