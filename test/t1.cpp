/* This is a test file for the STAREmaster project. */
#include "config.h"
#include <string>
#include <vector>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <netcdf.h>
#include "Modis05L2GeoFile.h"
#include "SidecarFile.h"

#define MAX_STR 256
#define ERR 1

int
main() {
    GeoFile gf_in;
    Modis05L2GeoFile gf;
    SidecarFile sf;
    std::string fileName = "data/MOD05_L2.A2005349.2125.061.2017294065400.hdf";
    std::string fileNameOut = "t1_sidecar.nc";
    int gf_format;
    int verbose = 1;

    std::cout << "Howdy!\n";

    // Test getting sidecarFileName.
    if (gf.sidecar_filename("bb") != "bb_stare.nc")
        return ERR;
    if (gf.sidecar_filename("bb.nc") != "bb_stare.nc")
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
    if (sf.writeSTAREIndex(1, 5, gf.geo_num_i[0], gf.geo_num_j[0],
                           &gf.geo_lat[0][0], &gf.geo_lon[0][0], &gf.geo_index[0][0], gf.var_name[0], "1km"))
        return ERR;

    // Close the sidecar file.
    if (sf.close_file())
	return ERR;

    // Read the sidecar file.
    int ncid;
    int num_index;
    vector <string> stare_index_name, variables;
    vector <size_t> size_i, size_j;
    vector<int> stare_varid;
    if (sf.read_sidecar_file(fileNameOut, verbose, num_index, stare_index_name, size_i,
			   size_j, variables, stare_varid, ncid))
        return ERR;
    if (nc_close(ncid))
        return ERR;

    // Check results.
    if (num_index != 1) return ERR;
    if (stare_index_name.size() != 1 || variables.size() != 1) return ERR;
    if (stare_index_name.at(0) != "STARE_index_1km") return ERR;
    if (variables.at(0) !=
        "Scan_Start_Time, Solar_Zenith, Solar_Azimuth, Water_Vapor_Infrared, Quality_Assurance_Infrared")
        return ERR;
    if (size_i.size() != 1 || size_i.at(0) != 406) return ERR;
    if (size_j.size() != 1 || size_j.at(0) != 270) return ERR;
    if (stare_varid.size() != 1 || stare_varid.at(0) != 2) return ERR;

    // Read it again with GeoFile.
    if (gf_in.read_sidecar_file(fileNameOut, ncid))
        return ERR;
    string varName = "Scan_Start_Time";
#if 0
    int varid;
    size_t si, sj;
    if (gf_in.getSTAREIndex(varName, 1, ncid, varid, si, sj))
        return ERR;
    if (varid != 2) return ERR;
#endif

    vector<unsigned long long> values;
    if (gf_in.get_stare_indices(varName, ncid, values))
        return ERR;

    if (gf_in.close_sidecar_file(ncid))
        return ERR;
    return 0;
}
