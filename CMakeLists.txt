cmake_minimum_required(VERSION 3.12.2)
project(cpptemplateproject VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_EXTENSIONS ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Prepare "Catch" library for other executables
set(CATCH_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/Catch2/catch2)
add_library(Catch2 INTERFACE)
target_include_directories(Catch2 INTERFACE ${CATCH_INCLUDE_DIR})

# Main application
add_executable(main
    src/main.cpp
)
target_include_directories(main PUBLIC inc)
target_include_directories(main PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/olcPixelGameEngine)
target_link_libraries(main
    X11
    png
    GL
)

# Test application
add_executable(test_main
    test/test_main.cpp
)
target_include_directories(test_main PUBLIC inc)
target_link_libraries(test_main
    Catch2
)
