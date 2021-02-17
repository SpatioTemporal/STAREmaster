/// @file

/// This class contains the main functionality for reading a file that
/// may be given a STARE sidecar file.

/// Ed Hartnett 4/4/20

#ifndef GEO_FILE_H_ /**< Protect file from double include. */
#define GEO_FILE_H_

#include <config.h>
#include <string>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <vector>
#include "ssc.h"
#include "STARE.h"
using namespace std;

#define MAX_NUM_INDEX 10 /**< Max number of STARE index vars in a file. */

/**
 * This is the base class for a data file with geolocation.
 */
class GeoFile
{
public:
    GeoFile();
    ~GeoFile();
    
    int determineFormat(const std::string fileName, int *gf_format);

    /** Read file. */
    int readFile(const string fileName, int verbose, int quiet, int build_level);

    /** Get STARE index sidecar filename. */
    string sidecarFileName(const string fileName);

    int readSidecarFile(const std::string fileName, int verbose, int &ncid);

    /** Get STARE index for data variable. */
    int getSTAREIndex(const std::string varName, int verbose, int ncid, int &varid,
		      size_t &my_size_i, size_t &my_size_j);

    /** Close sidecar file. */
    int closeSidecarFile(int verbose, int ncid);

    int num_index; /**< Number of STARE indices needed for this file. */
    int *geo_num_i1; /**< Number of I. */
    int *geo_num_j1; /**< Number of J. */
    double **geo_lat1; /**< Array of latitude values. */
    double **geo_lon1; /**< Array of longitude values. */
    unsigned long long **geo_index1; /**< Array of STARE index. */

    int num_cover;
    unsigned long long **geo_cover1; /**< Array of STARE index intervals. */
    int *geo_num_cover_values1;
    vector<string> var_name[MAX_NUM_INDEX]; /**< Names of vars that use this index. */
    STARE_SpatialIntervals cover;

    int cover_level;
    int perimeter_stride;

    vector<string> stare_index_name;
    vector<string> stare_cover_name;
    vector<string> variables;

    vector<size_t> size_i, size_j;
    vector<int> stare_varid;
};

#endif /* GEO_FILE_H_ */
