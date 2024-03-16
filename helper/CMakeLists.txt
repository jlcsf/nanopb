cmake_minimum_required(VERSION 3.0)
project(client C)

# Add the executable target
add_executable(request request.c)

file(GLOB HEADER_FILES ../headers/*.h)
file(GLOB SOURCE_FILES ../headers/*.c)
target_sources(request PRIVATE ${HEADER_FILES} ${SOURCE_FILES})

# Set include directories
target_include_directories(request PRIVATE ../headers/ ../nanopb/)

# Add nanopb library files
target_sources(request PRIVATE
    ../nanopb/pb_encode.c
    ../nanopb/pb_decode.c
    ../nanopb/pb_common.c
)
