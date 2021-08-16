/// @file
/// THis is the root class for the STAREmaster functionality. Derived
/// classes read different format input files and create sidecar files
/// for them.
/// 
/// Ed Hartnett 2/21/21

#include "config.h"

#include <netcdf.h>

#include "ModisGeoFile.h"
#include "SidecarFile.h"

/** Construct a ModisGeoFile.
 *
 * @return a ModisGeoFile
 */
ModisGeoFile::ModisGeoFile() {
    // cout << "ModisGeoFile constructor" << endl;
}

/** Destroy a ModisGeoFile.
 *
 */
ModisGeoFile::~ModisGeoFile() {
    // cout << "ModisGeoFile destructor\n";
}

/**
 * Determine the format of the target file.
 *
 * @param fileName Name of the target file.
 * @param gf_format Pointer to int that gets a constant indicating the
 * format. Ignored if NULL.
 *
 * @return 0 for success, error code otherwise.
 */
int
ModisGeoFile::determineFormat(const std::string fileName, int *gf_format) {
    int32 swathfileid;

    if (gf_format)
        *gf_format = SSC_FORMAT_HDF4;

    // Try to open as swath file with the HDF-EOS library.
    swathfileid = SWopen((char *) fileName.c_str(), DFACC_RDONLY);
    if (swathfileid != -1) {
        if (gf_format)
            *gf_format = SSC_FORMAT_MODIS_L2;

        // Close the swath file.
        if (SWclose(swathfileid) < 0)
            return SSC_EHDF4ERR;
    }

    return 0;
}

/** Read the GRing info.
 *
 * @param fileName name of the file.
 *
 * @return 0 for success, error code otherwise.
 */
int
ModisGeoFile::getGRing(const std::string fileName, int verbose, float *gring_lat, float *gring_lon) {
    char attr_name[SSC_MAX_NAME];
    char *sm_attr;
    string sm;
    string lon_str = "GRINGPOINTLONGITUDE";
    string lat_str = "GRINGPOINTLATITUDE";
    int32 num_datasets, num_global_attrs;
    string grlon, grlat;
    int32 sd_id;
    int32 data_type, count;
    int32 am0_idx;
    const int LON_INC = 126;
    const int LAT_INC = 125;
    const int LON_LEN = 71;
    const int LAT_LEN = 70;
    int ret;
    verbose=1;

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
    size_t lon_pos, lat_pos;
    size_t end_lon_pos, end_lat_pos;
    string close_paren_str = ")";
    string open_paren_str = "(";

    // Find GRINGPOINTLONGITUDE section.
    lon_pos = sm.find(lon_str, 0);
    // Find start of longitude list.
    lon_pos = sm.find(open_paren_str, lon_pos) + 1;
    end_lon_pos = sm.find(close_paren_str, lon_pos);
    if (verbose)
        cout << "lon_pos " << lon_pos << " end_lon_pos " << end_lon_pos << " lat_pos " << lat_pos <<
	    " end_lat_pos " << end_lat_pos << "\n";

    // Pull the longitude GRing values from the ArchiveMetadata.0 text
    // string.
    grlon = sm.substr(lon_pos, end_lon_pos - lon_pos - 1);
    if (verbose)
        cout << "grlon " << grlon << "\n";

    // Get four float values from the text string.
    size_t sz, off = 0;
    for (int i = 0; i < SSC_NUM_GRING; i++) {
        gring_lon[i] = stof(grlon.substr(off), &sz);
        off += sz + 2;
        if (verbose)
            cout << "gring_lon[" << i << "]=" << gring_lon[i] << "\n";
    }

    // Pull the latitude GRing values from the ArchiveMetadata.0 text
    // string.
    lat_pos = sm.find(lat_str, 0);
    cout << "sm.substr(lat_pos, 256) " << sm.substr(lat_pos, 256) << "\n";
    lat_pos = sm.find(open_paren_str, lat_pos) + 1;
    cout << "sm.substr(lat_pos, 256) " << sm.substr(lat_pos, 256) << "\n";
    end_lat_pos = sm.find(close_paren_str, lat_pos);
    grlat = sm.substr(lat_pos, end_lat_pos - lat_pos - 1);
    if (verbose)
        cout << "grlat " << grlat << "\n";
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

