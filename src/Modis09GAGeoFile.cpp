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


/** Does a file exist? */
bool
Modis09GAGeoFile::fileExists(const std::string& name)
{
    if (FILE *file = fopen(name.c_str(), "r")) {
        fclose(file);
        return true;
    } else {
        return false;
    }   
}

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

    string dir_name, base_name;
    string h_str, v_str;
    const int FILE_NAME_LEN = 45;
    const int H_POS = 18;
    const int V_POS = 21;
    const int LEN = 2;
    const string FILE_PREFIX = "MOD09GA_";
    const string TABLE_NAME = "sn_bound_10deg.txt";
    string fileOut;
    int h, v;

    stare_index_name.push_back("1km");
    stare_index_name.push_back("500m");
    stare_index_name.push_back("250m");
    stare_cover_name.push_back("1km");

    // Create the output file name and find the h and v tile numbers.
    dir_name = fileName.substr(0, fileName.rfind("/") + 1);
    cout << "dir_name " << dir_name << "\n";
    base_name = fileName.substr(fileName.rfind("/") + 1, FILE_NAME_LEN);
    cout << "base_name " << base_name << "\n";
    h_str = base_name.substr(H_POS, LEN);
    v_str = base_name.substr(V_POS, LEN);
    cout << "h " << h_str << " v " << v_str << "\n";
    h = stoi(h_str);
    v = stoi(v_str);
    cout << "h " << h << " v " << v << "\n";
    fileOut = dir_name + FILE_PREFIX + "h" + h_str + "v" + v_str + "_stare.nc";
    cout << "fileOut = " << fileOut << "\n";

    // If this file already exists, assume we are done. ;-)
    if (fileExists(fileOut))
    {
	cout << fileOut << " exists\n";
	return 0;
    }

    // Open the table file and get the lat/lons for this v/h.
    cout << "creating " << fileOut << "\n";
    fstream newfile;
    const int SKIP_LINES = 7;
    
    newfile.open(TABLE_NAME, ios::in);
    if (newfile.is_open()) { 
	string tp;
	int c = 0;
	while (getline(newfile, tp)) {
	    if (c++ <= SKIP_LINES)
		continue;
	    cout << tp << "\n";
	}
	newfile.close();
    }

    // Calculate the rest of the lat/lons.

    // Calculate the STARE indexes.

    
    return 0;
}

