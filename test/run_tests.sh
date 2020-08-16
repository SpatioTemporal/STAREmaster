# This script runs tests for the STAREmaster application.
#
# Ed Hartnett, 5/1/20

set -e
../src/createSidecarFile data/MOD05_L2.A2005349.2125.061.2017294065400.hdf

# Check the sidecar file here!

../src/createSidecarFile -d MOD043K data/MYD04_3K.A2020001.0040.061.2020002235147.hdf

