// This is a test program for the STAREmaster project.
//
// This program tests MOD043K.
//
// Ed Hartnett 8/16/20

#include <string>
#include <sstream>
#include <iostream>
#include <iomanip>
#include "ModisL2GeoFile.h"
#include "Modis043KGeoFile.h"
#include "NetcdfSidecarFile.h"

#define MAX_STR 256
#define ERR 1
int
main()
{
    Modis043KGeoFile gf;
//    NetcdfSidecarFile sf;
    std::string fileName = "data/MYD04_3K.A2020001.0040.061.2020002235147.hdf";
    std::string fileNameOut = "t2_MOD043K_sidecar.nc";
//    int gf_format;
    
    std::cout << "Howdy!\n";

    // Read the file.
    if (gf.readFile(fileName, 1, 0, 5))
    	return ERR;

    // // Create the sidecar file.
    // if (sf.createFile(fileNameOut, 1))
    // 	return ERR;
    
    // // Write the sidecar file.
    // if (sf.writeSTAREIndex(1, 0, 5, gf.geo_num_i1[0], gf.geo_num_j1[0],
    // 			   gf.geo_lat1[0], gf.geo_lon1[0], gf.geo_index1[0], gf.var_name[0], "1km"))
    // 	return ERR;

    // // Close the sidecar file.
    // if (sf.closeFile())
    // 	return ERR;

    return 0;
}
