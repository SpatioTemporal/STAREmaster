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

/** Construct a ModisL2GeoFile.
 *
 * @return a ModisL2GeoFile
 */
ModisL2GeoFile::ModisL2GeoFile()
{
    cout<<"ModisL2GeoFile constructor\n";
    
}

/** Destroy a ModisL2GeoFile.
 *
 */
ModisL2GeoFile::~ModisL2GeoFile()
{
    cout<<"ModisL2GeoFile destructor\n";
}

/**
 * Read a HDF4 MODIS L2 MOD05 file.
 *
 * @param fileName the data file name.
 * @param verbose non-zero for verbose output to stdout.
 * @param quiet non-zero for no output.
 * @param build_level STARE build level.
 *
 * @return 0 for no error, error code otherwise.
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

    stare_index_name.push_back("1km");
    stare_index_name.push_back("500m");
    stare_index_name.push_back("250m");
    stare_cover_name.push_back("1km");
    
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

    num_cover = 1;
    if (!(geo_num_cover_values1 = (int *)malloc(num_cover * sizeof(int))))
      return SSC_ENOMEM;
    if (!(geo_cover1 = (unsigned long long **)malloc(num_cover * sizeof(unsigned long long *))))
      return SSC_ENOMEM;
    /*
    if (!(cover1 = (STARE_SpatialIntervals *)malloc(num_cover * sizeof(STARE_SpatialIntervals))))
      return SSC_ENOMEM;
    */

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

    int finest_resolution = 0;
    
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

	for (int j = 0; j < MAX_ACROSS; j++) {
	  int test_resolution = geo_index1[0][i * MAX_ACROSS + j] & 31; // LevelMask
	  if ( test_resolution > finest_resolution ) {
	    finest_resolution = test_resolution;
	  }
	}
    }

    LatLonDegrees64ValueVector perimeter(2*MAX_ALONG+2*MAX_ACROSS-4);

    std::cout << "calculating perimeter" << "\n" << std::flush;

    int pk = 2*MAX_ALONG+2*MAX_ACROSS-4 - 1;
    { // Go along the 'bottom' CCW. Do the full side.
      int i = 0;
	{
	  for (int j = 0; j < MAX_ACROSS; j++)
	    {
	      perimeter[pk].lat = latitude[i][j];
	      perimeter[pk].lon = longitude[i][j];
	      --pk;
	    }
	}
    }
    
    { // Go up along the right side. Start at 1, because the corner's done.
      int j = MAX_ACROSS-1;
      for (int i = 1; i < MAX_ALONG; i++)
	{
	    {
	      perimeter[pk].lat = latitude[i][j];
	      perimeter[pk].lon = longitude[i][j];
	      --pk;
	    }
	}
    }
    
    { // Go back along the top. Start one over.
	int i = MAX_ALONG-1;
	{
	  for (int j = MAX_ACROSS-2; j > -1; j--)
	    {
	      perimeter[pk].lat = latitude[i][j];
	      perimeter[pk].lon = longitude[i][j];
	      --pk;
	    }
	}
    }
    
    { // Go back down along the left side. Start one over and don't include the end.
      int j = 0;
      for (int i = MAX_ALONG-2; i > 0; i--)
	{
	    {
	      perimeter[pk].lat = latitude[i][j];
	      perimeter[pk].lon = longitude[i][j];
	      --pk;
	    }
	}
    }

    if (verbose) std::cout << "perimeter size = " << perimeter.size() << ", pk = " << pk << "\n" << std::flush;
    
//    cover1[0]                = index.NonConvexHull(perimeter,finest_resolution);
    cover = index.NonConvexHull(perimeter,finest_resolution);

    if (verbose) std::cout << "cover size = " << cover.size()  << "\n";
    
// <<<<<<< HEAD
//     geo_num_cover_values1[0] = cover1[0].size();
// 
//     // If we can't avoid copying...
//     // if (!(geo_cover1[0] = (unsigned long long *)calloc(geo_num_cover_values1[0],sizeof(unsigned long long))))
//     //	return SSC_ENOMEM;
//     
//     geo_cover1[0]            = &(cover1[0])[0];
// =======
// //    geo_cover1[0]            = &(cover1[0])[0];
// //    geo_num_cover_values1[0] = cover1[0].size();
//     geo_num_cover_values1[0] = cover.size();    
// >>>>>>> ddaa8660c5d3f7734a00940592a3e323728abbdb

    geo_num_cover_values1[0] = cover.size();    
    if (!(geo_cover1[0] = (unsigned long long *)calloc(geo_num_cover_values1[0],sizeof(unsigned long long))))
      return SSC_ENOMEM;
    for(int k=0; k<geo_num_cover_values1[0]; ++k) {
      geo_cover1[0][k] = cover[k];
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
