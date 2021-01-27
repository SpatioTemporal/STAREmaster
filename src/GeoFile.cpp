/// @file
/// This class contains the main functionality for reading a file for
/// which STARE sidecar files may be constructed.

// Ed Hartnett 4/4/20

/**
 * @mainpage
 *
 * @section intro Introduction
 *
 * The STAREmaster utility computes STARE indicies for common NASA
 * datasets.
 *
 * STAREmaster consists of:
 * - a command line utility to create STARE sidecar files
 * - a C++ library to read geo-location information from selected data sets.
 *
 * STAREmaster also uses the STARE library, which computes the
 * spacio-temporal indicies for the geo-location data that STAREmaster
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
#include "NetcdfSidecarFile.h"
#include <netcdf.h>
#include <mfhdf.h>
#include <hdf.h>
#include <HdfEosDef.h>

/** Construct a GeoFile.
 *
 * @return a GeoFile
 */
GeoFile::GeoFile()
{
    cout<<"GeoFile constructor\n";

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
GeoFile::~GeoFile()
{
    cout<<"GeoFile destructor\n";

    // Free any allocated memory.
    if (geo_lat1)
    {
        for (int i = 0; i < num_index; i++)
            if (geo_lat1[i])
                free(geo_lat1[i]);
	free(geo_lat1);
    }

    if (geo_lon1)
    {
        for (int i = 0; i < num_index; i++)
            if (geo_lon1[i])
                free(geo_lon1[i]);
	free(geo_lon1);
    }
    
    if (geo_index1)
    {
        for (int i = 0; i < num_index; i++)
            if (geo_index1[i])
                free(geo_index1[i]);
	free(geo_index1);
    }
        
    if (geo_num_i1)
	free(geo_num_i1);
    if (geo_num_j1)
	free(geo_num_j1);

    for (int i = 0; i < num_cover; i++)
    {
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
GeoFile::determineFormat(const std::string fileName, int *gf_format)
{
    int32 swathfileid;
    
    if (gf_format)
	*gf_format = SSC_FORMAT_HDF4;

    // Try to open as swath file with the HDF-EOS library.
    swathfileid = SWopen((char *)fileName.c_str(), DFACC_RDONLY);
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

string
GeoFile::sidecarFileName(const string fileName)
{
    string sidecarFileName;
    
    // Is there a file extension?
    size_t f = fileName.rfind(".");
    if (f != string::npos)
	sidecarFileName = fileName.substr(0, f) + "_stare.nc";
    else
    {
        sidecarFileName = fileName;
	sidecarFileName.append("_stare.nc");
    }

    return sidecarFileName;
}

/**
 * Read a sidecare file.
 *
 * @param fileName Name of the sidecar file.
 * @param verbose Set to non-zero to enable verbose output for
 * debugging.
 * @return 0 for success, error code otherwise.
 */
int
GeoFile::readSidecarFile(const std::string fileName, int verbose, int &ncid)
{
    NetcdfSidecarFile sf;
    vector<size_t> size_i, size_j;
    int ret;
    
    if ((ret = sf.readSidecarFile(fileName, verbose, num_index, stare_index_name, size_i, size_j, variables, ncid)))
        return ret;
    return 0;
}

/**
 * Get STARE index for data varaible.
 *
 * @param ncid ID of the sidecar file.
 * @param verbose Set to non-zero to enable verbose output for
 * debugging.
 * @param varid A reference that gets the varid of the STARE index.
 * @return 0 for success, error code otherwise.
 */
int
GeoFile::getSTAREIndex(const std::string varName, int verbose, int ncid, int &varid)
{
    for (string v : variables)
    {
	cout << v << endl;
	if (v.find(varName) != string::npos) {
	    //.. found.
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
GeoFile::closeSidecarFile(int verbose, int ncid)
{
    int ret;
    
    if (verbose) std::cout << "Closing sidecar file with ncid " << ncid << "\n";
    if ((ret = nc_close(ncid)))
        return ret;
    
    return 0;
}


