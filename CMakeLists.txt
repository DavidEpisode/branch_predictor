cmake_minimum_required(VERSION 3.12)
project(branch_predictor)

set(CMAKE_CXX_STANDARD 14)

include_directories(src)

add_executable(branch_predictor
        src/main.c
        src/predictor.c
        src/predictor.h
)
