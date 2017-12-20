#!/bin/bash -e
set -evx

# if possible, ask for the precise number of processors,
# otherwise take 2 processors as reasonable default; see
# https://docs.travis-ci.com/user/speeding-up-the-build/#Makefile-optimization
if [ -x /usr/bin/getconf ]; then
    NPROCESSORS=$(/usr/bin/getconf _NPROCESSORS_ONLN)
else
    NPROCESSORS=2
fi
# as of 2017-09-04 Travis CI reports 32 processors, but GCC build
# crashes if parallelized too much (maybe memory consumption problem),
# so limit to 4 processors for the time being.
if [ $NPROCESSORS -gt 4 ] ; then
	echo "$0:Note: Limiting processors to use by make from $NPROCESSORS to 4."
	NPROCESSORS=4
fi
# Tell make to use the processors. No preceding '-' required.
MAKEFLAGS="j${NPROCESSORS}"
export MAKEFLAGS

PROJECT_PATH="${PWD}/../project_travis"

echo ${PROJECT_PATH}

mkdir -p ${PROJECT_PATH}

cd ${PROJECT_PATH}

cmake "${PROJECT_PATH}/../DonerECS/DonerECS" -DCMAKE_CONFIGURATION_TYPES="Debug" -DDECS_ENABLE_TESTS=1 -DMAX_ENTITIES=4096 -DMAX_TAGS=64

make VERBOSE=1
make CTEST_OUTPUT_ON_FAILURE=1 test
