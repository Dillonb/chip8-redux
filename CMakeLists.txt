cmake_minimum_required (VERSION 3.10)

ADD_COMPILE_OPTIONS(-Wall -march=native)

set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -DCH8_DEBUG_MODE")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DCH8_DEBUG_MODE")

project (chip8)
set(CHIP8_TARGET chip8)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake/modules")
add_subdirectory(src)