// This is the main program to create the sidecar files.
//
// Ed Hartnett 3/14/2020

#include "config.h"

#include <getopt.h>

#include <STARE.h>
#include "VarStr.h"

#include "ssc.h"
#include "Modis05L2GeoFile.h"
#include "Modis09L2GeoFile.h"
#include "Modis09GAGeoFile.h"
#include "SidecarFile.h"

using namespace std;

void usage(char *name) {
    cout
        << "STARE spatial create sidecar file. " << endl
        << "Usage: " << name << " [options] filename " << endl
        << "Examples:" << endl
        << "  " << name << " data.nc" << endl
        << "  " << name << " data.h5" << endl
        << endl
        << "Options:" << endl
        << "  " << " -h, --help        : print this help" << endl
        << "  " << " -v, --verbose     : verbose: print all" << endl
        << "  " << " -b, --build_level : Higher levels -> longer initialization time. (default is 5)" << endl
        << "  " << " -c, --cover_level    : Cover resolution, level 10 ~ 10 km." << endl
        << "  " << " -g, --use_gring      : Use GRING data to construct cover (default)" << endl
        << "  " << " -w, --walk_perimeter : Provide stride and walk perimeter to construct cover (more accurate)"
        << endl
        << "  " << " -d, --data_type   : Allows specification of data type." << endl
        << "  " << " -i, --institution : Institution where sidecar file is produced." << endl
        << "  " << " -o, --output_file : Provide file name for output file." << endl
        << "  " << " -r, --output_dir  : Provide output directory name." << endl
        << endl;
    exit(0);
};

struct Arguments {
    bool verbose = false;
    int build_level = SSC_DEFAULT_BUILD_LEVEL;
    int cover_level = -1;
    bool cover_gring = false;
    int stride = -1; // if stride > 0, then we're walking the perimeter and cover_gring = false.
    char data_type[SSC_MAX_NAME] = "";
    char institution[SSC_MAX_NAME] = "";
    char output_file[SSC_MAX_NAME] = "";
    char output_dir[SSC_MAX_NAME] = "";
    int err_code = 0;
};

Arguments parseArguments(int argc, char *argv[]) {
    if (argc == 1) usage(argv[0]);
    Arguments arguments;
    static struct option long_options[] = {
            {"help",             no_argument,       0, 'h'},
            {"verbose",          no_argument,       0, 'v'},
            {"build_level",      required_argument, 0, 'b'},
            {"cover_level",      required_argument, 0, 'c'},
            {"use_gring",        no_argument,       0, 'g'},
            {"walk_perimeter",   required_argument, 0, 'w'},
            {"data_type",        required_argument, 0, 'd'},
            {"institution",      required_argument, 0, 'i'},
            {"output_file",      required_argument, 0, 'o'},
            {"output_directory", required_argument, 0, 'r'},
            {0,                  0,                 0, 0}
    };

    int long_index = 0;
    int opt = 0;
    while ((opt = getopt_long(argc, argv, "hvqb:c:gw:d:o:r:i:", long_options, &long_index)) != -1) {
        switch (opt) {
            case 'h':
                usage(argv[0]);
            case 'v':
                arguments.verbose = true;
                break;
            case 'b':
                arguments.build_level = atoi(optarg);
                break;
            case 'c':
                arguments.cover_level = atoi(optarg);
                break;
            case 'g':
                arguments.cover_gring = true;
                break;
            case 'w':
                arguments.stride = atoi(optarg);
                break;
            case 'd':
                strcpy(arguments.data_type, optarg);
                break;
            case 'i':
                strcpy(arguments.institution, optarg);
                break;
            case 'o':
                strcpy(arguments.output_file, optarg);
                break;
            case 'r':
                strcpy(arguments.output_dir, optarg);
                break;
        }
    }

    // Check for argument consistency.
    if (!arguments.cover_gring) {
        if (arguments.stride <= 0) {
            arguments.cover_gring = true;
        }
    }
    else {
        if (arguments.stride > 0) { // error case, both gring and walk are set
            cerr << "Incompatible arguments. Both perimeter walk (-w) and gring (-g) STARE covers requested.\n";
            arguments.err_code = 99;
        }
    }

    return arguments;
};

/** Pick an output filename for the STARE index file, including output
 * directory.
 *
 * @param file_in_char Name of the data file.
 * @param output_dir_char If present, the name of the output directory
 * for the STARE index sidecar file.
 * @return The path and name of the sidecar file.
 */
string
pickOutputName(const char *file_in_char, char *output_dir_char) {
    string file_out(file_in_char);
    string output_dir(output_dir_char);

    // Do we want this in a different directory?
    if (strlen(output_dir_char)) {
        size_t f = file_out.rfind("/");
        if (f != string::npos)
            file_out = output_dir + file_out.substr(f, string::npos);
        else
            file_out = output_dir + file_out;
    }

    return file_out;
}

int
main(int argc, char *argv[]) {
    Arguments arg = parseArguments(argc, argv);
    GeoFile *gf;
    SidecarFile sf;
    string file_out;
    const string MOD09 = "MOD09";
    const string MOD09GA = "MOD09GA";
    const string SIN_TABLE = "sn_bound_10deg.txt";

    // Input file must be provided.
    if (!argv[optind]) {
        cerr << "Must provide input file.\n";
        return SSC_EINPUT;
    }

    if (arg.err_code) {
        return arg.err_code;
    }

    if (arg.data_type == MOD09) {
        gf = new Modis09L2GeoFile();
        if (((Modis09L2GeoFile *) gf)->readFile(argv[optind], arg.verbose, arg.build_level,
						arg.cover_level, arg.cover_gring, arg.stride)) {
            cerr << "Error reading MOD09 L2 file.\n";
            return 99;
        }
    }
    else if (arg.data_type == MOD09GA) {
        gf = new Modis09GAGeoFile();
        if (((Modis09GAGeoFile *) gf)->readFile(argv[optind], arg.verbose, arg.build_level)) {
            cerr << "Error reading MOD09GA file.\n";
            return 99;
        }
    }
    else {
        gf = new Modis05L2GeoFile();
        if (((Modis05L2GeoFile *) gf)->readFile(argv[optind], arg.verbose, arg.build_level,
                                                arg.cover_level, arg.cover_gring, arg.stride)) {
            cerr << "Error reading MOD05 file.\n";
            return 99;
        }
    }

    // Determine the output filename.
    if (strlen(arg.output_file))
        file_out = arg.output_file;
    else
        file_out = pickOutputName(gf->sidecar_filename(argv[optind]).c_str(), arg.output_dir);

    // Create the sidecar file.
    sf.createFile(file_out, arg.verbose, arg.institution);

    // Write the sidecar file.
    for (int i = 0; i < gf->d_num_index; i++)
    {
	double *lats = &gf->geo_lat[i][0];
	double *lons = &gf->geo_lon[i][0];
	unsigned long long int *geo_idx = &gf->geo_index[i][0];
	if (sf.writeSTAREIndex(arg.verbose, arg.build_level, gf->geo_num_i[i],
                               gf->geo_num_j[i], lats, lons, geo_idx,
                               gf->var_name[i], gf->d_stare_index_name[i])) {
            cerr << "Error writing STARE index.\n";
            return 99;
        }
    }

    if (arg.verbose)
	std::cout << "writing covers" << std::endl;
    for (int i = 0; i < gf->num_cover; i++) {
	if (arg.verbose)
	    std::cout << "writing cover i = " << i << ", name = " <<
		gf->stare_cover_name.at(i) << std::endl;
        if (sf.writeSTARECover(arg.verbose, gf->geo_num_cover_values[i], &gf->geo_cover[i][0],
                               gf->stare_cover_name.at(i))) {
            cerr << "Error writing STARE cover.\n";
            return 99;
        }
    }

    // Close the sidecar file.
    sf.close_file();

    delete gf;
    return 0;
};

