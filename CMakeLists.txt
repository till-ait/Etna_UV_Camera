cmake_minimum_required(VERSION 3.15)
project(Etna_UV_Camera VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

set(CMAKE_AUTOMOC ON)
set(CMAKE_PREFIX_PATH "C:\\Qt\\6.10.2\\msvc2022_64")
find_package(Qt6 REQUIRED COMPONENTS Widgets Gui Core Charts)

set(EBUS_INCLUDE_DIR "C:/Program Files/JAI/eBUS SDK/Includes")
set(EBUS_LIB_DIR     "C:/Program Files/JAI/eBUS SDK/Libraries")

add_subdirectory(lib)
add_subdirectory(src)