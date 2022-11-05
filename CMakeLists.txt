cmake_minimum_required(VERSION 3.23)
project(bta C)

set(CMAKE_C_STANDARD 17)

add_executable(bta main.c /usr/include/cjson/cJSON.c install.h main.h usage.h)
