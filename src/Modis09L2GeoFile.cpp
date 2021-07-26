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

double
itk_1km_to_250m(double i_tk_1km) {
    return 1.5 + 4. * i_tk_1km;
}

//    return the 1km grid index along track of a 250m pixel
double
itk_250m_to_1km (double i_tk_250m ) {
    return (i_tk_250m - 1.5) / 4.;
}

//    return the 250m grid index cross track of a 1km pixel
float
isc_1km_to_250m (float i_sc_1km ) {
    return 4. * i_sc_1km;
}

// return the 1km grid index cross track of a 250m pixel
float
isc_250m_to_1km (float i_sc_250m ) {
    return i_sc_250m / 4.;
}

// Width = 2330, nscan_lines = 1354
// return the (lat,lon) of a 250m pixel specified with its indexes, when the geolocation datasets are given at 1km resolution
// @param itk_250m grid index of the 250m pixel along-track
// @param isc_250m grid index of the 250m pixel cross-track
// @param lat_1km latitudes dataset at 1km resolution
// @param lon_1km longitudes dataset at 1km resolution
// @return the ( lat, lon ) of the 250m pixel  [ itk_250m, isc_250m ]
int
get_250m_pix_pos (double itk_250m, double isc_250m, float *lat_1km,
		  float *lon_1km, float *lat, float *lon) {

    // # change date meridian case sentinel
    int is_crossing_change_date = 0;

    // # check 250m indexes validity
    int sz_sc_1km  = MAX_ACROSS;
    int sz_sc_250m = MAX_ACROSS_250;

    int sz_tk_1km = MAX_ALONG;
    int sz_tk_250m = 4 * sz_tk_1km;
    if ((isc_250m < 0) || (isc_250m > sz_sc_250m - 1))
	return -1;
    if ((itk_250m < 0) || (itk_250m > sz_tk_250m - 1))
	return -1;

    // --- set the bounding 1km pixels to take for interpolation
    // set the (track,scan) indexes of the 1km pixel in the 250m grid
    double itk_1km = itk_250m_to_1km(itk_250m);
    double isc_1km = isc_250m_to_1km(isc_250m);

    printf("i_250m=[%.2f, %.2f] -> i_1km=[%.2f, %.2f]\n", itk_250m, isc_250m,
	   itk_1km, isc_1km);

    // the width of one scan, in number of pixels, at 250m resolution
    int w_scan_250m = 40;
    int itk_top_1km;
    int itk_bottom_1km;
    // extra/interpolation 1km pixels along track
    // if ((itk_250m % w_scan_250m) <= 1) { // extrapolate start of scan
    // 	itk_top_1km    = ceil(itk_1km);
    // 	itk_bottom_1km = itk_top_1km + 1;
    // elif ( itk_250m % w_scan_250m ) >= 38 : # extrapolate end of scan
    //     itk_top_1km    = math.floor ( itk_1km )
    //     itk_bottom_1km = itk_top_1km - 1
    // else : # general case : middle of scan
    //     itk_top_1km    = math.floor ( itk_1km )
    //     itk_bottom_1km = itk_top_1km + 1
    // # - extra/interpolation 1km pixels along track
    // if ( isc_1km >= 1353. ) : # extrapolate end of scan line
    //     isc_left_1km  = math.floor ( isc_1km ) - 1
    //     isc_right_1km = math.floor ( isc_1km )
    // else : # general case : interpolation
    //     isc_left_1km  = math.floor ( isc_1km )
    //     isc_right_1km = isc_left_1km + 1
    // }

    // #print "itk_top_1km=%d itk_bottom_1km=%d isc_left_1km=%d isc_right_1km=%d"%(itk_top_1km, itk_bottom_1km, isc_left_1km, isc_right_1km)

    // # --- set the 1km track lines position ; left border ---
    // lat_left_250m, lon_left_250m   = get_y_pos_1km_to_250m (
    //             isc_left_1km, itk_top_1km, itk_bottom_1km,
    //             lat_1km, lon_1km,
    //             itk_250m )
    // # --- set the 1km track lines position ; right border ---
    // lat_right_250m, lon_right_250m = get_y_pos_1km_to_250m (
    //             isc_right_1km, itk_top_1km, itk_bottom_1km,
    //             lat_1km, lon_1km,
    //             itk_250m )

    // #print "left_250m=[%f,%f] right_250m=[%f,%f]"%(lat_left_250m, lon_left_250m, lat_right_250m, lon_right_250m)

    // # check for change date meridian case
    // if  abs ( lon_right_250m  - lon_left_250m  ) > 180. :
    //     is_crossing_change_date = True
    //     # all negative longitudes will be incremented of 360 before interpolation
    //     if lon_left_250m < 0. :
    //         lon_left_250m += 360.
    //     if lon_right_250m < 0. :
    //         lon_right_250m += 360.

    // # for each track line position, interpolate along scan to retrieve the 250m geolocation
    // lat, lon = get_x_pos_1km_to_250m ( lat_left_250m,  lon_left_250m,  isc_left_1km,
    //                                  lat_right_250m, lon_right_250m, isc_right_1km,
    //                                  isc_250m )
    // #print "geolocation = [%f, %f]"%(lat,lon)

    // # in case of change date crossing, turn values > 180. to negative
    // if lon > 180.:
    //     lon -= 360.
    // elif lon < -180.:
    //     lon += 360.
    // return lat, lon
    return 0;
}

int
get_y_pos_1km_to_250m (float isc_1km, float itk_p1_1km, float itk_p2_1km,
		       float *lat_1km, float *lon_1km, float itk_250m,
		       float *lat_left_250m, float *lon_left_250m) {
    // return the position of 250m pixel defined by its along-track index between 2 successive 1km pixels on a same cross-track line
    // @warning If the 2 pixels are crossing the changing date meridian, the negative longitude will be returned
    // with an increment of 360 to manage correctly the interpolation
    // @param isc_1km index of the cross-track line in the 1km grid
    // @param itk_p1_1km index of the first along-track pixel in the 1km grid
    // @param itk_p2_1km index of the second along-track pixel in the 1km grid
    // @param lat_1km latitudes dataset at 1km resolution
    // @param lon_1km longitudes dataset at 1km resolution
    // @param itk_250m index of the along-track 250m along-track pixel to interpolate
    // @return (lat,lon) of the pixel at [itk_250m, isc_1km], interpolated between the 2 successive 1km pixels p1 and p2
    // make sure P1 and P1 are 2 successive pixels along-track
    if (abs(itk_p1_1km - itk_p2_1km) != 1)
	return -1;

    // // lat, lon of the 1km bounding pixels
    // p1_1km_lat = lat_1km [ itk_p1_1km, isc_1km ]
    // p1_1km_lon = lon_1km [ itk_p1_1km, isc_1km ]
    // p2_1km_lat = lat_1km [ itk_p2_1km, isc_1km ]
    // p2_1km_lon = lon_1km [ itk_p2_1km, isc_1km ]

    // # check for change date meridian particular case

    // # change date meridian case sentinel
    // if abs ( p1_1km_lon - p2_1km_lon ) > 180. :
    //     if p1_1km_lon < 0. :
    //         p1_1km_lon += 360.
    //     elif p2_1km_lon < 0. :
    //         p2_1km_lon += 360.

    // # coordinates of p1, p2 in the 250m grid
    // itk_p1_250m = itk_1km_to_250m ( itk_p1_1km )
    // itk_p2_250m = itk_1km_to_250m ( itk_p2_1km )

    // #print "itk_p1_250m=%f itk_p2_250m=%f"%( itk_p1_250m, itk_p2_250m )

    // # linear interpolation on the position
    // alpha_lon = ( p2_1km_lon - p1_1km_lon ) / ( itk_p2_250m - itk_p1_250m )
    // alpha_lat = ( p2_1km_lat - p1_1km_lat ) / ( itk_p2_250m - itk_p1_250m )
    // lon = p1_1km_lon + alpha_lon * ( itk_250m - itk_p1_250m )
    // lat = p1_1km_lat + alpha_lat * ( itk_250m - itk_p1_250m )

    // # in case of change date crossing, turn values > 180. to negative
    // if lon > 180. :
    //     lon -= 360.
    // elif lon < -180. :
    //     lon += 360.

    // return lat, lon
    return 0;
}

/**
 * Interpolate the 1km resolution lat/lon to 250m. Based on python
 * code from
 * https://github.com/SpatioTemporal/GeoData/tree/master/geodata/modis_coarse_to_fine_geolocation.
 *
 */
int
interp_lat(float32 *latitude, int i, int j, int m, int n, double *lat_out)
{
    *lat_out = latitude[m * MAX_ACROSS + n];
    printf("lat_out = %g\n", *lat_out);
    return 0;
}

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

    // TODO Make these vector<vector<float32>>. jhrg 6/9/21
    d_num_index = 3;
    if (!(geo_num_i1 = (int *) malloc(d_num_index * sizeof(int))))
        return SSC_ENOMEM;
    if (!(geo_num_j1 = (int *) malloc(d_num_index * sizeof(int))))
        return SSC_ENOMEM;
    if (!(geo_lat1 = (double **) malloc(d_num_index * sizeof(double *))))
        return SSC_ENOMEM;
    if (!(geo_lon1 = (double **) malloc(d_num_index * sizeof(double *))))
        return SSC_ENOMEM;
    if (!(geo_index1 = (unsigned long long **) malloc(d_num_index * sizeof(unsigned long long *))))
        return SSC_ENOMEM;

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
#if 1
    // Get lat and lon values.
    string LONGITUDE = "Longitude";
    if (SWreadfield(swathid, (char *) LONGITUDE.c_str(), NULL, NULL, NULL, longitude))
        return SSC_EHDF4ERR;
    string LATITUDE = "Latitude";
    if (SWreadfield(swathid, (char *) LATITUDE.c_str(), NULL, NULL, NULL, latitude))
        return SSC_EHDF4ERR;
#endif
    geo_num_i1[0] = MAX_ALONG;
    geo_num_j1[0] = MAX_ACROSS;
    if (!(geo_lat1[0] = (double *) calloc(geo_num_i1[0] * geo_num_j1[0], sizeof(double))))
        return SSC_ENOMEM;
    if (!(geo_lon1[0] = (double *) calloc(geo_num_i1[0] * geo_num_j1[0], sizeof(double))))
        return SSC_ENOMEM;
    if (!(geo_index1[0] = (unsigned long long *) calloc(geo_num_i1[0] * geo_num_j1[0], sizeof(unsigned long long))))
        return SSC_ENOMEM;

    int level = 27;
    int finest_resolution = 0;

    // Calculate STARE index for each point.
    STARE index1(level, build_level);

#if 0
#pragma omp parallel reduction(max : finest_resolution)
    {
        STARE index1(level, build_level);
#pragma omp for
        for (int i = 0; i < MAX_ALONG; i++) {
            for (int j = 0; j < MAX_ACROSS; j++) {
                geo_lat1[0][i * MAX_ACROSS + j] = latitude[i * MAX_ACROSS + j];
                geo_lon1[0][i * MAX_ACROSS + j] = longitude[i * MAX_ACROSS + j];

                // Calculate the stare indices.
                geo_index1[0][i * MAX_ACROSS + j] = index1.ValueFromLatLonDegrees((double) latitude[i * MAX_ACROSS + j],
                                                                                  (double) longitude[i * MAX_ACROSS +
                                                                                                     j], level);
            }
#if 0
            index1.adaptSpatialResolutionEstimatesInPlace(&(geo_index1[0][i * MAX_ACROSS]), MAX_ACROSS);
            // finest_resolution is never used. jhrg 6/9/21
            for (int j = 0; j < MAX_ACROSS; j++) {
                int test_resolution = geo_index1[0][i * MAX_ACROSS + j] & 31; // LevelMask
                if (test_resolution > finest_resolution) {
                    finest_resolution = test_resolution;
                }
            }
#endif
        }
    }
#endif

#if 1
    {

        // geo_lat1, _lon1 and _index1 are each three arrays of MAX_ALONG by MAX_ACROSS
        // values that will hold the lat, lon, and index values. 'latitude' and longitude'
        // are arrays of lat and lon values.

        unsigned long length = MAX_ALONG * MAX_ACROSS;
        for (unsigned long i = 0; i < length; ++i) {
            geo_lat1[0][i] = (double) latitude[i];
            geo_lon1[0][i] = (double) longitude[i];

            // Calculate the stare indices.
            geo_index1[0][i] = index1.ValueFromLatLonDegrees(geo_lat1[0][i], geo_lon1[0][i], level);
        }

#if 0
        for (unsigned long i = 0; i < MAX_ALONG; ++i)
            index1.adaptSpatialResolutionEstimatesInPlace(&(geo_index1[0][i * MAX_ACROSS]), MAX_ACROSS);
#endif
#if 0
        for (unsigned long i = 0; i < MAX_ALONG; ++i)
            index1.adaptSpatialResolutionEstimatesInPlace(&(geo_index1[0][i * MAX_ACROSS]), MAX_ACROSS);
#endif
    }
#endif

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
        geo_num_i1[1] = MAX_ALONG_500;
        geo_num_j1[1] = MAX_ACROSS_500;
        if (!(geo_lat1[1] = (double *) calloc(geo_num_i1[1] * geo_num_j1[1], sizeof(double))))
            return SSC_ENOMEM;
        if (!(geo_lon1[1] = (double *) calloc(geo_num_i1[1] * geo_num_j1[1], sizeof(double))))
            return SSC_ENOMEM;
        if (!(geo_index1[1] = (unsigned long long *) calloc(geo_num_i1[1] * geo_num_j1[1],
                                                            sizeof(unsigned long long))))
            return SSC_ENOMEM;

        // Calculate STARE index for each point.
        int m = 0;
        for (int i = 0; i < MAX_ALONG_500; i++) {
            if (i && !(i % 2)) m++;
            int n = 0;
            for (int j = 0; j < MAX_ACROSS_500; j++) {
                if (j && !(j % 2)) n++;
                geo_lat1[1][i * MAX_ACROSS_500 + j] = latitude[m * MAX_ACROSS + n];
                geo_lon1[1][i * MAX_ACROSS_500 + j] = longitude[m * MAX_ACROSS + n];

                geo_index1[1][i * MAX_ACROSS_500 + j] = geo_index1[0][m * MAX_ACROSS + n];
            }
        }

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
        geo_num_i1[2] = MAX_ALONG_250;
        geo_num_j1[2] = MAX_ACROSS_250;
        if (!(geo_lat1[2] = (double *) calloc(geo_num_i1[2] * geo_num_j1[2], sizeof(double))))
            return SSC_ENOMEM;
        if (!(geo_lon1[2] = (double *) calloc(geo_num_i1[2] * geo_num_j1[2], sizeof(double))))
            return SSC_ENOMEM;
        if (!(geo_index1[2] = (unsigned long long *) calloc(geo_num_i1[2] * geo_num_j1[2],
                                                            sizeof(unsigned long long))))
            return SSC_ENOMEM;

        // Calculate STARE index for each point.
        int m = 0;
        for (int i = 0; i < MAX_ALONG_250; i++) {
            if (i && !(i % 4)) m++;
            int n = 0;
            for (int j = 0; j < MAX_ACROSS_250; j++) {
		int ret;
		double lat_delta, lon_delta;
                if (j && !(j % 4)) n++;
		// if ((ret = interp_lat(latitude, i, j, m, n, &geo_lat1[2][i * MAX_ACROSS_250 + j])))
		//     return ret;
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
		geo_lat1[2][i * MAX_ACROSS_250 + j] = latitude[m * MAX_ACROSS + n] +
		    (j % 4) * lat_delta / 4;
		geo_lon1[2][i * MAX_ACROSS_250 + j] = longitude[m * MAX_ACROSS + n] +
		    (j % 4) * lon_delta / 4;

                // Calculate the stare indices.
                //geo_index1[2][i * MAX_ACROSS_250 + j] = geo_index1[0][m * MAX_ACROSS + n];
		geo_index1[2][i * MAX_ACROSS_250 + j] = index1.ValueFromLatLonDegrees((double)latitude[m * MAX_ACROSS + n],
                 								     (double)longitude[m * MAX_ACROSS + n], level);
            }
        }

	{
	    int m = 0;
	    for (int i = 0; i < 10; i++) {
		if (i && !(i % 4)) m++;
		int n = 0;
		for (int j = 0; j < 10; j++) {
		    if (j && !(j % 4)) n++;
		    // printf("latitude[%d]=%g\n", i * m * MAX_ACROSS + n + j, latitude[m * MAX_ACROSS + n]);
		    // printf("geo_lat1[2][%d]=%g\n", i * MAX_ACROSS_250 + j, geo_lat1[2][i * MAX_ACROSS_250 + j]);
		}
	    }
	}
	
        // for (int i = 0; i < 10; i++) {
        //     for (int j = 0; j < 10; j++) {
	// 	printf("geo_lon1[2][%d]=%g\n", i * MAX_ACROSS_250 + j, geo_lon1[2][i * MAX_ACROSS_250 + j]);
	//     }
	// }


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

