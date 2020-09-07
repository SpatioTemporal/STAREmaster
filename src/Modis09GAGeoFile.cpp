// This class is handling MODIS MOD09GA files. These are the sinusodial grid files.

// Ed Hartnett 9/6/20

#include "config.h"
#include "Modis09GAGeoFile.h"
#include <mfhdf.h>
#include <hdf.h>
#include <vector>
#include <HdfEosDef.h>
#include "STARE.h"
using namespace std;

#define MAX_NAME 256
#define MAX_DIMS 16

#define MAX_ALONG 2030
#define MAX_ACROSS 1354
#define MAX_ALONG_500 4060
#define MAX_ACROSS_500 2708
#define MAX_ALONG_250 (MAX_ALONG_500 * 2)
#define MAX_ACROSS_250 (MAX_ACROSS_500 * 2)

/**
 * Read a HDF4 MODIS MOD09 GA file.
 *
 * @param fileName the data file name.
 * @param verbose non-zero for verbose output to stdout.
 * @param quiet non-zero for no output.
 * @param build_level STARE build level.
 *
 * @return 0 for no error, error code otherwise.
 */
int
Modis09GAGeoFile::readFile(const std::string fileName, int verbose, int quiet,
			   int build_level)

{
    
    if (verbose) std::cout << "Reading HDF4 file " << fileName <<
		     " with build level " << build_level << "\n";

    string base_name;
    string h_str, v_str;
    const int FILE_NAME_LEN = 45;
    const int H_POS = 18;
    const int V_POS = 21;
    const int LEN = 2;
    int h, v;

    base_name = fileName.substr(fileName.rfind("/") + 1, FILE_NAME_LEN);
    cout << "base_name " << base_name << "\n";
    h_str = base_name.substr(H_POS, LEN);
    v_str = base_name.substr(V_POS, LEN);
    cout << "h " << h_str << " v " << v_str << "\n";
    h = stoi(h_str);
    v = stoi(v_str);
    cout << "h " << h << " v " << v << "\n";
    return 0;
}

