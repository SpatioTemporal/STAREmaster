/// @file

/// This class contains the main functionality for reading a MODIS 09
/// GA file, which will read with HDF4 and the HDF-EOS library. This
/// is a sinusodial grid file.

/// @author Ed Hartnett @date 9/6/20

#include <config.h>
#include <string>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <vector>
#include "ssc.h"
#include "GeoFile.h"
#include "ModisL2GeoFile.h"

#ifndef MODIS09_GA_GEO_FILE_H_
#define MODIS09_GA_GEO_FILE_H_

/**
 * This class reads HDF4 data files.
 */
class Modis09GAGeoFile: public ModisL2GeoFile
{
public:
    bool fileExists(const std::string& name);

    int readFile(const std::string fileName, int verbose, int quiet, int build_level);
};

#endif /* MODIS09_GA_GEO_FILE_H_ */
