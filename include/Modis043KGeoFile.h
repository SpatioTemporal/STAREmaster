/// @file

/// This class contains the main functionality for reading a MODIS 09
/// file, which will read with HDF4 and the HDF-EOS library.

/// @author Ed Hartnett @date 8/14/20

#include <config.h>
#include <string>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <vector>
#include "ssc.h"
#include "GeoFile.h"
#include "ModisL2GeoFile.h"

#ifndef MODIS04_3K_GEO_FILE_H_
#define MODIS04_3K_GEO_FILE_H_

/**
 * This class reads HDF4 data files.
 */
class Modis043KGeoFile: public ModisL2GeoFile
{
public:
    int readFile(const std::string fileName, int verbose, int quiet, int build_level);
};

#endif /* MODIS04_3K_GEO_FILE_H_ */
