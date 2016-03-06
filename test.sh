#!/bin/bash

set -e
set -u

CXX=${CXX:-clang++-3.7}
OUT=build
INCLUDES="-Iinclude/"
#INCLUDES+=" -I${HOME}/cxxlibs/hana/include/"
INCLUDES+=" -Ideps/github.com/Naios/function2/include/"
OPTS="-std=c++1y -Wall -Werror"

mkdir -p ${OUT}

CMD="$CXX test/main.cpp -o ${OUT}/test ${INCLUDES} ${OPTS}"
echo $CMD
$CMD

${OUT}/test
