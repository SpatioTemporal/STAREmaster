/// @file
/// This class contains the main functionality for reading a file for
/// which STARE sidecar files may be constructed.

// Ed Hartnett 4/4/20

/**
 * @mainpage
 *
 * @section intro Introduction
 *
 * The STAREmaster utility computes STARE indices for common NASA
 * datasets.
 *
 * STAREmaster consists of:
 * - a command line utility to create STARE sidecar files
 * - a C++ library to read geo-location information from selected data sets.
 *
 * STAREmaster also uses the STARE library, which computes the
 * spacio-temporal indices for the geo-location data that STAREmaster
 * reads from the data file.
 * 
 * @section createSidecarFile Command Line Tool createSidecarFile
 *
 * The command line tool creates a sidecar file for a scienfic data
 * set.
 *
 * Use the command line tool like this:
 *
 * createSidecarFile -d dataset_code data_file
 *
 * The full set of options is listed with --help:
 * <pre>
STARE spatial create sidecar file. 
Usage: ./createSidecarFile [options] [filename]} 
Examples:
  ./createSidecarFile data.nc
  ./createSidecarFile data.h5

Options:
   -h, --help        : print this help
   -v, --verbose     : verbose: print all
   -q, --quiet       : don't chat, just give back index
   -b, --build_level : Higher levels -> longer initialization time. (default is 5)
   -d, --data_type   : Allows specification of data type.
   -i, --institution : Institution where sidecar file is produced.
   -o, --output_file : Provide file name for output file.
   -r, --output_dir  : Provide output directory name.
</pre>
 *
 * @section STAREmasterLibrary STAREmaster Library
 *
 * The STAREmaster library is a C++ library which can read targeted
 * data sets and determine their geo-location information.
 *
 * Each data set which is understood by the STAREmaster library has a class file with full documentation.
 *
 * Data Set | Library Class
 * ---------|--------------
 * MOD05    | ModisL2GeoFile
 * MOD09    | Modis09L2GeoFile
 * 
 * @section refs References
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
#include "SidecarFile.h"
#include <netcdf.h>
#include <mfhdf.h>
#include <hdf.h>
#include <HdfEosDef.h>

#undef DEBUG

/** Construct a GeoFile.
 *
 * @return a GeoFile
 */
GeoFile::GeoFile() {
#if DEBUG
    cout << "GeoFile constructor\n";
#endif

    // Initialize values.
    num_index = 0;
    geo_num_i1 = NULL;
    geo_num_j1 = NULL;
    geo_cover1 = NULL;
    geo_num_cover_values1 = NULL;
    geo_lat1 = NULL;
    geo_lon1 = NULL;
    geo_index1 = NULL;
}

/** Destroy a GeoFile.
 *
 */
GeoFile::~GeoFile() {
#if DEBUG
    cout << "GeoFile destructor\n";
#endif

    // Free any allocated memory.
    if (geo_lat1) {
        for (int i = 0; i < num_index; i++)
            if (geo_lat1[i])
                free(geo_lat1[i]);
        free(geo_lat1);
    }

    if (geo_lon1) {
        for (int i = 0; i < num_index; i++)
            if (geo_lon1[i])
                free(geo_lon1[i]);
        free(geo_lon1);
    }

    if (geo_index1) {
        for (int i = 0; i < num_index; i++)
            if (geo_index1[i])
                free(geo_index1[i]);
        free(geo_index1);
    }

    if (geo_num_i1)
        free(geo_num_i1);
    if (geo_num_j1)
        free(geo_num_j1);

    for (int i = 0; i < num_cover; i++) {
        if (geo_cover1)
            free(geo_cover1[i]);
        // if (cover1)
        //     free(cover1[i]);
    }

    if (geo_cover1)
        free(geo_cover1);
    if (geo_num_cover_values1)
        free(geo_num_cover_values1);
    //    if (cover1)
    //	free(cover1);
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
GeoFile::determineFormat(const std::string fileName, int *gf_format) {
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

/**
 * Return the name of the STARE sidecar file for the given main file.
 * @param fileName The name of the main data file
 * @return The name of the companion STARE index file
 */
std::string
GeoFile::sidecarFileName(const std::string fileName) {
    std::string sidecarFileName;

    // Is there a file extension?
    size_t f = fileName.rfind(".");
    if (f != std::string::npos)
        sidecarFileName = fileName.substr(0, f) + "_stare.nc";
    else {
        sidecarFileName = fileName;
        sidecarFileName.append("_stare.nc");
    }

    return sidecarFileName;
}

/**
 * Read a sidecar file.
 *
 * Sets internal state values for this sidecar file including the mapping
 * between sets of indices and different variables in the main data file.
 *
 * @param fileName Name of the sidecar file.
 * @param verbose Set to non-zero to enable verbose output for debugging.
 * @param ncid Value-result parameter used to return the sidecar file 'ncid.'
 * @return 0 for success, error code otherwise.
 */
int
GeoFile::readSidecarFile(const std::string fileName, int verbose, int &ncid) {
    SidecarFile sf;
    int ret;

    if ((ret = sf.readSidecarFile(fileName, verbose, num_index, stare_index_name,
                                  size_i, size_j, variables, stare_varid, ncid)))
        return ret;

    return 0;
}

/**
 * Get STARE index for data variable.
 *
 * @param ncid ID of the sidecar file.
 * @param verbose Set to non-zero to enable verbose output for
 * debugging.
 * @param varid A reference that gets the varid of the STARE index.
 * @param my_size_i A reference that gets the size of the X-axis (longitude)
 * for the array of STARE indices.
 * @param my_size_j A reference that gets the size of the Y-axis (latitude)
 * for the array of STARE indices.
 * @return 0 for success, error code otherwise.
 */
int
GeoFile::getSTAREIndex(const std::string varName, int verbose, int ncid, int &varid,
                       size_t &my_size_i, size_t &my_size_j) {
    // Check all of our STARE indexes.
    for (int v = 0; v < variables.size(); v++) {
        string vars = variables.at(v);
        if (verbose) std::cout << vars << endl;

        // Is the desired variable listed in the vars string?
        if (vars.find(varName) != string::npos) {
            if (verbose) std::cout << "found!" << endl;
            varid = stare_varid.at(v);
            my_size_i = size_i.at(v);
            my_size_j = size_j.at(v);
        }
    }
    return 0;
}

/**
 * Close sidecar file.
 *
 * @param ncid ID of the sidecar file.
 * @param verbose Set to non-zero to enable verbose output for
 * debugging.
 * @return 0 for success, error code otherwise.
 */
int
GeoFile::closeSidecarFile(int verbose, int ncid) {
    int ret;

    if (verbose) std::cout << "Closing sidecar file with ncid " << ncid << "\n";
    if ((ret = nc_close(ncid)))
        return ret;

    return 0;
}


