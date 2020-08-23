# STAREmaster

This is a utility to use the STARE library on netCDF and HDF5
files. For full documentation, see
https://spatiotemporal.github.io/STAREmaster/

For more info about STARE, see [the STARE GitHub
site](https://github.com/SpatioTemporal/STARE)

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

### Building with CMake

## Using STAREmaster

## Supported Datasets

Dataset | STAREmaster ID | Full Name | Notes
------- | -------------- | --------- | -----
MYD09   | MOD09          | Atmospherically Corrected Surface Reflectance 5-Min L2 Swath 250m, 500m, 1km | LAADS DAAC https://ladsweb.modaps.eosdis.nasa.gov/missions-and-measurements/products/MOD09
