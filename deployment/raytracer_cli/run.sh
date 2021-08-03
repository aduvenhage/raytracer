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
echo
cd raytracer_cli
echo Running: raytracer_cli $SCENARIO $OUTPUT
./raytracer_cli $SCENARIO $OUTPUT

exit 1
