/// @file

/// This class contains the main functionality for reading a MODIS L2
/// file, which will read with HDF4 and the HDF-EOS library.

/// @author Ed Hartnett @date 4/4/20

#include <config.h>
#include <string>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <vector>
#include "ssc.h"
#include "GeoFile.h"

#ifndef MODIS_L2_GEO_FILE_H_
#define MODIS_L2_GEO_FILE_H_

/**
 * This class reads HDF4 data files.
 */
class ModisL2GeoFile : public GeoFile
{
public:
    ModisL2GeoFile();
    ~ModisL2GeoFile();
    
    int readFile(const std::string fileName, int verbose, int quiet, int build_level);
    int getGRing(const std::string fileName, int verbose, float *gring_lat, float *gring_lon);

};

#endif /* MODIS_L2_GEO_FILE_H_ */
