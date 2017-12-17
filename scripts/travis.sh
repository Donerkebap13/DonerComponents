#!/bin/bash -e

cd ".."

python project_generation_scripts.py --generate-tests

cd generate_solution_scripts/linux

sh debug-01-generate.sh

make VERBOSE=1
make test
