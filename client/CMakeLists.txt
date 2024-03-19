cmake_minimum_required(VERSION 3.0)
project(client_helper C)

# Add the executable target
add_executable(client_helper client_helper.c)

file(GLOB HEADER_FILES ../headers/*.h)
file(GLOB SOURCE_FILES ../headers/*.c)
target_sources(client_helper PRIVATE ${HEADER_FILES} ${SOURCE_FILES})

# Set include directories
target_include_directories(client_helper PRIVATE ../headers/ ../nanopb/)

# Add nanopb library files
target_sources(client_helper PRIVATE
    ../nanopb/pb_encode.c
    ../nanopb/pb_decode.c
    ../nanopb/pb_common.c
)

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

