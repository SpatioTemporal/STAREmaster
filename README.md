# STAREmaster

This is a utility to use the STARE library on netCDF and HDF5
files. For full documentation, see
https://spatiotemporal.github.io/STAREmaster/

For more info about STARE, see [the STARE GitHub
site](https://github.com/SpatioTemporal/STARE)

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
STARE   | 0.15.6-beta           | https://github.com/SpatioTemporal/STARE/releases/tag/0.15.6-beta
HDF4    | 4.2.13                | https://support.hdfgroup.org/ftp/HDF/HDF_Current/src/hdf-4.2.13.tar.gz
HDFEOS2 | 1.00                  | https://observer.gsfc.nasa.gov/ftp/edhs/hdfeos/latest_release/HDF-EOS2.20v1.00.tar.Z
HDF5    | 1.8.x, 1.10.x, 1.12.x | https://support.hdfgroup.org/ftp/HDF5/releases/hdf5-1.10/hdf5-1.10.6/src/hdf5-1.10.6.tar.gz
netcdf-c| 4.x                   | https://github.com/Unidata/netcdf-c/releases

### Building with Autotools

When building with autotools locations of all required header files
and libraries must be provided via the CPPFLAGS and LDFLAGS
environmental variable. For example:

<pre>
export CPPFLAGS="-I/usr/local/hdfeos/include -I/usr/local/hdf-4.2.15/include -I/usr/local/STARE-0.15.6/include -I/usr/local/netcdf-c-4.7.4-development_hdf5-1.10.6/include"
export LDFLAGS="-L/usr/local/hdfeos/lib -L/usr/local/hdf-4.2.15/lib -L/usr/local/STARE-0.15.6/lib -L/usr/local/netcdf-c-4.7.4-development_hdf5-1.10.6/lib"
</pre>

Configure and make in the standard way:

<pre>
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

## Using STAREmaster

The STAREmaster package installs a binary createSidecarFile, whcih may
be used to create a sidecar file for any of the supported datasets. To
use:

<pre>
createSidecarFile -d MOD09 data/MYD09.A2020058.1515.006.2020060020205.hdf
</pre>


