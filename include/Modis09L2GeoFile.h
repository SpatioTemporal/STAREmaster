/// @file

/// This class contains the main functionality for reading a MODIS 09
/// file, which will read with HDF4 and the HDF-EOS library.

/// @author Ed Hartnett @date 6/1/20

#include <config.h>
#include <string>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <vector>
#include "ssc.h"
#include "GeoFile.h"
#include "ModisL2GeoFile.h"

#ifndef MODIS09_L2_GEO_FILE_H_
#define MODIS09_L2_GEO_FILE_H_

/**
 * This class reads HDF4 data files.
 */
class Modis09L2GeoFile: public ModisL2GeoFile
{
public:
    int readFile(const std::string fileName, int verbose, int quiet, int build_level);
};

#endif /* MODIS09_L2_GEO_FILE_H_ */
