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
    int writeSTAREIndex(int verbose, int quiet, int build_level, int i, int j,
			double *geo_lat, double *geo_lon,
			unsigned long long *stare_index);
    int closeFile();
};

#endif /* SIDECAR_FILE_H_ */
