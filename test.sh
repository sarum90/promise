#!/bin/bash
#
# This is a quick/lazy bash script to compile and execute a test cpp file in
# `test/<test-name>.cpp`. It expectest <test-name> to be passed in as the first
# and only argument.

set -e
set -u

TEST=$1

CXX=${CXX:-clang++-3.7}
OUT=build
INCLUDES="-Iinclude/"
INCLUDES+=" -Ideps/github.com/jimporter/mettle/include/"
INCLUDES+=" -Ideps/github.com/Naios/function2/include/"
INCLUDES+=" -Ideps/github.com/mapbox/variant/"
OPTS="-std=c++1y -Wall -Werror"

mkdir -p ${OUT}


CMD="$CXX test/${TEST}.cpp -o ${OUT}/${TEST} ${INCLUDES} ${OPTS}"
echo $CMD
$CMD
${OUT}/${TEST}
