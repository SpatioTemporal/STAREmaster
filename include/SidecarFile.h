/// @file
/// This class contains the main functionality for creating and
/// manipulating sidecar files.

/// @author Ed Hartnett @date 3/16/20

#include <config.h>
#include <string>
#include <vector>
#include <sstream>
#include <iostream>
#include <iomanip>
#include "ssc.h"

#ifndef SIDECAR_FILE_H_
#define SIDECAR_FILE_H_

using std::vector;
using std::string;

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
    int readSidecarFile(const std::string fileName, int verbose, int &num_index,
                        vector<string> &stare_index_name, vector<size_t> &size_i,
                        vector<size_t> &size_j, vector<string> &variables, int &ncid);
    
    /** Close the file. */
    int closeFile();
};

#endif /* SIDECAR_FILE_H_ */
