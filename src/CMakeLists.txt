# This is the cmake file for the src directory of the STAREmaster project.

# Ed Hartnett 7/18/20

# These are the code files.
set(CSF_FILES SidecarFile.cpp GeoFile.cpp ModisL2GeoFile.cpp NetcdfSidecarFile.cpp Modis09L2GeoFile.cpp createSidecarFile.cpp)

# This is the executable we create.
add_executable(createSidecarFile ${CSF_FILES})

# Install the execuatable in the bin directory.
install(TARGETS createSidecarFile RUNTIME DESTINATION bin)

