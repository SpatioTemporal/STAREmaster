#include <string>
#include <sstream>
#include <iostream>
#include <iomanip>
#include "Modis05L2GeoFile.h"
#include "NetcdfSidecarFile.h"

#define MAX_STR 256
#define ERR 1
int
main()
{
    Modis05L2GeoFile gf;
    NetcdfSidecarFile sf;
    std::string fileName = "data/MOD05_L2.A2005349.2125.061.2017294065400.hdf";
    std::string fileNameOut = "t1_sidecar.nc";
    int gf_format;
    int verbose = 1;
    
    std::cout << "Howdy!\n";

    // Test getting sidecarFileName.
    if (gf.sidecarFileName("bb") != "bb_stare.nc")
        return ERR;
    if (gf.sidecarFileName("bb.nc") != "bb_stare.nc")
        return ERR;

    // What type of file is this?
    if (gf.determineFormat(fileName, &gf_format))
	return ERR;
    if (gf_format != SSC_FORMAT_MODIS_L2) return 1;

    // Read the file.
    if (gf.readFile(fileName, 1, 5, -1, false, 1))
	return ERR;

    // Create the sidecar file.
    if (sf.createFile(fileNameOut, 1, NULL))
	return ERR;
    
    // Write the sidecar file.
    if (sf.writeSTAREIndex(1, 5, gf.geo_num_i1[0], gf.geo_num_j1[0],
			   gf.geo_lat1[0], gf.geo_lon1[0], gf.geo_index1[0], gf.var_name[0], "1km"))
	return ERR;

    // Close the sidecar file.
    if (sf.closeFile())
	return ERR;

    // Read the sidecar file.
    if (sf.readSidecarFile(fileNameOut, verbose))
        return ERR;

    return 0;
}
