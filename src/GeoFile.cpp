/// @file
/// This class contains the main functionality for reading a file for
/// which STARE sidecar files may be constructed.

/// @author Ed Hartnett @date 4/4/20

/**
 * @mainpage
 *
 * @section intro Introduction
 *
 * The STAREmaster utility computes STARE indicies for common NASA
 * datasets.
 * 
 * @section refs References
 *
 * 
 * MODIS HDF4 Level 2 Documentation: Ocean Level-2 Data Products,
 * March 22, 2010
 * https://oceancolor.gsfc.nasa.gov/docs/format/Ocean_Level-2_Data_Products.pdf
 *
 * MODIS netCDF Level 2 Documentation: Ocean Level-2 Data Format
 * Specification, https://oceancolor.gsfc.nasa.gov/docs/format/l2nc/
 *
 */
#include "config.h"
#include "GeoFile.h"
#include <mfhdf.h>
#include <hdf.h>
#include <HdfEosDef.h>

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
GeoFile::determineFormat(const std::string fileName, int *gf_format)
{
    int32 swathfileid;
    
    if (gf_format)
	*gf_format = SSC_FORMAT_HDF4;

    // Try to open as swath file with the HDF-EOS library.
    swathfileid = SWopen(fileName.c_str(), DFACC_RDONLY);
    if (swathfileid != -1)
    {
	if (gf_format)
	    *gf_format = SSC_FORMAT_MODIS_L2;	

	// Close the swath file.
	if (SWclose(swathfileid) < 0)
	    return SSC_EHDF4ERR;
    }
    
    return 0;
}

