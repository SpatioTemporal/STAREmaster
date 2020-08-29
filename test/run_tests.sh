# This is part of STAREmaster. This is a script to run tests.
set -e
set -x

echo "*** creating sidecar file for MOD05..."
../src/createSidecarFile data/MOD05_L2.A2005349.2125.061.2017294065400.hdf

# Create a CDL file of the header info from the STARE index sidecar
# file we produced.
echo "*** creating CDL of MOD05 sidecar file header..."
ncdump -h data/MOD05_L2.A2005349.2125.061.2017294065400_stare.nc > MOD05_L2.A2005349.2125.061.2017294065400_stare_out.cdl

echo "*** checking that sidecar header is correct..."
diff -b -w MOD05_L2.A2005349.2125.061.2017294065400_stare_out.cdl MOD05_L2.A2005349.2125.061.2017294065400_stare.cdl


