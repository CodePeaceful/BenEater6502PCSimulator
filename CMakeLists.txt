cmake_minimum_required(VERSION 3.24)

project(EaterPC)

set(CMAKE_CXX_STANDARD 23)

message ("${CMAKE_C_COMPILER}")
message ("${CMAKE_CXX_COMPILER}")
message ("${CMAKE_CXX_COMPILER_AR}")
message ("${CMAKE_CXX_COMPILER_RANLIB}")

if (MSVC)
    # warning level 4
    add_compile_options(/W4)
    #add_compile_options(/O2)
else()
    # additional warnings
    add_compile_options(-Wall -Wextra -Wpedantic)
    #add_compile_options(-O3)
    #add_link_options(-flto)
endif()

set(SFML_STATIC_LIBRARIES TRUE)

add_subdirectory(libs/SFML-3.0.0)

enable_testing()

add_subdirectory(src)

add_subdirectory(tests)