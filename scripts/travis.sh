#!/bin/bash

python project_generation_scripts.py --generate-tests --all-platforms

if [ "${TRAVIS_OS_NAME}" = "linux" ]; then
	sh "generate_solution_scripts/linux/debug-01-generate.sh"
	cd "projects/linux-debug"
elif [ "${TRAVIS_OS_NAME}" = "osx" ]; then
	sh "generate_solution_scripts/darwin/debug-01-generate.sh"
	cd "projects/darwin-debug"
fi

make VERBOSE=1 -j8
make CTEST_OUTPUT_ON_FAILURE=1 test -j8
