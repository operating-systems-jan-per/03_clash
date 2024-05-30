cmake_minimum_required(VERSION 3.28)
project(03_clash C)

set(CMAKE_C_STANDARD 11)

include_directories(.)

add_executable(03_clash
        plist.c
        plist.h
        clash.c)
