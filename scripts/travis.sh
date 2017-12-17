#!/bin/bash -e

PROJECT_PATH="../project_travis"

mkdir -p ${PROJECT_PATH}

cd ${PROJECT_PATH}

cmake "${PROJECT_PATH}/../DonerECS" -DCMAKE_CONFIGURATION_TYPES="Debug" -DDECS_ENABLE_TESTS=1 -DMAX_ENTITIES=4096 -DMAX_TAGS=64

make VERBOSE=1
make test
