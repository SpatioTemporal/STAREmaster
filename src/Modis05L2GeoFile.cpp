/// @file
/// This class handles the MOD05 L2 files from MODIS.
/// @author Ed Hartnett @date 4/4/20

#include "config.h"
#include "Modis05L2GeoFile.h"
#include <mfhdf.h>
#include <hdf.h>
#include <HdfEosDef.h>
#include "STARE.h"
#include <iostream>
#include <sstream>

#ifdef USE_OPENMP
#include <omp.h>
#endif

#define MAX_NAME 256
#define MAX_DIMS 16

#define MAX_ALONG 406
#define MAX_ACROSS 270

/** Construct a Modis05L2GeoFile.
 *
 * @return a Modis05L2GeoFile
 */
Modis05L2GeoFile::Modis05L2GeoFile() {
    cout << "Modis05L2GeoFile constructor\n";

}

/** Destroy a Modis05L2GeoFile.
 *
 */
Modis05L2GeoFile::~Modis05L2GeoFile() {
    cout << "Modis05L2GeoFile destructor\n";
}

/** Read the GRing info.
 *
 * @param fileName name of the file.
 *
 * @return 0 for success, error code otherwise.
 */
int
Modis05L2GeoFile::getGRing(const std::string fileName, int verbose, float *gring_lat, float *gring_lon) {
    char attr_name[SSC_MAX_NAME];
    char *sm_attr;
    string sm;
    string lon_str = "GRINGPOINTLONGITUDE";
    string lat_str = "GRINGPOINTLATITUDE";
    string am0_str = "ArchiveMetadata.0";
    size_t lon_pos, lat_pos;
    int32 num_datasets, num_global_attrs;
    string grlon, grlat;
    int32 sd_id;
    int32 data_type, count;
    int32 am0_idx;
    size_t sz, off = 0;
    const int LON_INC = 126;
    const int LAT_INC = 125;
    const int LON_LEN = 71;
    const int LAT_LEN = 70;
    int ret;

    // Open the HDF4 SD API for this file.
    if ((sd_id = SDstart(fileName.c_str(), DFACC_READ)) == -1)
        return SSC_EHDF4ERR;

    // Learn about this file.
    if (SDfileinfo(sd_id, &num_datasets, &num_global_attrs))
        return SSC_EHDF4ERR;
    if (verbose)
        cout << "num_datasets " << num_datasets << " num_global_attrs " << num_global_attrs << "\n";

    // Find the attribute called ArchiveMetadata.0.
    if ((am0_idx = SDfindattr(sd_id, am0_str.c_str())) == -1)
        return SSC_EHDF4ERR;

    // Learn about the am0 attribute.
    if ((ret = SDattrinfo(sd_id, am0_idx, attr_name, &data_type, &count)) == -1)
        return SSC_EHDF4ERR;
    if (verbose)
        cout << "attribute " << attr_name << " data type " << data_type << " count " << count << "\n";
    if (!(sm_attr = (char *) malloc(count * sizeof(char))))
        return SSC_ENOMEM;
    if ((ret = SDreadattr(sd_id, am0_idx, sm_attr)))
        return SSC_EHDF4ERR;
    sm = sm_attr;

    // Find the positions of the longitude and latitude GRing info in
    // the ArchiveMetadata.0 text string.
    lon_pos = sm.find(lon_str, 0);
    lat_pos = sm.find(lat_str, 0);
    if (verbose)
        cout << "lon_pos " << lon_pos << " lat_pos " << lat_pos << "\n";

    // Pull the longitude GRing values from the ArchiveMetadata.0 text
    // string.
    grlon = sm.substr(lon_pos + LON_INC, LON_LEN);
    if (verbose)
        cout << grlon << "\n";
    for (int i = 0; i < SSC_NUM_GRING; i++) {
        gring_lon[i] = stof(grlon.substr(off), &sz);
        off += sz + 2;
        if (verbose)
            cout << "gring_lon[" << i << "]=" << gring_lon[i] << "\n";
    }

    // Pull the latitude GRing values from the ArchiveMetadata.0 text
    // string.
    grlat = sm.substr(lat_pos + LAT_INC, LAT_LEN);
    if (verbose)
        cout << grlat << "\n";
    off = 0;
    for (int i = 0; i < SSC_NUM_GRING; i++) {
        gring_lat[i] = stof(grlat.substr(off), &sz);
        off += sz + 2;
        if (verbose)
            cout << "gring_lat[" << i << "]=" << gring_lat[i] << "\n";
    }

    // Free resources.
    free(sm_attr);

    // Close the HDF4 SD API for this file.
    if (SDend(sd_id))
        return SSC_EHDF4ERR;

    return 0;
}

/**
 * Read a HDF4 MODIS L2 MOD05 file.
 *
 * @param fileName the data file name.
 * @param verbose non-zero for verbose output to stdout.
 * @param build_level STARE build level.
 * @param cover_level STARE cover level.
 * @param use_gring if true, use g-ring data for cover calculation.
 * @param perimeter_stride perimeter stride.
 *
 * @return 0 for no error, error code otherwise.
 */
int
Modis05L2GeoFile::readFile(const std::string fileName, int verbose,
                           int build_level, int cover_level,
                           bool use_gring, int perimeter_stride) {
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
    float gring_lat[SSC_NUM_GRING], gring_lon[SSC_NUM_GRING];
    int ret;


    // Geolocation data stored in MOD05 is at 5km and may be interpolated to 1km.
    // The same 1km geolocation data can be found in MOD03.
    d_stare_index_name.push_back("5km");
    var_name[0].push_back("Scan_Start_Time");
    var_name[0].push_back("Solar_Zenith");
    var_name[0].push_back("Solar_Azimuth");
    var_name[0].push_back("Water_Vapor_Infrared");
    var_name[0].push_back("Quality_Assurance_Infrared");

    // stare_index_name.push_back("1km");
    // stare_index_name.push_back("500m");
    // stare_index_name.push_back("250m");
    stare_cover_name.push_back("5km");

    if (verbose)
        std::cout << "Reading HDF4 file " << fileName <<
                  " with build level " << build_level << "\n";

    d_num_index = 1;
    if (!(geo_lat1 = (double **)malloc(d_num_index * sizeof(double *))))
        return SSC_ENOMEM;
    if (!(geo_lon1 = (double **)malloc(d_num_index * sizeof(double *))))
        return SSC_ENOMEM;
    if (!(geo_index1 = (unsigned long long **)malloc(d_num_index * sizeof(unsigned long long *))))
        return SSC_ENOMEM;

    num_cover = 1;
    if (!(geo_num_cover_values1 = (int *) malloc(num_cover * sizeof(int))))
        return SSC_ENOMEM;
    if (!(geo_cover1 = (unsigned long long **) malloc(num_cover * sizeof(unsigned long long *))))
        return SSC_ENOMEM;

    // Open the swath file.
    if ((swathfileid = SWopen((char *) fileName.c_str(), DFACC_RDONLY)) < 0)
        return SSC_EHDF4ERR;

    if ((nswath = SWinqswath((char *) fileName.c_str(), swathlist, &strbufsize)) < 0)
        return SSC_EHDF4ERR;
    if (verbose) std::cout << "nswath " << nswath << " " << swathlist << "\n";

    // Attach to a swath.
    string ssc_mod05 = SSC_MOD05;
    if ((swathid = SWattach(swathfileid, (char *) ssc_mod05.c_str())) < 0)
        return SSC_EHDF4ERR;

    // Get lat and lon values.
    string ssc_lon_name = SSC_LON_NAME;
    if (SWreadfield(swathid, (char *) ssc_lon_name.c_str(), NULL, NULL, NULL, longitude))
        return SSC_EHDF4ERR;
    string ssc_lat_name = SSC_LAT_NAME;
    if (SWreadfield(swathid, (char *) ssc_lat_name.c_str(), NULL, NULL, NULL, latitude))
        return SSC_EHDF4ERR;

    geo_num_i.push_back(MAX_ALONG);
    geo_num_j.push_back(MAX_ACROSS);
    if (!(geo_lat1[0] = (double *) calloc(geo_num_i.at(0) * geo_num_j.at(0), sizeof(double))))
        return SSC_ENOMEM;
    if (!(geo_lon1[0] = (double *) calloc(geo_num_i.at(0) * geo_num_j.at(0), sizeof(double))))
        return SSC_ENOMEM;
    if (!(geo_index1[0] = (unsigned long long *) calloc(geo_num_i.at(0) * geo_num_j.at(0),
                                                        sizeof(unsigned long long))))
        return SSC_ENOMEM;

    int level = 27;
    int finest_resolution = 0;
    STARE index(level, build_level);

    // Calculate STARE index for each point.
#pragma omp parallel reduction(max : finest_resolution)
    {
        STARE index1(level, build_level);
#pragma omp for
        for (int i = 0; i < MAX_ALONG; i++) {
            for (int j = 0; j < MAX_ACROSS; j++) {
                geo_lat1[0][i * MAX_ACROSS + j] = latitude[i][j];
                geo_lon1[0][i * MAX_ACROSS + j] = longitude[i][j];

                // Calculate the stare indices.
                geo_index1[0][i * MAX_ACROSS + j] = index1.ValueFromLatLonDegrees((double) latitude[i][j],
                                                                                  (double) longitude[i][j], level);
            }
            index1.adaptSpatialResolutionEstimatesInPlace(&(geo_index1[0][i * MAX_ACROSS]), MAX_ACROSS);

            for (int j = 0; j < MAX_ACROSS; j++) {
                int test_resolution = geo_index1[0][i * MAX_ACROSS + j] & 31; // LevelMask
                if (test_resolution > finest_resolution) {
                    finest_resolution = test_resolution;
                }
            }
        }
    }

    // Now set up and calculate STARE cover
    // int perimeter_stride = 10;
    this->perimeter_stride = perimeter_stride;

    LatLonDegrees64ValueVector perimeter; // Resize below
    int pk; // perimeter counter

    if (!use_gring) { // walk_perimeter, stride > 0...

        pk = 2 * MAX_ALONG + 2 * MAX_ACROSS - 4 - 1;
        perimeter.resize(2 * MAX_ALONG + 2 * MAX_ACROSS - 4); // Walk the perimeter. Note pk below.

        if (verbose) std::cout << "perimeter_stride " << this->perimeter_stride << "\n" << std::flush;

        if (perimeter_stride > 0) { // Maybe redundant if-block

            if (verbose) std::cout << "calculating perimeter" << "\n" << std::flush;

            { // Go along the 'bottom' CCW. Do the full side.
                int i = 0;
                {
                    for (int j = 0; j < MAX_ACROSS; j += perimeter_stride) {
                        perimeter[pk].lat = latitude[i][j];
                        perimeter[pk].lon = longitude[i][j];
                        --pk;
                        if ((perimeter_stride > 1) && (j + perimeter_stride >= MAX_ACROSS)) {
                            perimeter[pk].lat = latitude[i][MAX_ACROSS - 1];
                            perimeter[pk].lon = longitude[i][MAX_ACROSS - 1];
                            --pk;
                        }
                    }
                }
            }

            { // Go up along the right side. Start at 1, because the corner's done.
                int j = MAX_ACROSS - 1;
                for (int i = 1; i < MAX_ALONG; i += perimeter_stride) {
                    {
                        perimeter[pk].lat = latitude[i][j];
                        perimeter[pk].lon = longitude[i][j];
                        --pk;
                        if ((perimeter_stride > 1) && (i + perimeter_stride >= MAX_ALONG)) {
                            perimeter[pk].lat = latitude[MAX_ALONG - 1][j];
                            perimeter[pk].lon = longitude[MAX_ALONG - 1][j];
                            --pk;
                        }
                    }
                }
            }

            { // Go back along the top. Start one over.
                int i = MAX_ALONG - 1;
                {
                    for (int j = MAX_ACROSS - 2; j > -1; j -= perimeter_stride) {
                        perimeter[pk].lat = latitude[i][j];
                        perimeter[pk].lon = longitude[i][j];
                        --pk;
                        if ((perimeter_stride > 1) && (j - perimeter_stride < 0)) {
                            perimeter[pk].lat = latitude[i][0];
                            perimeter[pk].lon = longitude[i][0];
                            --pk;
                        }

                    }
                }
            }

            { // Go back down along the left side. Start one over and don't include the end.
                int j = 0;
                for (int i = MAX_ALONG - 2; i > 0; i -= perimeter_stride) {
                    {
                        perimeter[pk].lat = latitude[i][j];
                        perimeter[pk].lon = longitude[i][j];
                        --pk;
                        if ((perimeter_stride > 1) && (i - perimeter_stride < 0)) {
                            perimeter[pk].lat = latitude[0][j];
                            perimeter[pk].lon = longitude[0][j];
                            --pk;
                        }

                    }
                }
            }

            if (perimeter_stride > 1) {
                // Clean up and resize the vector.
                // Recall that     LatLonDegrees64ValueVector perimeter(2*MAX_ALONG+2*MAX_ACROSS-4).

                // Copy to front of vector.
                int i = 0;
                for (int k = pk + 1; k < 2 * MAX_ALONG + 2 * MAX_ACROSS - 4; ++k) {
                    perimeter[i].lat = perimeter[k].lat;
                    perimeter[i].lon = perimeter[k].lon;
                    ++i;
                }
                perimeter.resize(i);
            }

        }
    }
    else { // Right now, use_gring goes here. If we ever load from a separate file, reconsider.
        //             perimeter_stride <= 0 ) {

        // Get the GRing info. After this call, gring_lat and gring_lon
        // contain the 4 gring values for lat and lon.
        if ((ret = getGRing(fileName, verbose, gring_lat, gring_lon))) {
            cerr << "Error with GRing, maybe retry with --walk_perimeter 1.\n";
            return ret;
        }

        // Note the hardcoded 4 for the 4 corners or the gring.
        perimeter.resize(4); // Use 4 here until we find a granule with more than 4.
        pk = 3;
        for (int i = 0; i < 4; ++i) {
            perimeter[pk].lat = gring_lat[i];
            perimeter[pk].lon = gring_lon[i];
            --pk;
        }
    }

    if (verbose)
        std::cout << "perimeter size = " << perimeter.size() << ", pk = " << pk << "\n" << std::flush;

    if (cover_level == -1)
        this->cover_level = finest_resolution;
    else
        this->cover_level = cover_level;

    if (verbose)
        std::cout << "cover_level = " << this->cover_level << "\n" << std::flush;

    cover = index.NonConvexHull(perimeter, this->cover_level);

    if (verbose) std::cout << "cover size = " << cover.size() << "\n";

    geo_num_cover_values1[0] = cover.size();
    if (!(geo_cover1[0] = (unsigned long long *) calloc(geo_num_cover_values1[0],
                                                        sizeof(unsigned long long))))
        return SSC_ENOMEM;
    for (int k = 0; k < geo_num_cover_values1[0]; ++k)
        geo_cover1[0][k] = cover[k];

    // Learn about dims for this swath.
    if ((ndims = SWinqdims(swathid, dimnames, dimids)) < 0)
        return SSC_EHDF4ERR;
    if (verbose)
        std::cout << "ndims " << ndims << " " << dimnames << "\n";

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
    //            offset, increment))
    //  return SSC_EHDF4ERR;

    // Detach from the swath.
    if (SWdetach(swathid) < 0)
        return SSC_EHDF4ERR;

    // Close the swath file.
    if (SWclose(swathfileid) < 0)
        return SSC_EHDF4ERR;

    return 0;
}
