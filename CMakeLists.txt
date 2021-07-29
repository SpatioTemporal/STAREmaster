# This is the cmake file for the src directory of the STAREmaster project.

# Ed Hartnett 7/18/20

# Hacked to work with the 'hyrax' repo, CLion and autotools - we need the
# source files listed for CLion to work, but all of the other directrives
# will fail since we're using autotools. jhrg 5/20/21

include_directories(include)
include_directories(.)

add_executable(mk_stare
		src/mk_stare.cpp
		src/SidecarFile.cpp
		src/GeoFile.cpp
		src/Modis05L2GeoFile.cpp
		src/Modis09L2GeoFile.cpp
		src/Modis09GAGeoFile.cpp
		src/ModisGeoFile.cpp
		src/STAREmaster.c

		include/SidecarFile.h
		include/GeoFile.h
		include/Modis05L2GeoFile.h
		include/Modis09L2GeoFile.h
		include/Modis09GAGeoFile.h
		include/ModisGeoFile.h
		include/STAREmaster.h
		include/ssc.h
		src/print_stare.cpp)

add_executable(print_stare
		src/print_stare.cpp)
