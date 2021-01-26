/// @file
/// This class contains the main functionality for creating and
/// manipulating sidecar files.

/// @author Ed Hartnett @date 3/16/20

#include "config.h"
#include "SidecarFile.h"
#include "ssc.h"
#include <netcdf.h>
#include <cstring>

#define NDIM2 2

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
 * @param num_index Reference to an int that will get the number of
 * STARE indexes in the file.
 * @param stare_index_name Reference to a vector of string which hold
 * the names of the STARE index variables.
 * @return 0 for success, error code otherwise.
 */
int
SidecarFile::readSidecarFile(const std::string fileName, int verbose, int &num_index,
                             vector<string> &stare_index_name, vector<size_t> &size_i,
                             vector<size_t> &size_j, int &ncid)
{
    char title_in[NC_MAX_NAME + 1];
    int ndims, nvars;
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

    // How many vars and dims?
    if ((ret = nc_inq(ncid, &ndims, &nvars, NULL, NULL)))
        return ret;

    // Find all variables that are STARE indexes.
    for (int v = 0; v < nvars; v++)
    {
        char var_name[NC_MAX_NAME + 1];
        char long_name_in[NC_MAX_NAME + 1];
        nc_type xtype;
        int ndims, dimids[NDIM2], natts;
        size_t dimlen[NDIM2];

        // Learn about this var.
        if ((ret = nc_inq_var(ncid, v, var_name, &xtype, &ndims, dimids, &natts)))
            return ret;

        if (verbose) std::cout << "var " << var_name << " type " << xtype <<
                         " ndims " << ndims << "\n";

        // Get the long_name attribute value.
        if ((ret = nc_get_att_text(ncid, v, SSC_LONG_NAME, long_name_in)))
            continue;

        // If this is a STARE index, learn about it.
        num_index = 0;
        if (!strncmp(long_name_in, SSC_INDEX_LONG_NAME, NC_MAX_NAME))
        {
            char variables_in[NC_MAX_NAME + 1];

            // Find the length of the dimensions.
            if ((ret = nc_inq_dimlen(ncid, dimids[0], &dimlen[0])))
                return ret;
            if ((ret = nc_inq_dimlen(ncid, dimids[1], &dimlen[1])))
                return ret;
            
            // What variables does this STARE index apply to?
            if ((ret = nc_get_att_text(ncid, v, SSC_INDEX_VAR_ATT_NAME, variables_in)))
                return ret;

            // Save the name of this STARE index variable.
            stare_index_name.push_back(var_name);

            // Save the dimensions of the STARE index var.
            size_i.push_back(dimlen[0]);
            size_j.push_back(dimlen[1]);

            // Keep count of how many STARE indexes we find in the file.
            num_index++;
            if (verbose)
                std::cout << "variable_in " << variables_in << "\n";
        }
    }

    return 0;
}
    
