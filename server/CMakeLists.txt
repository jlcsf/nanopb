cmake_minimum_required(VERSION 3.0)
project(server_helper C)

add_executable(server_helper server_helper.c)

file(GLOB HEADER_FILES ../headers/*.h)
file(GLOB SOURCE_FILES ../headers/*.c)
target_sources(server_helper PRIVATE ${HEADER_FILES} ${SOURCE_FILES})

# Set include directories
target_include_directories(server_helper PRIVATE ../headers/ ../nanopb/)

# Add nanopb library files
target_sources(server_helper PRIVATE
    ../nanopb/pb_encode.c
    ../nanopb/pb_decode.c
    ../nanopb/pb_common.c
)







# Add executable and source files
add_executable(server server.c python_bindings.c)

# Include header files
file(GLOB HEADER_FILES ../headers/*.h)
file(GLOB SOURCE_FILES ../headers/*.c)
target_sources(server PRIVATE ${HEADER_FILES} ${SOURCE_FILES})

# Set include directories
target_include_directories(server PRIVATE 
    ../headers/ 
    ../nanopb/ 
    /usr/include/python3.11         # Add Python include path here                     
)

target_link_libraries(server PRIVATE vaccel)

# Add nanopb library files
target_sources(server PRIVATE
    ../nanopb/pb_encode.c
    ../nanopb/pb_decode.c
    ../nanopb/pb_common.c
)







### add shared object for python wrapper using ctypes

add_library(server_shared SHARED 
    server.c
    ../nanopb/pb_encode.c
    ../nanopb/pb_decode.c
    ../nanopb/pb_common.c
)
target_include_directories(server_shared PRIVATE 
    ../headers/ 
    ../nanopb/
)
target_sources(server_shared PRIVATE ${HEADER_FILES} ${SOURCE_FILES})

target_link_libraries(server PRIVATE server_shared)