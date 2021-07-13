/// @file

/// This class contains the main functionality for reading a file that
/// may be given a STARE sidecar file.

/// Ed Hartnett 4/4/20

#ifndef MODIS_GEO_FILE_H_ /**< Protect file from double include. */
#define MODIS_GEO_FILE_H_

#include "GeoFile.h"
#include <mfhdf.h>
#include <hdf.h>
#include <HdfEosDef.h>

using namespace std;

/**
 * This is the base class for a Modis data file with geolocation.
 */
class ModisGeoFile : public GeoFile {
public:
    ModisGeoFile();

    ~ModisGeoFile();

    int determineFormat(const std::string fileName, int *gf_format);
};

#endif /* MODIS_GEO_FILE_H_ */
