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

/** This macro prints an error message with line number and name of
 * test program. */
#define NCERR(ret) do {                            \
        fflush(stdout); /* Make sure our stdout is synced with stderr. */ \
        fprintf(stderr, "Sorry! Unexpected result, %s, line: %d %s\n",     \
                __FILE__, __LINE__, nc_strerror(ret));            \
        fflush(stderr);                                                 \
    return SSC_ENETCDF;                        \
    } while (0)

using std::vector;
using std::string;

class SidecarFile {
private:
    int ncid;

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
    int read_sidecar_file(const std::string fileName, int verbose, int &num_index,
                        vector<string> &stare_index_name, vector<size_t> &size_i,
                        vector<size_t> &size_j, vector<string> &variables,
                        vector<int> &stare_varid, int &ncid);

    int createFile(const std::string fileName, int verbose, char *institution);

    int writeSTAREIndex(int verbose, int build_level, int i, int j,
                        double *geo_lat, double *geo_lon, unsigned long long *stare_index,
                        vector<string> var_name, string stare_index_name);

    int writeSTARECover(int verbose, int stare_cover_size, unsigned long long *stare_cover,
                        string stare_cover_name);

    /** Close the file. */
    int close_file();
};

#endif /* SIDECAR_FILE_H_ */
