cmake_minimum_required(VERSION 3.0.0)
project(WebServer VERSION 0.1.0)


enable_testing()

file(GLOB TcpServerImpl src/TcpServerImpl/*.cpp)
file(GLOB ThreadpoolImpl src/ThreadpoolImpl/*.cpp)
file(GLOB Interface src/interface/*.cpp)

# add_library(projectLib )
add_executable(WebServer main.cpp ${TcpServerImpl} ${ThreadpoolImpl} ${Interface} src/TcpConnection.cpp src/HttpRequestHeader.cpp src/utils.cpp)
# target_link_libraries(WebServer PUBLIC projectLib)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 20)

find_package(Threads REQUIRED)
find_package(spdlog REQUIRED)
find_package(nlohmann_json 3.2.0 REQUIRED)

target_link_libraries(WebServer PRIVATE Threads::Threads spdlog::spdlog nlohmann_json::nlohmann_json)
target_include_directories(WebServer PUBLIC ./)


# add assets folder
file(COPY utils DESTINATION ${CMAKE_BINARY_DIR})
file(COPY api DESTINATION ${CMAKE_BINARY_DIR})
file(COPY public DESTINATION ${CMAKE_BINARY_DIR})

# add optimization flags
if (CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_options(WebServer PRIVATE -O3)
endif()


# Include(FetchContent)

# FetchContent_Declare(
#   Catch2
#   GIT_REPOSITORY https://github.com/catchorg/Catch2.git
#   GIT_TAG        v3.3.2 # or a later release
# )

# FetchContent_MakeAvailable(Catch2)

# add_subdirectory(tests)
# include_directories(${CATCH2_INCLUDE_DIRS})


# file(GLOB Tests tests/**/**.cpp)

# add_executable(runTest ${Tests})
# target_link_libraries(runTest PRIVATE projectLib Catch2::Catch2WithMain Threads::Threads spdlog::spdlog nlohmann_json::nlohmann_json)
# target_include_directories(runTest PUBLIC ./)
