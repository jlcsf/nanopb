cmake_minimum_required(VERSION 3.0)
project(client_helper C)

# Add the executable target with client.c and requests.c
add_executable(client client.c ./requests.c)
# Specify the header files explicitly
set(HEADER_FILES
    ../headers/agent.pb.h
    ../headers/error.pb.h
    ../headers/genop.pb.h
    ../headers/image.pb.h
    ../headers/profiling.pb.h
    ../headers/resources.pb.h
    ../headers/session.pb.h
    ../headers/tensorflow.pb.h
    ../headers/torch.pb.h
    ./requests.h
    ./client.h
)

# Add header files to the target
target_sources(client PRIVATE ${HEADER_FILES})

# Set include directories
target_include_directories(client PRIVATE ../headers/ ../nanopb/)

# Add nanopb library files
target_sources(client PRIVATE
    ../nanopb/pb_encode.c
    ../nanopb/pb_decode.c
    ../nanopb/pb_common.c
)

# Add the requests.c file explicitly
target_sources(client PRIVATE requests.c)
