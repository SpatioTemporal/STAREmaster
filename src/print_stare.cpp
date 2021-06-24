//
// Created by James Gallagher on 6/24/21.
//
// Utility to simplify working with STARE indices.

#include "config.h"

#include <getopt.h>
#include <iostream>
#include <sstream>
#include <bitset>

using namespace std;

void usage(char *name) {
    cout
    << "STARE index utility. " << endl
    << "Usage: " << name << " [options] STARE index " << endl
    << "Examples:" << endl
    << "  " << name << "-f h 4015281913904386217" << endl
    << endl
    << "Options:" << endl
    << " -h, --help        : print this help" << endl
    << " -v, --verbose     : verbose: print all" << endl
    << " -f, --format h|b  : output format" << endl
    << " -s, --split       : Split the location and resolution information" << endl
    << " -b, --build <loc> <res> : Build a S-index from the location and resolution;" << endl
    << "                           loc and res are 59-bit and 5-bit hex numbers, respectively." << endl
    << "                           Each of the values should use the '0x' prefix." << endl;

    exit(0);
};

struct Arguments {
    bool verbose = false;
    string format;
    bool split = false;
    bool build = false;
    int err_code = 0;
};

Arguments parseArguments(int argc, char *argv[]) {
    if (argc == 1) usage(argv[0]);
    Arguments arguments;
    static struct option long_options[] = {
            {"help",             no_argument,       nullptr, 'h'},
            {"verbose",          no_argument,       nullptr, 'v'},
            {"format",           required_argument, nullptr, 'f'},
            {"split",            no_argument,       nullptr, 's'},
            {"build",            no_argument,       nullptr, 'b'},
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
            case 'f':
                arguments.format = optarg;
                break;
            case 's':
                arguments.split = true;
                break;
            case 'b':
                arguments.build = true;
                break;
            default:
                usage(argv[0]);
        }
    }

    // Check for 'format'' consistency.
    if (arguments.format.empty() && !arguments.build) {
        cerr << "Must include a print representation using --format=[h|b] or specify --build" << endl;
        arguments.err_code = 99;
    }

    return arguments;
};

int main(int argc, char *argv[])
{
    Arguments arg = parseArguments(argc, argv);

    // Input STARE index must be provided.
    if (!argv[optind]) {
        cerr << "Must provide input STARE index." << endl;
        return 1;
    }

    if (arg.err_code) {
        return arg.err_code;
    }

    if (arg.build) {
        // must be two args
        if (argc - optind != 2) {
            cerr << "Build requires two arguments" << endl;
            return 1;
        }
        istringstream iss{string(argv[optind]) + " " + string(argv[optind+1]) };
        unsigned long long s_index;
        iss >> hex >> s_index; // leading_zero >> x >> hex
        unsigned short res;
        iss >> hex >> res; // >> leading_zero >> x

        s_index = s_index << 5;
        s_index += res;

        cout << s_index << endl;
    }
    else {
        if (arg.format.find_first_of("hb") == string::npos) {
            cerr << "Must include a print representation using --format=[h|b]" << endl;
            return 1;
        }

        istringstream iss{string(argv[optind])};
        unsigned long long s_index;
        iss >> s_index;

        if (arg.format.find('h') != string::npos) {
            if (arg.split) {
                unsigned long long location = s_index >> 5;
                cout << "Location: 0x" << hex << location << endl;
                unsigned short resolution = s_index & 0x000000000000001f;
                cout << "Resolution: 0x" << hex << resolution << endl;
            }
            else {
                cout << "0x" << hex << s_index << endl;
            }
        }
        else if (arg.format.find('b') != string::npos) {
            if (arg.split) {
                unsigned long long location = s_index >> 5;
                cout << "Location: b" << bitset<59>(location) << endl;
                unsigned short resolution = s_index & 0x000000000000001f;
                cout << "Resolution: b" << bitset<5>(resolution) << endl;
            }
            else {
                cout << "b" << bitset<64>(s_index) << endl;
            }
        }
        else {
            cerr << "Unrecognized output format." << endl;
            return 1;
        }
    }
    return 0;
}
