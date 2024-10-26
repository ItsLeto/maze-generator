# include for val_config.hpp
include_directories(${PROJECT_SOURCE_DIR})

# for generated proto files
include_directories(${CMAKE_BINARY_DIR}/proto)

# include headers in toolchaina
include_directories(${CMAKE_SYSROOT}/usr/include)
