#!/bin/bash

INPUT=$1
BUILDFOLDER=../build

# get the operating system string
OS=`uname`

# print detected operating system
echo "operating system: '"$OS"'"

# default value for building jobs
CORES=1

# OS specific available number of cores extraction
if   [ "$OS" == "Darwin" ]; then
   CORES=`sysctl -n hw.ncpu`
elif [ "$OS" == "Linux" ]; then
   CORES=`grep -c ^processor /proc/cpuinfo`
else 
   echo "no available CPU core extraction available"
fi
echo "building with " $CORES "cores " 

./clean.sh
if [ "$INPUT" != "" ]; then
   if [ "$INPUT" == "help" ] ; then
      echo "usage: ./run_regression.sh [options]"
      echo ""
      echo "options:"
      echo "  help      prints this help message"
      echo "  submit    submits the result to online cdash service"
      exit -1
   elif [ "$INPUT" == "submit" ] ; then
      mkdir -p $BUILDFOLDER
      cd $BUILDFOLDER
      cmake ..
      # build and submit results to online cdash service
      make Nightly ARGS=-j$CORES
      echo ""
      echo "regression result is available here:"
      echo "----------------------------------------------"
      echo "http://my.cdash.org/index.php?project=ViennaData"
      echo "----------------------------------------------"
      echo ""
   else 
      echo ""
      echo "# Error - wrong option: \""$INPUT"\""
      echo ""
      echo "usage: ./run_regression.sh [options]"
      echo ""
      echo "options:"
      echo "  help      prints this help message"
      echo "  submit    submits the result to online cdash service"
      exit -1
   fi
else
   mkdir -p $BUILDFOLDER
   cd $BUILDFOLDER
   cmake ..
   # plain build without submit
   make -j$CORES
   make test ARGS=-j$CORES
fi


