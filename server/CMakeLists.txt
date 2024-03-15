cmake_minimum_required(VERSION 3.0)
project(server C)

add_executable(server server.c)

file(GLOB HEADER_FILES ../headers/*.h)
file(GLOB SOURCE_FILES ../headers/*.c)
target_sources(server PRIVATE ${HEADER_FILES} ${SOURCE_FILES})

# Set include directories
target_include_directories(server PRIVATE ../headers/ ../nanopb/)

# Add nanopb library files
target_sources(server PRIVATE
    ../nanopb/pb_encode.c
    ../nanopb/pb_decode.c
    ../nanopb/pb_common.c
)
