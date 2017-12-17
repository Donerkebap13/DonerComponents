#!/bin/bash -e

PROJECT_PATH="${PWD}/../project_travis"

echo ${PROJECT_PATH}

mkdir -p ${PROJECT_PATH}

cd ${PROJECT_PATH}

cmake "${PROJECT_PATH}/../DonerECS" -DCMAKE_CONFIGURATION_TYPES="Debug" -DDECS_ENABLE_TESTS=1 -DMAX_ENTITIES=4096 -DMAX_TAGS=64

make VERBOSE=1
make test
