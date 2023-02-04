#!/bin/bash

scriptdir=$(cd $(dirname $0) && pwd)

mkdir -p ${scriptdir}/build > /dev/null 2>&1
cd ${scriptdir}/build
cmake ../
cmake --build .

