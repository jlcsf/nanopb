cmake_minimum_required(VERSION 3.0)
project(my_project)

# Add the executable target
add_executable(client client.c)

file(GLOB HEADER_FILES ../headers/*.h)
file(GLOB SOURCE_FILES ../headers/*.c)
target_sources(client PRIVATE ${HEADER_FILES} ${SOURCE_FILES})

# Set include directories
target_include_directories(client PRIVATE ../headers/ ../nanopb/)

# Add nanopb library files
target_sources(client PRIVATE
    ../nanopb/pb_encode.c
    ../nanopb/pb_decode.c
    ../nanopb/pb_common.c
)

