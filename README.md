# STAREmaster

This is a utility to use the STARE library on netCDF and HDF5
files. For full documentation, see
https://spatiotemporal.github.io/STAREmaster/

For more info about STARE, see [the STARE GitHub
site](https://github.com/SpatioTemporal/STARE) and [STARE - TOWARD
UNPRECEDENTED GEO-DATA
INTEROPERABILITY](https://www.researchgate.net/publication/320908197_STARE_-_TOWARD_UNPRECEDENTED_GEO-DATA_INTEROPERABILITY).

## Supported Datasets

Dataset        | STAREmaster ID | Full Name | Notes
-------        | -------------- | --------- | -----
MODIS MYD05    | MOD05          | Total Precipitable Water Vapor 5-Min L2 Swath 1km and 5km | https://ladsweb.modaps.eosdis.nasa.gov/missions-and-measurements/products/MYD05_L2
MODIS MYD09    | MOD09          | Atmospherically Corrected Surface Reflectance 5-Min L2 Swath 250m, 500m, 1km | LAADS DAAC https://ladsweb.modaps.eosdis.nasa.gov/missions-and-measurements/products/MOD09

## Building STAREmaster

STAREmaster has both autotools and CMake build systems. The
STAREmaster program requires several libraries.

Library | Required Version      | Source
------- | ----------------      | ------
STARE   | 0.16.3-beta           | https://github.com/SpatioTemporal/STARE/releases/tag/0.16.3-beta
HDF4    | 4.2.13                | https://support.hdfgroup.org/ftp/HDF/HDF_Current/src/hdf-4.2.13.tar.gz
HDFEOS2 | 1.00                  | https://observer.gsfc.nasa.gov/ftp/edhs/hdfeos/latest_release/HDF-EOS2.20v1.00.tar.Z
HDF5    | 1.8.x, 1.10.x, 1.12.x | https://support.hdfgroup.org/ftp/HDF5/releases/hdf5-1.10/hdf5-1.10.6/src/hdf5-1.10.6.tar.gz
netcdf-c| 4.x                   | https://github.com/Unidata/netcdf-c/releases

### Build Notes

When building HDF4 and HDFEOS2, there are some build issues. As
demonstrated in the GitHub workflow
builds(ex. https://github.com/SpatioTemporal/STAREmaster/blob/master/.github/workflows/autotools.yml),
set the CFLAGS to -fPIC.

When building HDF4, the --disable-netcdf option must be used.

Building HDF4 can be accomplished with these commands:

<pre>
export CFLAGS=-fPIC
./configure --prefix=/usr/local/hdf-4.2.15_fPIC --disable-netcdf
make all
make check
sudo make install
</pre>

The hdfeos documentation can be found here:
https://edhs1.gsfc.nasa.gov/waisdata/eeb/pdf/170eeb001.pdf

When building hdfeos, the --enable-install-include option must be used
with configure. Building hdfeos can be accomplished with these commands:

<pre>
export CPPFLAGS=-I/usr/local/hdf-4.2.15_fPIC/include LDFLAGS=-L/usr/local/hdf-4.2.15_fPIC/lib
export CFLAGS=-fPIC
./configure --prefix=/usr/local/hdfeos_fPIC --enable-install-include
make all
make check
sudo make install
</pre>

### Building with Autotools

When building with autotools locations of all required header files
and libraries must be provided via the CPPFLAGS and LDFLAGS
environmental variable. For example:

<pre>
export CPPFLAGS="-I/usr/local/hdfeos_fPIC/include -I/usr/local/hdf-4.2.15_fPIC/include -I/usr/local/STARE-0.16.3/include -I/usr/local/netcdf-c-4.7.4-development_hdf5-1.10.6/include"
export LDFLAGS="-L/usr/local/hdfeos_fPIC/lib -L/usr/local/hdf-4.2.15_fPIC/lib -L/usr/local/STARE-0.16.3/lib -L/usr/local/netcdf-c-4.7.4-development_hdf5-1.10.6/lib"
</pre>

Configure and make in the standard way:

<pre>
autoreconf -i
./configure
make check
make install
make clean
</pre>

### Building with CMake

CMake finds the necessary library using CMake variables:

<pre>
mkdir build
cd build
cmake  -DTEST_LARGE=/home/ed -DENABLE_LARGE_FILE_TESTS=ON -DCMAKE_BUILD_TYPE=Debug --trace-source=test/CMakeLists.txt -DNETCDF_INCLUDES=/usr/local/netcdf-c-4.7.4_hdf5-1.10.6/include -DNETCDF_LIBRARIES=/usr/local/netcdf-c-4.7.4_hdf5-1.10.6/lib -DSTARE_INCLUDE_DIR=/usr/local/STARE-0.15.6/include -DSTARE_LIBRARY=/usr/local/STARE-0.15.6/lib -DCMAKE_PREFIX_PATH="/usr/local/hdf-4.2.15;/usr/local/hdfeos" .. 
make VERBOSE=1
make VERBOSE=1 test
</pre>

Sometimes the FindNetCDF.cmake module has difficulty finding all the netCDF libraries needed for linking. This has been identified as an issue but the fix is non-trivial (see https://github.com/SpatioTemporal/STAREmaster/issues/11). A workaround is to set the CMAKE_CXX_STANDARD_LIBRARIES variable when invoking cmake. For example:

<pre>
cmake -DTEST_LARGE=/home/ed -DENABLE_LARGE_FILE_TESTS=ON -DCMAKE_BUILD_TYPE=Debug --trace-source=CMakeLists.txt -DNETCDF_INCLUDES=/usr/local/netcdf-c-4.7.4_hdf5-1.10.6/include -DNETCDF_LIBRARIES=/usr/local/netcdf-c-4.7.4_hdf5-1.10.6/lib -DSTARE_INCLUDE_DIR=/usr/local/STARE-0.16.3/include -DSTARE_LIBRARY=/usr/local/STARE-0.16.3/lib -DCMAKE_PREFIX_PATH="/usr/local/hdf-4.2.15;/usr/local/hdfeos" -DCMAKE_CXX_STANDARD_LIBRARIES="-lcurl" ..
</pre>

To make use of OpenMP multi-threading you may need to add -DOpenMP_EXE_LINKER_FLAGS=...
to the cmake command line.

## Using STAREmaster

The STAREmaster package installs a binary createSidecarFile, whcih may
be used to create a sidecar file for any of the supported datasets. To
use:

<pre>
createSidecarFile -d MOD09 data/MYD09.A2020058.1515.006.2020060020205.hdf
</pre>

To see a number usage options execute _createSidecarFile_ without any
command line arguments.

To enable OpenMP multithreading, you may need to set the environment
variable OMP_NUM_THREADS to the number of threads you wish used.

## References

Kuo, K-S and ML Rilee, “STARE – Toward unprecedented geo-data
interoperability,” 2017 Conference on Big Data from Space. Toulouse,
France. 28-30 November 2017, retrieved on Sep 19,2020 from
https://www.researchgate.net/publication/320908197_STARE_-_TOWARD_UNPRECEDENTED_GEO-DATA_INTEROPERABILITY.

Kuo, K-S, H. YuYu, and ML Rilee, "Leveraging STARE for Co-aligned Data
Locality with netCDF and Python MPI", 2019 IEEE International
Geoscience and Remote Sensing Symposium, June 2019, retrieved on Sep
19, 2020 from
https://www.researchgate.net/publication/337504144_Leveraging_STARE_for_Co-aligned_Data_Locality_with_netCDF_and_Python_MPI.

HDF-EOS Library User's Guide Volume 1: Overview and Examples,
retrieved Sep 7, 2020 from
http://newsroom.gsfc.nasa.gov/sdptoolkit/docs/2.20/HDF-EOS_UG.pdf

HDF-EOS Library User's Guide Volume 2: Function Reference Guide,
retrieved Sep 7, 2020 from
http://newsroom.gsfc.nasa.gov/sdptoolkit/docs/2.20/HDF-EOS_REF.pdf

HDF-EOS Interface Based on HDF5, Volume 1: Overview and Examples,
retrieved Sep 7, 2020 from
http://newsroom.gsfc.nasa.gov/sdptoolkit/docs/2.20/HDF-EOS5_UG.pdf

HDF-EOS Interface Based on HDF5, Volume 2: Function Reference Guide,
retrieved Sep 7, 2020 from
http://newsroom.gsfc.nasa.gov/sdptoolkit/docs/2.20/HDF-EOS5_REF.pdf
