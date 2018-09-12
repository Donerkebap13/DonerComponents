#!/bin/bash

ROOT=`pwd`
PROJECT_PATH=${ROOT}/project_clang
EXTRA_CMAKE_PARAMETERS=$1

if [ ! -d ${PROJECT_PATH} ]; then
    mkdir -p ${PROJECT_PATH}
fi

cd ${PROJECT_PATH}

export CC=C:/msys64/mingw32/bin/clang.exe
export CXX=C:/msys64/mingw32/bin/clang++.exe

cmake "../DonerECS" -G"MSYS Makefiles" -DMAX_ENTITIES=4096 -DMAX_TAGS=64 -DBUILD_SHARED_LIBS=0 -DDECS_ENABLE_TESTS=1 ${EXTRA_CMAKE_PARAMETERS}

make -j8
make CTEST_OUTPUT_ON_FAILURE=1 test -j8
