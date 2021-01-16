/// @file
/// This class contains the main functionality for creating and
/// manipulating sidecar files.

/// @author Ed Hartnett @date 3/16/20

#include <config.h>
#include <string>
#include <sstream>
#include <iostream>
#include <iomanip>
#include "ssc.h"

#ifndef SIDECAR_FILE_H_
#define SIDECAR_FILE_H_

class SidecarFile
{
public:
    int writeFile(const std::string fileName, int verbose,
		  int quiet, int build_level, int i, int j,
		  double *geo_lat, double *geo_lon,
		  unsigned long long *index);

    /** Write a STARE index. */
    int writeSTAREIndex(int verbose, int quiet, int build_level, int i, int j,
			double *geo_lat, double *geo_lon,
			unsigned long long *stare_index);
    /** Write a STARE cover */
    int writeSTARECover(int verbose, int quiet,
			int stare_cover_size, unsigned long long *stare_cover);

    /* Read a STARE sidecar file. */
    int readFile(const std::string fileName, int verbose);
    
    /** Close the file. */
    int closeFile();
};

#endif /* SIDECAR_FILE_H_ */
