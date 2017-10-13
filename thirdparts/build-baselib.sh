#!/bin/bash

if [ x$NDKROOT = "x" ]
then 
    echo ERROR:NDKROOT env variable is not set
	exit 1;
fi
# Environment
cd ./baselib/build
rm -R CMake*
sh androidbuild.sh
make clean
make -j4
make install
