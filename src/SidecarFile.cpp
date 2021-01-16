/// @file
/// This class contains the main functionality for creating and
/// manipulating sidecar files.

/// @author Ed Hartnett @date 3/16/20

#include "config.h"
#include "SidecarFile.h"

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
 */
int
SidecarFile::readFile(const std::string fileName, int verbose)
{
    if (verbose) std::cout << "Reading sidecar file " << fileName << "\n";
    return 0;
}
    
