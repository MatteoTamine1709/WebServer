#!/bin/bash
GREEN='\033[0;32m'
NC='\033[0m' # No Color
/usr/bin/cmake --no-warn-unused-cli -DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=TRUE -DCMAKE_BUILD_TYPE:STRING=Debug -DCMAKE_C_COMPILER:FILEPATH=/usr/bin/gcc -DCMAKE_CXX_COMPILER:FILEPATH=/usr/bin/g++ -S/home/mat/dev/C++/streaming/WebServer -B/home/mat/dev/C++/streaming/WebServer/build -G Ninja
/usr/bin/cmake --build /home/mat/dev/C++/streaming/WebServer/build --config Debug --target all --
echo -e "${GREEN}Running WebServer${NC}"
./build/WebServer
