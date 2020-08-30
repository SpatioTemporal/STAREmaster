# This is part of STAREmaster. This is a script to run tests.
set -e
set -x

echo "*** creating sidecar file for MOD05..."
../src/createSidecarFile data/MOD05_L2.A2005349.2125.061.2017294065400.hdf

../src/createSidecarFile -d MOD043K data/MYD04_3K.A2020001.0040.061.2020002235147.hdf

echo "*** creating CDL of MOD05 sidecar file header..."
ncdump -h data/MOD05_L2.A2005349.2125.061.2017294065400_stare.nc > MOD05_L2.A2005349.2125.061.2017294065400_stare_out.cdl

echo "*** checking that sidecar header is correct..."
diff -b -w MOD05_L2.A2005349.2125.061.2017294065400_stare_out.cdl ref_MOD05_L2.A2005349.2125.061.2017294065400_stare.cdl

