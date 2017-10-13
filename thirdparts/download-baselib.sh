#!/bin/bash

set -e
LOCAL_PATH=`pwd`
BASELIBPATH=$LOCAL_PATH/baselib
if [ ! -d "$BASELIBPATH" ]; then
    git clone https://github.com/showwaychen/baselib.git
fi

