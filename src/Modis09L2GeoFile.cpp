// This class is a GeoFile for HDF4 format files.

// Ed Hartnett 4/4/20

#include "config.h"
#include "Modis09L2GeoFile.h"
#include <mfhdf.h>
#include <hdf.h>
#include <vector>
#include <HdfEosDef.h>
#include "STARE.h"

#ifdef HAVE_OPENMP
#include <omp.h>
#endif

using namespace std;

#define MAX_NAME 256
#define MAX_DIMS 16

#define MAX_ALONG 2030
#define MAX_ACROSS 1354
#define MAX_ALONG_500 4060
#define MAX_ACROSS_500 2708
#define MAX_ALONG_250 (MAX_ALONG_500 * 2)
#define MAX_ACROSS_250 (MAX_ACROSS_500 * 2)

/**
 * Read a HDF4 MODIS L2 MOD09 file.
 *
 * @param fileName the data file name.
 * @param verbose non-zero for verbose output to stdout.
 * @param build_level STARE build level.
 *
 * @return 0 for no error, error code otherwise.
 */
int
Modis09L2GeoFile::readFile(const std::string fileName, int verbose, int build_level) {
    int32 swathfileid, swathid;
    int32 ndims, dimids[MAX_DIMS];
    float32 *longitude;
    float32 *latitude;
    char dimnames[MAX_NAME + 1];
    int32 dimsize;
    int32 ngeofields;
    int32 ndatafields;
    char fieldlist[MAX_NAME * 10 + 1];
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

    // TODO Make these vector<vector<float32>>. jhrg 6/9/21
    d_num_index = 3;
    // if (!(geo_index1 = (unsigned long long **) malloc(d_num_index * sizeof(unsigned long long *))))
    //     return SSC_ENOMEM;

    if (!(geo_num_cover_values1 = (int *) malloc(d_num_index * sizeof(int))))
        return SSC_ENOMEM;

    d_stare_index_name.push_back("1km");  //Added jhrg 6/9/21
    stare_cover_name.push_back("1km");
    var_name[0].push_back("1km Atmospheric Optical Depth Band 1");
    var_name[0].push_back("1km Atmospheric Optical Depth Band 3");
    var_name[0].push_back("1km Atmospheric Optical Depth Band 8");
    var_name[0].push_back("1km Atmospheric Optical Depth Model");
    var_name[0].push_back("1km water_vapor");
    var_name[0].push_back("1km Atmospheric Optical Depth Band QA");
    var_name[0].push_back("1km Atmospheric Optical Depth Band CM");

    // Open the swath file.
    if ((swathfileid = SWopen((char *) fileName.c_str(), DFACC_RDONLY)) < 0)
        return SSC_EHDF4ERR;

    if ((nswath = SWinqswath((char *) fileName.c_str(), swathlist, &strbufsize)) < 0)
        return SSC_EHDF4ERR;
    if (verbose) std::cout << "nswath " << nswath << " " << swathlist << "\n";

    // Attach to a swath.
    string MODIS_SWATH_TYPE_L2 = "MODIS SWATH TYPE L2";
    if ((swathid = SWattach(swathfileid, (char *) MODIS_SWATH_TYPE_L2.c_str())) < 0)
        return SSC_EHDF4ERR;

    if (!(longitude = (float32 *) calloc(MAX_ALONG * MAX_ACROSS, sizeof(float32))))
        return SSC_ENOMEM;
    if (!(latitude = (float32 *) calloc(MAX_ALONG * MAX_ACROSS, sizeof(float32))))
        return SSC_ENOMEM;

    // Get lat and lon values.
    string LONGITUDE = "Longitude";
    if (SWreadfield(swathid, (char *) LONGITUDE.c_str(), NULL, NULL, NULL, longitude))
        return SSC_EHDF4ERR;
    string LATITUDE = "Latitude";
    if (SWreadfield(swathid, (char *) LATITUDE.c_str(), NULL, NULL, NULL, latitude))
        return SSC_EHDF4ERR;

    geo_num_i.push_back(MAX_ALONG);
    geo_num_j.push_back(MAX_ACROSS);
    // if (!(geo_index1[0] = (unsigned long long *) calloc(geo_num_i.at(0) * geo_num_j.at(0),
    // 							sizeof(unsigned long long))))
    //     return SSC_ENOMEM;

    int level = 27;
    int finest_resolution = 0;

    // Calculate STARE index for each point.
    STARE index1(level, build_level);
    vector<double> lats;
    vector<double> lons;
    vector<unsigned long long int> geo_index_1;

#ifdef USE_OPENMP
#pragma omp parallel reduction(max : finest_resolution)
    {
#pragma omp for
        for (int i = 0; i < MAX_ALONG; i++) {
            for (int j = 0; j < MAX_ACROSS; j++) {
		lats.push_back(latitude[i * MAX_ACROSS + j]);
		lons.push_back(longitude[i * MAX_ACROSS + j]);

                // Calculate the stare indices.
                // geo_index1[0][i * MAX_ACROSS + j] = index1.ValueFromLatLonDegrees((double) latitude[i * MAX_ACROSS + j],
                //                                                                   (double) longitude[i * MAX_ACROSS +
                //                                                                                      j], level);
		geo_index_1.push_back(index1.ValueFromLatLonDegrees((double) latitude[i * MAX_ACROSS + j],
								   (double) longitude[i * MAX_ACROSS + j],
								   level));
            }
#if 0
            // index1.adaptSpatialResolutionEstimatesInPlace(&(geo_index1[0][i * MAX_ACROSS]), MAX_ACROSS);
            // // finest_resolution is never used. jhrg 6/9/21
            // for (int j = 0; j < MAX_ACROSS; j++) {
            //     int test_resolution = geo_index1[0][i * MAX_ACROSS + j] & 31; // LevelMask
            //     if (test_resolution > finest_resolution) {
            //         finest_resolution = test_resolution;
            //     }
            // }
#endif
        } // next i
    }

#else
    {
        unsigned long length = MAX_ALONG * MAX_ACROSS;
        for (unsigned long i = 0; i < length; ++i) {
	    lats.push_back(latitude[i]);
	    lons.push_back(longitude[i]);

            // Calculate the stare indices.
            // geo_index1[0][i] = index1.ValueFromLatLonDegrees(lats[i], lons[i], level);
	    geo_index_1.push_back(index1.ValueFromLatLonDegrees(lats[i], lons[i], level));
        }

#if 0
        // for (unsigned long i = 0; i < MAX_ALONG; ++i)
        //     index1.adaptSpatialResolutionEstimatesInPlace(&(geo_index1[0][i * MAX_ACROSS]), MAX_ACROSS);
        // for (unsigned long i = 0; i < MAX_ALONG; ++i)
        //     index1.adaptSpatialResolutionEstimatesInPlace(&(geo_index1[0][i * MAX_ACROSS]), MAX_ACROSS);
#endif
    }
#endif /* USE_OPENMP */

    geo_lat.push_back(lats);
    geo_lon.push_back(lons);
    geo_index.push_back(geo_index_1);

    // Learn about dims for this swath.
    if ((ndims = SWinqdims(swathid, dimnames, dimids)) < 0)
        return SSC_EHDF4ERR;
    if (verbose) std::cout << "ndims " << ndims << " " << dimnames << "\n";

    std::stringstream ss(dimnames);
    std::vector<std::string> result;
    while (ss.good()) {
        std::string substr;
        getline(ss, substr, ',');
        result.push_back(substr);

        // Get a dimsize.
        if ((dimsize = SWdiminfo(swathid, (char *) substr.c_str())) < 0)
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
    if (verbose)
        std::cout << "nattr " << nattr << " " << attrlist << " strbufsize " <<
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

    {
	vector<double> lats_500;
	vector<double> lons_500;
	vector<unsigned long long int> geo_index_500;
        geo_num_i.push_back(MAX_ALONG_500);
        geo_num_j.push_back(MAX_ACROSS_500);
        // if (!(geo_index1[1] = (unsigned long long *) calloc(geo_num_i[1] * geo_num_j[1],
        //                                                     sizeof(unsigned long long))))
        //     return SSC_ENOMEM;

        // Calculate STARE index for each point.
        int m = 0;
        for (int i = 0; i < MAX_ALONG_500; i++) {
            if (i && !(i % 2)) m++;
            int n = 0;
            for (int j = 0; j < MAX_ACROSS_500; j++) {
		int ret;
		double lat_delta, lon_delta;

                if (j && !(j % 2)) n++;
		if (n == 0)
		{
		    lon_delta = abs(longitude[m * MAX_ACROSS + n] - longitude[m * MAX_ACROSS + n + 1]);
		}
		else
		{
		    lon_delta = abs(longitude[m * MAX_ACROSS + n] - longitude[m * MAX_ACROSS + n - 1]);
		}
		if (m == 0)
		{
		    lat_delta = abs(latitude[m * MAX_ACROSS + n] - latitude[m * MAX_ACROSS + n + MAX_ACROSS]);
		}
		else
		{
		    lat_delta = abs(latitude[m * MAX_ACROSS + n] - latitude[m * MAX_ACROSS + n - MAX_ACROSS]);
		}
		if (i < 10 && j < 10)
		    printf("i %d j %d lat_delta %g lon_delta %g\n", i, j, lat_delta, lon_delta);

		// Deal with meridian.
		if (lon_delta >= 0.4)
		{
		    lon_delta = 360 - abs(lon_delta);
		}
		if (lon_delta >= 0.4)
		{
		    printf("i %d j %d lon_delta %g\n", i, j, lon_delta);
		    printf("m %d n %d longitude[m * MAX_ACROSS + n] %g longitude[m * MAX_ACROSS + n - 1] %g\n", m, n,
			   longitude[m * MAX_ACROSS + n], longitude[m * MAX_ACROSS + n - 1]);
		    printf("(longitude[m * MAX_ACROSS + n] - longitude[m * MAX_ACROSS + n - 1]) %g\n",
			   (longitude[m * MAX_ACROSS + n] - longitude[m * MAX_ACROSS + n - 1]));
		    printf("abs((longitude[m * MAX_ACROSS + n] - longitude[m * MAX_ACROSS + n - 1])) %g\n",
			   abs((longitude[m * MAX_ACROSS + n] - longitude[m * MAX_ACROSS + n - 1])));
		    printf("abs(longitude[m * MAX_ACROSS + n] - longitude[m * MAX_ACROSS + n + 1]) %g\n",
			   abs(longitude[m * MAX_ACROSS + n] - longitude[m * MAX_ACROSS + n + 1]));
		    return 99;
		}
		if (lat_delta >= 0.4)
		{
		    printf("i %d j %d lat_delta %g\n", i, j, lat_delta);
		    return 99;
		}
		lats_500.push_back(latitude[m * MAX_ACROSS + n] + (j % 2) * lat_delta / 2.0);
		lons_500.push_back(longitude[m * MAX_ACROSS + n] + (j % 2) * lon_delta / 2.0);

                // Calculate the stare indices.
		// geo_index1[1][i * MAX_ACROSS_500 + j] = index1.ValueFromLatLonDegrees((double)latitude[m * MAX_ACROSS + n],
                //  								     (double)longitude[m * MAX_ACROSS + n], level);
		geo_index_500.push_back(index1.ValueFromLatLonDegrees((double)latitude[m * MAX_ACROSS + n],
								      (double)longitude[m * MAX_ACROSS + n], level));
            }
        }

	geo_lat.push_back(lats_500);
	geo_lon.push_back(lons_500);
	geo_index.push_back(geo_index_500);
        d_stare_index_name.push_back("500m");
        stare_cover_name.push_back("500m");
        var_name[1].push_back("500m Surface Reflectance Band 1");
        var_name[1].push_back("500m Surface Reflectance Band 2");
        var_name[1].push_back("500m Surface Reflectance Band 3");
        var_name[1].push_back("500m Surface Reflectance Band 4");
        var_name[1].push_back("500m Surface Reflectance Band 5");
        var_name[1].push_back("500m Surface Reflectance Band 6");
        var_name[1].push_back("500m Surface Reflectance Band 7");
    }
    {
	vector<double> lats_250;
	vector<double> lons_250;
	vector<unsigned long long int> geo_index_250;
        geo_num_i.push_back(MAX_ALONG_250);
        geo_num_j.push_back(MAX_ACROSS_250);
        // if (!(geo_index1[2] = (unsigned long long *) calloc(geo_num_i.at(2) * geo_num_j.at(2),
        //                                                     sizeof(unsigned long long))))
        //     return SSC_ENOMEM;

        // Calculate STARE index for each point.
        int m = 0;
        for (int i = 0; i < MAX_ALONG_250; i++) {
            if (i && !(i % 4)) m++;
            int n = 0;
            for (int j = 0; j < MAX_ACROSS_250; j++) {
		int ret;
		double lat_delta, lon_delta;
                if (j && !(j % 4)) n++;
		if (n == 0)
		{
		    lon_delta = abs(longitude[m * MAX_ACROSS + n] - longitude[m * MAX_ACROSS + n + 1]);
		}
		else
		{
		    lon_delta = abs(longitude[m * MAX_ACROSS + n] - longitude[m * MAX_ACROSS + n - 1]);
		}
		if (m == 0)
		{
		    lat_delta = abs(latitude[m * MAX_ACROSS + n] - latitude[m * MAX_ACROSS + n + MAX_ACROSS]);
		}
		else
		{
		    lat_delta = abs(latitude[m * MAX_ACROSS + n] - latitude[m * MAX_ACROSS + n - MAX_ACROSS]);
		}
		if (i < 10 && j < 10)
		    printf("i %d j %d lat_delta %g lon_delta %g\n", i, j, lat_delta, lon_delta);

		// Deal with meridian.
		if (lon_delta >= 0.4)
		{
		    lon_delta = 360 - abs(lon_delta);
		}
		if (lon_delta >= 0.4)
		{
		    printf("i %d j %d lon_delta %g\n", i, j, lon_delta);
		    printf("m %d n %d longitude[m * MAX_ACROSS + n] %g longitude[m * MAX_ACROSS + n - 1] %g\n", m, n,
			   longitude[m * MAX_ACROSS + n], longitude[m * MAX_ACROSS + n - 1]);
		    printf("(longitude[m * MAX_ACROSS + n] - longitude[m * MAX_ACROSS + n - 1]) %g\n",
			   (longitude[m * MAX_ACROSS + n] - longitude[m * MAX_ACROSS + n - 1]));
		    printf("abs((longitude[m * MAX_ACROSS + n] - longitude[m * MAX_ACROSS + n - 1])) %g\n",
			   abs((longitude[m * MAX_ACROSS + n] - longitude[m * MAX_ACROSS + n - 1])));
		    printf("abs(longitude[m * MAX_ACROSS + n] - longitude[m * MAX_ACROSS + n + 1]) %g\n",
			   abs(longitude[m * MAX_ACROSS + n] - longitude[m * MAX_ACROSS + n + 1]));
		    return 99;
		}
		if (lat_delta >= 0.4)
		{
		    printf("i %d j %d lat_delta %g\n", i, j, lat_delta);
		    return 99;
		}
		lats_250.push_back(latitude[m * MAX_ACROSS + n] + (j % 4) * lat_delta / 4.0);
		lons_250.push_back(longitude[m * MAX_ACROSS + n] + (j % 4) * lon_delta / 4.0);

                // Calculate the stare indices.
                //geo_index1[2][i * MAX_ACROSS_250 + j] = geo_index1[0][m * MAX_ACROSS + n];
		// geo_index1[2][i * MAX_ACROSS_250 + j] = index1.ValueFromLatLonDegrees((double)latitude[m * MAX_ACROSS + n],
                //  								     (double)longitude[m * MAX_ACROSS + n], level);
		geo_index_250.push_back(index1.ValueFromLatLonDegrees((double)latitude[m * MAX_ACROSS + n],
								      (double)longitude[m * MAX_ACROSS + n], level));
            }
        }
	geo_lat.push_back(lats_250);
	geo_lon.push_back(lons_250);
	geo_index.push_back(geo_index_250);


	{
	    int m = 0;
	    for (int i = 0; i < 10; i++) {
		if (i && !(i % 4)) m++;
		int n = 0;
		for (int j = 0; j < 10; j++) {
		    if (j && !(j % 4)) n++;
		    // printf("latitude[%d]=%g\n", i * m * MAX_ACROSS + n + j, latitude[m * MAX_ACROSS + n]);
		}
	    }
	}
	
        d_stare_index_name.push_back("250m");
        stare_cover_name.push_back("250m");
        var_name[2].push_back("250m Surface Reflectance Band 1");
        var_name[2].push_back("250m Surface Reflectance Band 2");
        var_name[2].push_back("250m Surface Reflectance Band 3");
        var_name[2].push_back("250m Surface Reflectance Band 4");
        var_name[2].push_back("250m Surface Reflectance Band 5");
        var_name[2].push_back("250m Surface Reflectance Band 6");
        var_name[2].push_back("250m Surface Reflectance Band 7");
    }

    free(longitude);
    free(latitude);

    return 0;
}

