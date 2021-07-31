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
#define NAME_CONVENTIONS "Conventions"
#define NAME_INSTITUTION "institution"
#define NAME_SOURCE "source"
#define NAME_HISTORY "history"
#define MAX_TIME_STR 128
#define CF_VERSION "CF-1.8"

/**
 * Create a sidecar file.
 *
 * @param fileName The name of the sidecar file to create.
 * @param verbose Set to non-zero for verbose output.
 * @param institution_c Pointer to char array with text describing the
 * creating insitution, to support CF convention.
 * @return 0 for success, error code otherwise.
 */
int
SidecarFile::createFile(const std::string fileName, int verbose, char *institution_c) {
    int ret;
    string title = SSC_TITLE;
    string institution = "";
    string source = "";
    string history = "";
    string cf_version = CF_VERSION;

    if (verbose) std::cout << "Creating NETCDF sidecar file " << fileName << "\n";

    // Create a netCDF/HDF5 file.
    if ((ret = nc_create(fileName.c_str(), NC_CLOBBER | NC_NETCDF4, &ncid)))
        NCERR(ret);

    // Write some attributes to conform with CF conventions. See
    // https://cfconventions.org/Data/cf-conventions/cf-conventions-1.8/cf-conventions.html#_attributes.

    // From CF: Files that follow this version of the CF Conventions
    // must indicate this by setting the NUG defined global attribute
    // Conventions to a string value that contains "CF-1.8".
    if ((ret = nc_put_att_text(ncid, NC_GLOBAL, NAME_CONVENTIONS, cf_version.size() + 1, cf_version.c_str())))
        NCERR(ret);

    // title - A succinct description of what is in the dataset.
    if ((ret = nc_put_att_text(ncid, NC_GLOBAL, SSC_TITLE_NAME, title.size() + 1, title.c_str())))
        NCERR(ret);

    // institution - Specifies where the original data was produced.
    if (institution_c)
        institution.append(institution_c);
    if (institution.size())
        if ((ret = nc_put_att_text(ncid, NC_GLOBAL, NAME_INSTITUTION, institution.size() + 1, institution.c_str())))
            NCERR(ret);

    // source - The method of production of the original data. If it
    // was model-generated, source should name the model and its
    // version, as specifically as could be useful. If it is
    // observational, source should characterize it (e.g., "surface
    // observation" or "radiosonde").
    source.append("STAREmaster ");
    source.append(PACKAGE_VERSION);
    if (source.size())
        if ((ret = nc_put_att_text(ncid, NC_GLOBAL, NAME_SOURCE, source.size() + 1, source.c_str())))
            NCERR(ret);

    // history - Provides an audit trail for modifications to the
    // original data. Well-behaved generic netCDF filters will
    // automatically append their name and the parameters with which
    // they were invoked to the global history attribute of an input
    // netCDF file. We recommend that each line begin with a timestamp
    // indicating the date and time of day that the program was
    // executed.

    // Get the current date/time.
    char time_str[MAX_TIME_STR + 1];
    time_t time_ptr = time(NULL);
    strftime(time_str, MAX_TIME_STR, "%F %T", localtime(&time_ptr));
    history.append(time_str);
    history.append(" - STAREmaster ");
    history.append(fileName);
    if (history.size())
        if ((ret = nc_put_att_text(ncid, NC_GLOBAL, NAME_HISTORY, history.size() + 1, history.c_str())))
            NCERR(ret);

    return 0;
}

/**
 * Write a sidecar file.
 *
 * @param verbose Set to non-zero for verbose output.
 * @param build_level STARE build level.
 * @param i Size of latitude array.
 * @param j Size of longitude array.
 * @parma geo_lat Pointer to array of latitudes.
 * @param geo_lon Pointer to array of longitudes.
 * @param stare_index Pointer to array of STARE indexes.
 * @param var_name Vector of string with variable names this STARE
 * index applies to.
 * @param stare_index_name Name of the variable that will hold this
 * STARE index.
 * @return 0 for success, error code otherwise.
 */
int
SidecarFile::writeSTAREIndex(int verbose, int build_level, int i, int j,
                             double *geo_lat, double *geo_lon, unsigned long long *stare_index,
                             vector <string> var_name, string stare_index_name) {
    int dimid[SSC_NDIM2];
    int lat_varid, lon_varid, index_varid;
    string var_att;
    string dim_name;
    int ret;

    if (verbose) std::cout << "Writing NETCDF sidecar indices with build level " << build_level << "\n";

    // Define dimensions.
    dim_name.append(SSC_I_NAME);
    dim_name.append("_");
    dim_name.append(stare_index_name);
    if ((ret = nc_def_dim(ncid, dim_name.c_str(), i, &dimid[0])))
        NCERR(ret);

    dim_name.clear();
    dim_name.append(SSC_J_NAME);
    dim_name.append("_");
    dim_name.append(stare_index_name);
    if ((ret = nc_def_dim(ncid, dim_name.c_str(), j, &dimid[1])))
        NCERR(ret);

    // Define latitude.
    string lat_name;
    lat_name.append(SSC_LAT_NAME);
    lat_name.append("_");
    lat_name.append(stare_index_name);
    if ((ret = nc_def_var(ncid, lat_name.c_str(), NC_DOUBLE, SSC_NDIM2, dimid, &lat_varid)))
        NCERR(ret);
    if ((ret = nc_def_var_deflate(ncid, lat_varid, 1, 1, 3)))
        NCERR(ret);
    if ((ret = nc_put_att_text(ncid, lat_varid, SSC_LONG_NAME, sizeof(SSC_LAT_LONG_NAME),
                               SSC_LAT_LONG_NAME)))
        NCERR(ret);
    if ((ret = nc_put_att_text(ncid, lat_varid, SSC_UNITS, sizeof(SSC_LAT_UNITS),
                               SSC_LAT_UNITS)))
        NCERR(ret);

    // Define longitude.
    string lon_name;
    lon_name.append(SSC_LON_NAME);
    lon_name.append("_");
    lon_name.append(stare_index_name);
    if ((ret = nc_def_var(ncid, lon_name.c_str(), NC_DOUBLE, SSC_NDIM2, dimid, &lon_varid)))
        NCERR(ret);
    if ((ret = nc_def_var_deflate(ncid, lon_varid, 1, 1, 3)))
        NCERR(ret);
    if ((ret = nc_put_att_text(ncid, lon_varid, SSC_LONG_NAME, sizeof(SSC_LON_LONG_NAME),
                               SSC_LON_LONG_NAME)))
        NCERR(ret);
    if ((ret = nc_put_att_text(ncid, lon_varid, SSC_UNITS, sizeof(SSC_LON_UNITS),
                               SSC_LON_UNITS)))
        NCERR(ret);

    // Define STARE index.
    string index_name;
    index_name.append(SSC_INDEX_NAME);
    index_name.append("_");
    index_name.append(stare_index_name);
    if ((ret = nc_def_var(ncid, index_name.c_str(), NC_UINT64, SSC_NDIM2, dimid, &index_varid)))
        NCERR(ret);
    if ((ret = nc_def_var_deflate(ncid, index_varid, 1, 1, 3)))
        NCERR(ret);
    if ((ret = nc_put_att_text(ncid, index_varid, SSC_LONG_NAME, sizeof(SSC_INDEX_LONG_NAME),
                               SSC_INDEX_LONG_NAME)))
        NCERR(ret);

    // Add attribute with list of variables.
    for (int i = 0; i < (int) var_name.size(); i++) {
        var_att.append(var_name[i].c_str());
        if (i < (int) var_name.size() - 1)
            var_att.append(", ");
    }
    if ((ret = nc_put_att_text(ncid, index_varid, SSC_INDEX_VAR_ATT_NAME, var_att.size() + 1,
                               var_att.c_str())))
        NCERR(ret);

    // Write data.
    if ((ret = nc_put_var(ncid, lat_varid, geo_lat)))
        NCERR(ret);
    if ((ret = nc_put_var(ncid, lon_varid, geo_lon)))
        NCERR(ret);
    if ((ret = nc_put_var(ncid, index_varid, stare_index)))
        NCERR(ret);

    return 0;
}


/**
 * Write a cover to the file.
 *
 * @param verbose Set to non-zero for verbose output to stdout.
 * @param stare_cover_size Size of the STARE cover.
 * @param stare_cover Pointer to array which is the STARE cover.
 * @param stare_cover_name Name of the STARE cover.
 * @return zero for success, error code otherwise.
 */
int
SidecarFile::writeSTARECover(int verbose, int stare_cover_size, unsigned long long *stare_cover,
                             string stare_cover_name) {

    if (verbose) std::cout << "Writing NETCDF sidecar cover." << "\n";

    string dim_name;

    int cover_dimid[SSC_NDIM1];
    int cover_varid;

    int ret;

    // Define STARE cover.

    dim_name.clear();
    dim_name.append(SSC_L_NAME);
    dim_name.append("_");
    dim_name.append(stare_cover_name);
    if ((ret = nc_def_dim(ncid, dim_name.c_str(), stare_cover_size, &cover_dimid[0])))
        NCERR(ret);

    string cover_name;
    cover_name.append(SSC_COVER_NAME);
    cover_name.append("_");
    cover_name.append(stare_cover_name);

    // SSC_NDIM?
    if ((ret = nc_def_var(ncid, cover_name.c_str(), NC_UINT64, SSC_NDIM1, cover_dimid, &cover_varid)))
        NCERR(ret);
    if ((ret = nc_def_var_deflate(ncid, cover_varid, 1, 1, 3)))
        NCERR(ret);
    if ((ret = nc_put_att_text(ncid, cover_varid, SSC_LONG_NAME, sizeof(SSC_COVER_LONG_NAME),
                               SSC_COVER_LONG_NAME)))
        NCERR(ret);

    if ((ret = nc_put_var(ncid, cover_varid, stare_cover)))
        NCERR(ret);

    return 0;
}

/**
 * Close a sidecar file.
 */
int
SidecarFile::close_file()
{
    return nc_close(ncid);
}

/**
 * Write a sidecar file.
 */
int
SidecarFile::writeFile(const std::string fileName, int verbose,
                       int quiet, int build_level, int i, int j,
                       double *geo_lat, double *geo_lon,
                       unsigned long long *index) {
    if (verbose)
        std::cout << "Writing sidecar file " << fileName <<
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
 * @param size_i vector with the sizes of I for each STARE index.
 * @param size_j vector with the sizes of J for each STARE index.
 * @param variables vector of strings with variables each STARE index
 * applies to.
 * @param ncid The ncid of the opened sidecar file.
 * @return 0 for success, error code otherwise.
 */
int
SidecarFile::read_sidecar_file(const std::string fileName, int verbose, int &num_index,
			       vector<string> &stare_index_name, vector<size_t> &size_i,
			       vector<size_t> &size_j, vector<string> &variables,
			       vector<int> &stare_varid, int &ncid)
{
    if (verbose) std::cout << "Reading sidecar file " << fileName << "\n";

    // Open the sidecar file.
    int ret;
    if ((ret = nc_open(fileName.c_str(), NC_NOWRITE, &ncid)))
        return ret;

    // Check the title attribute to make sure this is a sidecar file.
    char title_in[NC_MAX_NAME + 1];
    if ((ret = nc_get_att_text(ncid, NC_GLOBAL, SSC_TITLE_NAME, title_in)))
        return ret;
    if (strncmp(title_in, SSC_TITLE, NC_MAX_NAME))
        return SSC_NOT_SIDECAR;

    // How many vars and dims?
    int ndims, nvars;
    if ((ret = nc_inq(ncid, &ndims, &nvars, NULL, NULL)))
        return ret;

    // Find all variables that are STARE indexes.
    num_index = 0;
    for (int v = 0; v < nvars; v++) {
        char var_name[NC_MAX_NAME + 1];
        char long_name_in[NC_MAX_NAME + 1];
        nc_type xtype;
        int ndims, dimids[NDIM2], natts;
        size_t dimlen[NDIM2];

        // Learn about this var.
        if ((ret = nc_inq_var(ncid, v, var_name, &xtype, &ndims, dimids, &natts)))
            return ret;

        if (verbose)
            std::cout << "var " << var_name << " type " << xtype <<
                      " ndims " << ndims << "\n";

        // Get the long_name attribute value.
        if ((ret = nc_get_att_text(ncid, v, SSC_LONG_NAME, long_name_in)))
            continue;

        // If this is a STARE index, learn about it.
        if (!strncmp(long_name_in, SSC_INDEX_LONG_NAME, NC_MAX_NAME)) {
            char variables_in[NC_MAX_NAME + 1];

            // Save the varid.
            stare_varid.push_back(v);

            // Find the length of the dimensions.
            if ((ret = nc_inq_dimlen(ncid, dimids[0], &dimlen[0])))
                return ret;
            if ((ret = nc_inq_dimlen(ncid, dimids[1], &dimlen[1])))
                return ret;

            // What variables does this STARE index apply to?
            if ((ret = nc_get_att_text(ncid, v, SSC_INDEX_VAR_ATT_NAME, variables_in)))
                return ret;
            std::string var_list = variables_in;
            variables.push_back(var_list);

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
    
