// This class is a GeoFile for HDF4 format files.

// Ed Hartnett 4/4/20

#include "config.h"
#include "ModisL2GeoFile.h"
#include <mfhdf.h>
#include <hdf.h>
#include <HdfEosDef.h>
#include "STARE.h"

#define MAX_NAME 256
#define MAX_DIMS 16

#define MAX_ALONG 406
#define MAX_ACROSS 270

/**
 * Read a HDF4 MODIS L2 file.
 */
int
ModisL2GeoFile::readFile(const std::string fileName, int verbose, int quiet,
			 int build_level)
{
    int32 swathfileid, swathid;
    int32 ndims, dimids[MAX_DIMS];
    float32 longitude[MAX_ALONG][MAX_ACROSS];
    float32 latitude[MAX_ALONG][MAX_ACROSS];
    char dimnames[MAX_NAME + 1];
    int32 dimsize;
    int32 ngeofields;
    int32 ndatafields;
    char fieldlist[MAX_NAME + 1];
    int32 rank[MAX_DIMS];
    int32 numbertype[MAX_DIMS];
    int32 nmaps;
    char dimmap[MAX_NAME + 1];
    int32 offset[MAX_DIMS], increment[MAX_DIMS];
    int32 nidxmaps;
    char idxmap[MAX_NAME + 1];
    int32 idxsizes[MAX_DIMS];
    int32 nattr;
    int32 strbufsize;
    char attrlist[MAX_NAME + 1] = "";
    int32 nswath;
    char swathlist[MAX_NAME + 1];
    
    if (verbose) std::cout << "Reading HDF4 file " << fileName <<
		     " with build level " << build_level << "\n";

    num_index = 1;
    if (!(geo_num_i1 = (int *)malloc(num_index * sizeof(int))))
	return SSC_ENOMEM;	
    if (!(geo_num_j1 = (int *)malloc(num_index * sizeof(int))))
	return SSC_ENOMEM;	
    if (!(geo_lat1 = (double **)malloc(num_index * sizeof(double *))))
	return SSC_ENOMEM;	
    if (!(geo_lon1 = (double **)malloc(num_index * sizeof(double *))))
	return SSC_ENOMEM;	
    if (!(geo_index1 = (unsigned long long **)malloc(num_index * sizeof(unsigned long long *))))
	return SSC_ENOMEM;	

    // Open the swath file.
    if ((swathfileid = SWopen(fileName.c_str(), DFACC_RDONLY)) < 0)
	return SSC_EHDF4ERR;

    if ((nswath = SWinqswath(fileName.c_str(), swathlist, &strbufsize)) < 0)
	return SSC_EHDF4ERR;
    if (verbose) std::cout << "nswath " << nswath << " " << swathlist << "\n";    

    // Attach to a swath.
    if ((swathid = SWattach(swathfileid, "mod05")) < 0)
	return SSC_EHDF4ERR;

    // Get lat and lon values.
    if (SWreadfield(swathid, "Longitude", NULL, NULL, NULL, longitude))
	return SSC_EHDF4ERR;
    if (SWreadfield(swathid, "Latitude", NULL, NULL, NULL, latitude))
	return SSC_EHDF4ERR;

    geo_num_i1[0] = MAX_ALONG;
    geo_num_j1[0] = MAX_ACROSS;
    if (!(geo_lat1[0] = (double *)calloc(geo_num_i1[0] * geo_num_j1[0], sizeof(double))))
	return SSC_ENOMEM;
    if (!(geo_lon1[0] = (double *)calloc(geo_num_i1[0] * geo_num_j1[0], sizeof(double))))
	return SSC_ENOMEM;
    if (!(geo_index1[0] = (unsigned long long *)calloc(geo_num_i1[0] * geo_num_j1[0],
						   sizeof(unsigned long long))))
	return SSC_ENOMEM;

    int level = 27;
    STARE index(level, build_level);
    
    // Calculate STARE index for each point. 
    for (int i = 0; i < MAX_ALONG; i++)
    {
    	for (int j = 0; j < MAX_ACROSS; j++)
	{
	    geo_lat1[0][i * MAX_ACROSS + j] = latitude[i][j];
	    geo_lon1[0][i * MAX_ACROSS + j] = longitude[i][j];
	    
	    // Calculate the stare indicies.
	    geo_index1[0][i * MAX_ACROSS + j] = index.ValueFromLatLonDegrees((double)latitude[i][j],
									 (double)longitude[i][j], level);
	}
	index.adaptSpatialResolutionEstimatesInPlace( &(geo_index1[0][i * MAX_ACROSS]), MAX_ACROSS );
    }    
    // Learn about dims for this swath.
    if ((ndims = SWinqdims(swathid, dimnames, dimids)) < 0)
	return SSC_EHDF4ERR;
    if (verbose) std::cout << "ndims " << ndims << " " << dimnames << "\n";

    std::stringstream ss(dimnames);
    std::vector<std::string> result;
    while (ss.good())
    {
	std::string substr;
	getline(ss, substr, ',' );
	result.push_back(substr);

	// Get a dimsize.
	if ((dimsize = SWdiminfo(swathid, substr.c_str())) < 0)
	    return SSC_EHDF4ERR;
	if (verbose) std::cout << "dim " << substr << " dimsize " << dimsize << "\n";

    }

    for (std::size_t i = 0; i < result.size(); i++)
        std::cout << result[i] << std::endl;
     
    if ((ngeofields = SWinqgeofields(swathid, fieldlist, rank, numbertype)) < 0)
	return SSC_EHDF4ERR;
    if (verbose) std::cout << "ngeofields " << ngeofields << " " << fieldlist << "\n";

    if ((ndatafields = SWinqdatafields(swathid, fieldlist, rank, numbertype)) < 0)
	return SSC_EHDF4ERR;
    if (verbose) std::cout << "ndatafields " << ndatafields << " " << fieldlist << "\n";

    if ((nmaps = SWinqmaps(swathid, dimmap, offset, increment)) < 0)
	return SSC_EHDF4ERR;
    if (verbose) std::cout << "nmaps " << nmaps << " " << dimmap << "\n";

    if ((nidxmaps = SWinqidxmaps(swathid, idxmap, idxsizes)) < 0)
	return SSC_EHDF4ERR;
    if (verbose) std::cout << "nidxmaps " << nidxmaps << " " << idxmap << "\n";

    if ((nattr = SWinqattrs(swathid, attrlist, &strbufsize)) < 0)
	return SSC_EHDF4ERR;
    if (verbose) std::cout << "nattr " << nattr << " " << attrlist << " strbufsize " <<
		     strbufsize << "\n";
    

    // if (SWmapinfo(swathid, "Latitude", "Cell_Across_Swath_5km/Cell_Across_Swath_1km",
    // 		  offset, increment))
    // 	return SSC_EHDF4ERR;
    
    // Detach from the swath.
    if (SWdetach(swathid) < 0)
	return SSC_EHDF4ERR;

    // Close the swath file.
    if (SWclose(swathfileid) < 0)
	return SSC_EHDF4ERR;
    
    return 0;
}
