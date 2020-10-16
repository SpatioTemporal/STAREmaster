// This is the main program to create the sidecar files.
//
// Ed Hartnett 3/14/2020

#include "config.h"
#include "STARE.h"
#include "VarStr.h"
#include <getopt.h>
#include "ssc.h"
#include "ModisL2GeoFile.h"
#include "Modis09L2GeoFile.h"
#include "Modis09GAGeoFile.h"
#include "NetcdfSidecarFile.h"

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
        << "  " << " -q, --quiet       : don't chat, just give back index" << endl
        << "  " << " -b, --build_level : Higher levels -> longer initialization time. (default is 5)" << endl
        << "  " << " -c, --cover_level : Cover resolution, level 10 ~ 10 km." << endl
        << "  " << " -s, --stride      : Perimeter stride" << endl
        << "  " << " -d, --data_type   : Allows specification of data type." << endl
        << "  " << " -i, --institution : Institution where sidecar file is produced." << endl
        << "  " << " -o, --output_file : Provide file name for output file." << endl
        << "  " << " -r, --output_dir  : Provide output directory name." << endl
        << endl;
    exit(0);
};

struct Arguments {
  bool verbose = false;
  bool quiet = false;
  int build_level = SSC_DEFAULT_BUILD_LEVEL;
  int cover_level = -1;
  int stride = 1;
  char data_type[SSC_MAX_NAME] = "";
  char institution[SSC_MAX_NAME] = "";
  char output_file[SSC_MAX_NAME] = "";
  char output_dir[SSC_MAX_NAME] = "";
};

Arguments parseArguments(int argc, char *argv[]) {
    if (argc == 1) usage(argv[0]);
    Arguments arguments;
    static struct option long_options[] = {
        {"help", no_argument, 0, 'h'},
        {"verbose", no_argument, 0, 'v'},
        {"quiet", no_argument, 0, 'q'},
        {"build_level", required_argument, 0, 'b'},
	{"cover_level", required_argument,0,'c'},
	{"stride", required_argument,0,'s'},
        {"data_type", required_argument, 0, 'd'},
	{"institution", required_argument, 0, 'i'},
        {"output_file", required_argument, 0, 'o'},
        {"output_directory", required_argument, 0, 'r'},
        {0, 0, 0, 0}
    };

    int long_index = 0;
    int opt = 0;
    while ((opt = getopt_long(argc, argv, "hvqb:c:s:d:o:r:i:", long_options, &long_index)) != -1) {
        switch (opt) {
        case 'h': usage(argv[0]);
        case 'v': arguments.verbose = true; break;
        case 'q': arguments.quiet = true; break;
        case 'b': arguments.build_level = atoi(optarg); break;
	case 'c': arguments.cover_level = atoi(optarg); break;
	case 's': arguments.stride = atoi(optarg); break;
        case 'd': strcpy(arguments.data_type, optarg); break;
        case 'i': strcpy(arguments.institution, optarg); break;
        case 'o': strcpy(arguments.output_file, optarg); break;
        case 'r': strcpy(arguments.output_dir, optarg); break;
        }
    }
    return arguments;
};

string
pickOutputName(char *file_in_char, char *output_dir_char)
{
    string file_in(file_in_char);
    string file_out(file_in_char);
    string output_dir(output_dir_char);

    // Is there a file extension?
    size_t f = file_in.rfind(".");
    if (f != string::npos)
	file_out = file_in.substr(0, f) + "_stare.nc";
    else
	file_out.append("_stare.nc");

    // Do we want this in a different directory?
    if (strlen(output_dir_char))
    {
	size_t f = file_out.rfind("/");
	if (f != string::npos)
	    file_out = output_dir + file_out.substr(f, string::npos);
	else
	    file_out = output_dir + file_out;
    }
    
    return file_out;
}

int
main(int argc, char *argv[])
{
    Arguments arg = parseArguments(argc, argv);
    ModisL2GeoFile *gf;
    NetcdfSidecarFile sf;
    string file_out;
    const string MOD09 = "MOD09";
    const string MOD09GA = "MOD09GA";
    const string SIN_TABLE = "sn_bound_10deg.txt";


    // Input file must be provided.
    if (!argv[optind])
    {
	cerr<<"Must provide input file.\n";
	return SSC_EINPUT;
    }

    // Determine the output filename.
    if (strlen(arg.output_file))
	file_out = arg.output_file;
    else
	file_out = pickOutputName(argv[optind], arg.output_dir);

    if (arg.data_type == MOD09)
    {
	gf = new Modis09L2GeoFile();
	if (((Modis09L2GeoFile *)gf)->readFile(argv[optind], arg.verbose, arg.quiet, arg.build_level))
	{
	    cerr<<"Error reading MOD09 L2 file.\n";
	    return 99;
	}
    }
    else if (arg.data_type == MOD09GA)
    {
	gf = new Modis09GAGeoFile();
	if (((Modis09GAGeoFile *)gf)->readFile(argv[optind], arg.verbose, arg.quiet, arg.build_level))
	{
	    cerr<<"Error reading MOD09GA file.\n";	    
	    return 99;
	}
    }
    else
    {
	gf = new ModisL2GeoFile();
	if (gf->readFile(argv[optind], arg.verbose, arg.quiet, arg.build_level, arg.cover_level, arg.stride))
	{
	    cerr<<"Error reading MOD05 file.\n";
	    return 99;
	}
    }

    // Create the sidecar file.
    sf.createFile(file_out, arg.verbose, arg.institution);
    
    // Write the sidecar file.
    for (int i = 0; i < gf->num_index; i++)
    {
	if (sf.writeSTAREIndex(arg.verbose, arg.quiet, arg.build_level, gf->geo_num_i1[i], gf->geo_num_j1[i],
			       gf->geo_lat1[i], gf->geo_lon1[i], gf->geo_index1[i], gf->var_name[i], gf->stare_index_name.at(i)))
	{
	    cerr<<"Error writing STARE index.\n";
	    return 99;
	}
    }

    std::cout << "writing covers" << std::endl;
    for (int i = 0; i < gf->num_cover; i++) {
	std::cout << "writing cover i = " << i << ", name = " << gf->stare_cover_name.at(i)  << std::endl;
	if (sf.writeSTARECover(arg.verbose, arg.quiet,
			       gf->geo_num_cover_values1[i], gf->geo_cover1[i],
			       gf->stare_cover_name.at(i)))
	{
	    cerr<<"Error writing STARE cover.\n";
	    return 99;
	}
    }

    // Close the sidecar file.
    sf.closeFile();


    delete gf;
    return 0;
};

