/// @file
/// This class contains the main functionality for creating and
/// manipulating sidecar files.

/// @author Ed Hartnett @date 3/16/20

#include <config.h>
#include <string>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <vector>
#include "ssc.h"
#include "SidecarFile.h"

using namespace std;

#ifndef NETCDF_SIDECAR_FILE_H_
#define NETCDF_SIDECAR_FILE_H_

/**
 * Class for writing a netCDF sidecar file.
 */
class NetcdfSidecarFile : public SidecarFile
{
private:
    int ncid;
    
public:
    int createFile(const std::string fileName, int verbose);
    int writeSTAREIndex(int verbose, int quiet, int build_level, int i, int j,
			double *geo_lat, double *geo_lon, unsigned long long *stare_index,
			vector<string> var_name, string stare_index_name);
    int closeFile();
};

#endif /* NETCDF_SIDECAR_FILE_H_ */
