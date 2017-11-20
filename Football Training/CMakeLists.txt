cmake_minimum_required(VERSION 3.8)
project(Football_Training)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp Field.h Element.h Player.h Ball.h Field.cpp Element.cpp Player.cpp Global.h Ball.cpp Statistics.h Statistics.cpp)
add_executable(Football_Training ${SOURCE_FILES})

include_directories(/usr/include/mpi/)
set(CMAKE_C_COMPILER "/usr/bin/mpicc")
set(CMAKE_CXX_COMPILER "/usr/bin/mpic++")