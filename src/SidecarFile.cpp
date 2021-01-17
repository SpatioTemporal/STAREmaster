/// @file
/// This class contains the main functionality for creating and
/// manipulating sidecar files.

/// @author Ed Hartnett @date 3/16/20

#include "config.h"
#include "SidecarFile.h"
#include "ssc.h"
#include <netcdf.h>
#include <cstring>

/**
 * Write a sidecar file.
 */
int
SidecarFile::writeFile(const std::string fileName, int verbose,
		       int quiet, int build_level, int i, int j,
		       double *geo_lat, double *geo_lon,
		       unsigned long long *index)
{
    if (verbose) std::cout << "Writing sidecar file " << fileName <<
		     " with build level " << build_level << "\n";
    return 0;
}

/**
 * Read a sidecare file.
 *
 * @param fileName Name of the sidecar file.
 * @param verbose Set to non-zero to enable verbose output for
 * debugging.
 * @return 0 for success, error code otherwise.
 */
int
SidecarFile::readSidecarFile(const std::string fileName, int verbose)
{
    int ncid;
    char title_in[NC_MAX_NAME + 1];
    int ret;
    
    if (verbose) std::cout << "Reading sidecar file " << fileName << "\n";

    // Open the sidecar file.
    if ((ret = nc_open(fileName.c_str(), NC_NOWRITE, &ncid)))
        return ret;

    // Check the title attribute to make sure this is a sidecar file.
    if ((ret = nc_get_att_text(ncid, NC_GLOBAL, SSC_TITLE_NAME, title_in)))
        return ret;
    if (strncmp(title_in, SSC_TITLE, NC_MAX_NAME))
        return SSC_NOT_SIDECAR;

    // Close the sidecar file.
    if ((ret = nc_close(ncid)))
        return ret;
    
    return 0;
}
    
