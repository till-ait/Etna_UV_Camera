cmake_minimum_required(VERSION 3.15)
project(Etna_UV_Camera VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

set(EBUS_INCLUDE_DIR "C:/Program Files/JAI/eBUS SDK/Includes")
set(EBUS_LIB_DIR     "C:/Program Files/JAI/eBUS SDK/Libraries")

add_subdirectory(lib)
add_subdirectory(src)