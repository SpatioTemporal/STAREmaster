# This is part of STAREmaster. This is a script to run tests.
set -e
set -x

# Create a sidecar file.
../src/createSidecarFile data/MOD05_L2.A2005349.2125.061.2017294065400.hdf

# Create a CDL file of the header info from the STARE index sidecar
# file we produced.
ncdump -h data/MOD05_L2.A2005349.2125.061.2017294065400_stare.nc > MOD05_L2.A2005349.2125.061.2017294065400_stare_out.cdl

