# This is the cmake file for the src directory of the STAREmaster project.

# Ed Hartnett 7/18/20

set(CSF_FILES SidecarFile.cpp GeoFile.cpp ModisL2GeoFile.cpp NetcdfSidecarFile.cpp Modis09L2GeoFile.cpp createSidecarFile.cpp)
add_executable(createSidecarFile ${CSF_FILES})

