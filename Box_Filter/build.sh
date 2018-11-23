#!/bin/bash
mkdir -p build
mkdir -p bin
cd build
if [ `ls -a | wc -l` -eq 2 ]
then
cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Debug ..
fi
cd ..
cmake --build ./build  -- -j 2

copyIfExists() { # $1 - source file, $2 - destination file name
[[ -e $1 ]] && cp $1 $2
return 0
}

cd bin
copyIfExists ../build/Box_Filter .



