cmake_minimum_required(VERSION 3.16)
cmake_policy(SET CMP0092 NEW)
cmake_policy(SET CMP0167 NEW)
include(FetchContent)
project(dte3611_2025_YOUR_NAME_HERE VERSION 0.1 LANGUAGES CXX)

if(MSVC AND NOT VCPKG_TOOLCHAIN)
  message(FATAL_ERROR
          "Enable VCPKG toolchain on MSVC platform\n"
          "  Star over; CMAKE_TOOLCHAIN_FILE definition missing!\n"
          "  i.e.: cmake -DCMAKE_TOOLCHAIN_FILE=<path_to>\\vcpkg\\scripts\\buildsystems\\vcpkg.cmake <path_to>\\source\\"
  )
endif()

set(CLANG_COMPILE_OPTIONS -Weverything -pedantic -Werror -Wno-c++98-compat -Wno-c++98-compat-pedantic -Wno-documentation)
set(GCC_COMPILE_OPTIONS -pedantic -Wall -Werror)
set(MSVC_COMPILE_OPTIONS /WX /W4 /permissive-)

# Configuration pour Boost Graph uniquement
set(BOOST_INCLUDE_LIBRARIES graph)

FetchContent_Declare(Boost
        URL https://github.com/boostorg/boost/releases/download/boost-1.82.0/boost-1.82.0.tar.xz
        DOWNLOAD_EXTRACT_TIMESTAMP ON
        EXCLUDE_FROM_ALL  # Disponible CMake 3.28+, sinon voir alternative ci-dessous
)

FetchContent_MakeAvailable(Boost)

add_subdirectory(lib3611)
target_link_libraries(lib3611 INTERFACE Boost::graph)

add_subdirectory(benchmark_and_testing)