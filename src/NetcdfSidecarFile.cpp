/// @file
/// This class contains the main functionality for creating and
/// manipulating NETCDF sidecar files.

/// @author Ed @date Hartnett 4/11/20

#include "config.h"
#include "ssc.h"
#include "SidecarFile.h"
#include "NetcdfSidecarFile.h"
#include <netcdf.h>
#include <stdio.h>
#include <vector>

using namespace std;

/** This macro prints an error message with line number and name of
 * test program. */
#define NCERR(ret) do {							\
        fflush(stdout); /* Make sure our stdout is synced with stderr. */ \
        fprintf(stderr, "Sorry! Unexpected result, %s, line: %d %s\n",     \
                __FILE__, __LINE__, nc_strerror(ret));			\
        fflush(stderr);                                                 \
	return SSC_ENETCDF;						\
    } while (0)


/**
 * Create a sidecar file.
 */
int
NetcdfSidecarFile::createFile(const std::string fileName, int verbose)
{
    int ret;
    
    if (verbose) std::cout << "Creating NETCDF sidecar file " << fileName << "\n";

    // Create a netCDF/HDF5 file.
    if ((ret = nc_create(fileName.c_str(), NC_CLOBBER|NC_NETCDF4, &ncid)))
	NCERR(ret);

    return 0;
}

/**
 * Write a sidecar file.
 */
int
NetcdfSidecarFile::writeSTAREIndex(int verbose, int quiet, int build_level, int i, int j,
				   double *geo_lat, double *geo_lon, unsigned long long *stare_index,
				   vector<string> var_name, string stare_index_name)
{
    int dimid[SSC_NDIM2];
    int lat_varid, lon_varid, index_varid;
    string var_att;
    string dim_name;
    int ret;
    
    if (verbose) std::cout << "Writing NETCDF sidecar indicies with build level " << build_level << "\n";

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
    for (int i = 0; i < (int)var_name.size(); i++)
    {
	var_att.append(var_name.at(i).c_str());
	if (i < (int)var_name.size() - 1)
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
 */
int
NetcdfSidecarFile::writeSTARECover(int verbose, int quiet,
				   int stare_cover_size, unsigned long long *stare_cover,
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
NetcdfSidecarFile::closeFile()
{
    return nc_close(ncid);
}

