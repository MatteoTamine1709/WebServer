cmake_minimum_required(VERSION 3.0.0)
project(WebServer VERSION 0.1.0)

include(CTest)
enable_testing()

add_executable(WebServer main.cpp src/TcpServer.cpp src/TcpConnection.cpp src/HttpRequestHeader.cpp src/HttpResponseHeader.cpp src/utils.cpp)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

# C++ 20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add pthread
find_package(Threads REQUIRED)

# add assets folder
file(COPY utils DESTINATION ${CMAKE_BINARY_DIR})
file(COPY handlers DESTINATION ${CMAKE_BINARY_DIR})
file(COPY public DESTINATION ${CMAKE_BINARY_DIR})
