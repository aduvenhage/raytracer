#!/bin/bash

# verify environment variables
echo
echo Bash Environment:
printenv

# verify files in root
echo
echo Root folder content:
ls

# start rendering
cd raytracer_cli
echo Running $SCENARIO
./raytracer_cli $SCENARIO

exit 1
