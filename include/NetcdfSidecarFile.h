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

/** This macro prints an error message with line number and name of
 * test program. */
#define NCERR(ret) do {							\
        fflush(stdout); /* Make sure our stdout is synced with stderr. */ \
        fprintf(stderr, "Sorry! Unexpected result, %s, line: %d %s\n",     \
                __FILE__, __LINE__, nc_strerror(ret));			\
        fflush(stderr);                                                 \
	return SSC_ENETCDF;						\
    } while (0)


/**
 * Class for writing a netCDF sidecar file.
 */
class NetcdfSidecarFile : public SidecarFile
{
private:
    int ncid;
    
public:
    int createFile(const std::string fileName, int verbose, char *institution);
    int writeSTAREIndex(int verbose, int build_level, int i, int j,
			double *geo_lat, double *geo_lon, unsigned long long *stare_index,
			vector<string> var_name, string stare_index_name);
    int writeSTARECover(int verbose, int stare_cover_size, unsigned long long *stare_cover,
			string stare_cover_name);
    int closeFile();
};

#endif /* NETCDF_SIDECAR_FILE_H_ */
