/// @file

/// This class contains the main functionality for reading a file that
/// may be given a STARE sidecar file.

/// Ed Hartnett 4/4/20

#ifndef GEO_FILE_H_ /**< Protect file from double include. */
#define GEO_FILE_H_

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
class GeoFile {
public:
    GeoFile();

    ~GeoFile();

    /** Read file. */
    int readFile(const string fileName, int verbose, int quiet, int build_level);

    /** Get STARE index sidecar filename. */
    string sidecar_filename(const string &file_name);

    int read_sidecar_file(const std::string fileName, int &ncid);


    int read_sidecar_file(const std::string fileName, int verbose, int &ncid);

    /** Get STARE indices for data variable. */
    int get_stare_indices(const std::string varName, int ncid, vector<unsigned long long> &values);

    /** Close sidecar file. */
    int close_sidecar_file(int ncid);

    int d_num_index; /**< Number of STARE index sets needed for this file. */
    int d_ncid; ///< id of the open netCDF4 file    
    vector<int> geo_num_i; /**< Number of I. */
    vector<int> geo_num_j; /**< Number of J. */
    vector<vector<double>> geo_lat;
    vector<vector<double>> geo_lon;
    vector<vector<unsigned long long int>> geo_index;

    int num_cover; /**< Number of covers. */
    vector<vector<unsigned long long int>> geo_cover; /**< The covers. */
    vector<int> geo_num_cover_values; /**< Size of each cover. */
    vector<string> var_name[MAX_NUM_INDEX]; /**< Names of vars that use this index. */
    STARE_SpatialIntervals cover;

    int cover_level;
    int perimeter_stride;

    vector<string> d_stare_index_name;
    vector<string> stare_cover_name;
    vector<string> d_variables;

#if 1
    // I think these are only used by the old getSTAREIndex() method
    vector<size_t> d_size_i, d_size_j;
    vector<int> d_stare_varid;
#endif
};

#endif /* GEO_FILE_H_ */
