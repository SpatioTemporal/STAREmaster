/* This is a test file for the STAREmaster project. */
#include "config.h"
#include <string>
#include <sstream>
#include <iostream>
#include <iomanip>
#include "Modis05L2GeoFile.h"
#include "SidecarFile.h"

#define MAX_STR 256
#define ERR 1

int
main() {
    Modis05L2GeoFile gf;
    SidecarFile sf;
    std::string fileName = "data/MOD05_L2.A2005349.2125.061.2017294065400.hdf";
    std::string fileNameOut = "t2_sidecar.nc";
    int gf_format;

    std::cout << "Howdy!\n";

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
    if (sf.writeSTAREIndex(1, 5, gf.geo_num_i[0], gf.geo_num_j[0],
                           &gf.geo_lat[0][0], &gf.geo_lon[0][0], &gf.geo_index[0][0], gf.var_name[0], "1km"))
        return ERR;

    // Close the sidecar file.
    if (sf.close_file())
	return ERR;
    
    return 0;
}
