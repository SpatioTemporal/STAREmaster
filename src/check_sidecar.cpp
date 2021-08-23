//
// Created by Ed Hartnett on 8/21/21.
//
// Utility to check correctness of a STARE sidecar file.

#include "config.h"

#include <getopt.h>
#include <iostream>
#include <sstream>
#include <bitset>
#include "netcdf.h"
#include "ssc.h"
#include "SidecarFile.h"
#include <string.h>

using namespace std;

#define NDIM4 4

void usage(char *name) {
    cout
    << "STARE sidecare check utility. " << endl
    << "Usage: " << name << " [options] sidecar_file " << endl
    << "Examples:" << endl
    << "  " << name << " MOD05_L2.A2021232.1600.061.2021233022815_stare.nc" << endl
    << endl
    << "Options:" << endl
    << " -h, --help        : print this help" << endl
    << " -v, --verbose     : verbose: print all" << endl;

    exit(0);
};

struct Arguments {
    bool verbose = false;
    int err_code = 0;
};

Arguments parseArguments(int argc, char *argv[]) {
    if (argc == 1) usage(argv[0]);
    Arguments arguments;
    static struct option long_options[] = {
            {"help",             no_argument,       nullptr, 'h'},
            {"verbose",          no_argument,       nullptr, 'v'},
            {nullptr,           0,         nullptr, 0}
    };

    int long_index = 0;
    int opt;
    while ((opt = getopt_long(argc, argv, "hvf:sb", long_options, &long_index)) != -1) {
        switch (opt) {
            case 'h':
                usage(argv[0]);
            case 'v':
                arguments.verbose = true;
                break;
            default:
                usage(argv[0]);
        }
    }

    return arguments;
};

int main(int argc, char *argv[])
{
    Arguments arg = parseArguments(argc, argv);

    // Input STARE sidecar file must be provided.
    if (!argv[optind]) {
        cerr << "Must provide STARE sidecar filen name." << endl;
        return 1;
    }

    if (arg.err_code) {
        return arg.err_code;
    }

    // Read the sidecar file.
    {
	SidecarFile sf;
	int ncid;
	int num_index;
	vector <string> stare_index_name, variables;
	vector <size_t> size_i, size_j;
	vector<int> stare_varid;
	int ret;

	if ((ret = sf.read_sidecar_file(argv[optind], arg.verbose, num_index, stare_index_name, size_i,
					size_j, variables, stare_varid, ncid)))
	    return ret;
    }

    {
	int ncid;
	int ret;
   
	// Open sidecar file with netCDF.
	if (arg.verbose)
	    cout << "Opening sidecar file " << argv[optind] << "\n";
	if ((ret = nc_open(argv[optind], NC_NOWRITE, &ncid)))
	    return ret;

	int ndims, nvars, natts, unlimdimid;
	if ((ret = nc_inq(ncid, &ndims, &nvars, &natts, &unlimdimid)))
	    return ret;
	if (arg.verbose)
	    cout << "ndims " << ndims << " nvars " << nvars << " natts " <<
		natts << " unlimdimid " << unlimdimid << "\n";

	// Learn about variables.
	for (int v = 0; v < nvars; v++)
	{
	    char var_name[NC_MAX_NAME + 1];
	    int xtype, num_var_dims, var_dimids[NDIM4], var_natts;

	    if ((ret = nc_inq_var(ncid, v, var_name, &xtype, &num_var_dims, var_dimids, &var_natts)))
		return ret;
	    if (arg.verbose)
		cout << "var_name " << var_name << " xtype " << xtype << " num_var_dims " <<
		    num_var_dims << " var_natts " << var_natts << "\n";

	    // Every var has an attribute named "long_name".
	    char long_name[SSC_MAX_NAME];
	    if ((ret = nc_get_att_text(ncid, v, SSC_LONG_NAME, long_name)))
		return ret;
	    if (arg.verbose)
		cout << "attribute long_name " << long_name << "\n";

	    // Read the attributes of this variable.
	    for (int a = 0; a < var_natts; a++)
	    {
		char att_name[NC_MAX_NAME + 1];
		int att_xtype;
		size_t att_len;

		// Learn about the attributes.
		if ((ret = nc_inq_attname(ncid, v, a, att_name)))
		    return ret;
		if ((ret = nc_inq_att(ncid, v, att_name, &att_xtype, &att_len)))
		    return ret;
		if (arg.verbose)
		    cout << "att_name " << att_name << " att_xtype " << att_xtype << " att_len " <<
			att_len << "\n";

		// If this is the long_name, we've already dealt with it.
		if (!strcmp(att_name, SSC_LONG_NAME)) continue;
	    } // next att
	} // next var
	     
	// Close sidecar file.
	if (arg.verbose)
	    cout << "Closing sidecar file.\n";
	if ((ret = nc_close(ncid)))
	    return ret;
    }

    return 0;
}
