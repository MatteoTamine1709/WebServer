cmake_minimum_required(VERSION 3.0.0)
project(WebServer VERSION 0.1.0)


include(CTest)
enable_testing()

file(GLOB TcpServerImpl src/TcpServerImpl/*.cpp)

add_executable(WebServer main.cpp ${TcpServerImpl} src/TcpConnection.cpp src/HttpRequestHeader.cpp src/HttpResponseHeader.cpp src/utils.cpp)

set(CMAKE_BUILD_TYPE Debug)
set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 20)

find_package(Threads REQUIRED)
find_package(spdlog REQUIRED)
find_package(nlohmann_json 3.2.0 REQUIRED)

target_link_libraries(WebServer PRIVATE Threads::Threads spdlog::spdlog nlohmann_json::nlohmann_json)

# add assets folder
file(COPY utils DESTINATION ${CMAKE_BINARY_DIR})
file(COPY endpoints DESTINATION ${CMAKE_BINARY_DIR})
file(COPY public DESTINATION ${CMAKE_BINARY_DIR})