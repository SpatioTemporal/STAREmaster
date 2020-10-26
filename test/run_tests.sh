# This is part of STAREmaster. This is a script to run tests.
set -e
set -x

#echo "*** creating sidecar file for MOD09GA..."
../src/createSidecarFile -w 1 -d MOD09GA data/MOD09GA.A2020009.h00v08.006.2020011025435.hdf

#echo "*** creating CDL of MOD05 sidecar file header..."
#ncdump -h data/MOD09GA.h00v08_stare.nc > MOD09GA.h00v08_stare_out.cdl

#echo "*** checking that sidecar header is correct..."
#diff -b -w MOD09GA.h00v08_stare_out.cdl ref_MOD09GA.h00v08_stare.cdl

echo "*** creating sidecar file for MOD05..."
../src/createSidecarFile -w 1 data/MOD05_L2.A2005349.2125.061.2017294065400.hdf

echo "*** creating CDL of MOD05 sidecar file header..."
ncdump -h data/MOD05_L2.A2005349.2125.061.2017294065400_stare.nc > MOD05_L2.A2005349.2125.061.2017294065400_stare_out.cdl

# Remove the line that has the history attribute, containing date/time.
sed '23d' MOD05_L2.A2005349.2125.061.2017294065400_stare_out.cdl > MOD05_L2.A2005349.2125.061.2017294065400_stare_no_hist_out.cdl

echo "*** checking that sidecar header is correct..."
diff -b -w MOD05_L2.A2005349.2125.061.2017294065400_stare_no_hist_out.cdl ref_MOD05_L2.A2005349.2125.061.2017294065400_stare.cdl

echo "*** creating sidecar file for MOD05 with institution..."
../src/createSidecarFile -w 1 -i "Institute for the Study of Institutes" data/MOD05_L2.A2005349.2125.061.2017294065400.hdf

echo "*** creating CDL of MOD05 sidecar file header with institution..."
ncdump -h data/MOD05_L2.A2005349.2125.061.2017294065400_stare.nc > MOD05_L2.A2005349.2125.061.2017294065400_stare_inst_out.cdl

# Remove the line that has the history attribute, containing date/time.
sed '24d' MOD05_L2.A2005349.2125.061.2017294065400_stare_inst_out.cdl > MOD05_L2.A2005349.2125.061.2017294065400_stare_no_hist_inst_out.cdl

echo "*** checking that sidecar header with institution is correct..."
diff -b -w MOD05_L2.A2005349.2125.061.2017294065400_stare_no_hist_inst_out.cdl ref_MOD05_L2.A2005349.2125.061.2017294065400_inst_stare.cdl



