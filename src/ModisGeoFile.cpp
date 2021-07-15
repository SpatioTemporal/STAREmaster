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




