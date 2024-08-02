cmake_minimum_required(VERSION 3.5)
project(ProjectName C)

function(add_common_sources target)
    file(GLOB HEADER_FILES "${CMAKE_SOURCE_DIR}/headers/*.h")
    file(GLOB SOURCE_FILES "${CMAKE_SOURCE_DIR}/headers/*.c")

    target_sources(${target} PRIVATE ${HEADER_FILES} ${SOURCE_FILES})

    target_include_directories(${target} PRIVATE "${CMAKE_SOURCE_DIR}/headers" "${CMAKE_SOURCE_DIR}/nanopb")

    target_sources(${target} PRIVATE
        "${CMAKE_SOURCE_DIR}/nanopb/pb_encode.c"
        "${CMAKE_SOURCE_DIR}/nanopb/pb_decode.c"
        "${CMAKE_SOURCE_DIR}/nanopb/pb_common.c"
    )
endfunction()

add_custom_target(generate_headers ALL
    COMMAND python "${CMAKE_SOURCE_DIR}/nanopb/generator/nanopb_generator.py" -D "${CMAKE_SOURCE_DIR}/headers" -I "${CMAKE_SOURCE_DIR}/proto" error.proto
    COMMAND python "${CMAKE_SOURCE_DIR}/nanopb/generator/nanopb_generator.py" -D "${CMAKE_SOURCE_DIR}/headers" -I "${CMAKE_SOURCE_DIR}/proto" genop.proto
    COMMAND python "${CMAKE_SOURCE_DIR}/nanopb/generator/nanopb_generator.py" -D "${CMAKE_SOURCE_DIR}/headers" -I "${CMAKE_SOURCE_DIR}/proto" image.proto
    COMMAND python "${CMAKE_SOURCE_DIR}/nanopb/generator/nanopb_generator.py" -D "${CMAKE_SOURCE_DIR}/headers" -I "${CMAKE_SOURCE_DIR}/proto" profiling.proto
    COMMAND python "${CMAKE_SOURCE_DIR}/nanopb/generator/nanopb_generator.py" -D "${CMAKE_SOURCE_DIR}/headers" -I "${CMAKE_SOURCE_DIR}/proto" resources.proto
    COMMAND python "${CMAKE_SOURCE_DIR}/nanopb/generator/nanopb_generator.py" -D "${CMAKE_SOURCE_DIR}/headers" -I "${CMAKE_SOURCE_DIR}/proto" session.proto
    COMMAND python "${CMAKE_SOURCE_DIR}/nanopb/generator/nanopb_generator.py" -D "${CMAKE_SOURCE_DIR}/headers" -I "${CMAKE_SOURCE_DIR}/proto" tensorflow.proto
    COMMAND python "${CMAKE_SOURCE_DIR}/nanopb/generator/nanopb_generator.py" -D "${CMAKE_SOURCE_DIR}/headers" -I "${CMAKE_SOURCE_DIR}/proto" torch.proto
    COMMAND python "${CMAKE_SOURCE_DIR}/nanopb/generator/nanopb_generator.py" -D "${CMAKE_SOURCE_DIR}/headers" -I "${CMAKE_SOURCE_DIR}/proto" agent.proto
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)

add_executable(client_helper "${CMAKE_SOURCE_DIR}/client/client_helper.c")
add_common_sources(client_helper)
add_dependencies(client_helper generate_headers)

add_executable(client "${CMAKE_SOURCE_DIR}/client/client.c")
add_common_sources(client)
add_dependencies(client generate_headers)

add_executable(server_helper "${CMAKE_SOURCE_DIR}/server/server_helper.c")
add_common_sources(server_helper)
add_dependencies(server_helper generate_headers)

add_executable(server "${CMAKE_SOURCE_DIR}/server/server.c" "${CMAKE_SOURCE_DIR}/server/python_bindings.c")
add_common_sources(server)
add_dependencies(server generate_headers)

target_include_directories(server PRIVATE /usr/include/python3.11)
target_link_libraries(server PRIVATE vaccel)

# Shared library for Python wrapper
add_library(server_shared SHARED "${CMAKE_SOURCE_DIR}/server/server.c")
add_common_sources(server_shared)
target_include_directories(server_shared PRIVATE "${CMAKE_SOURCE_DIR}/headers" "${CMAKE_SOURCE_DIR}/nanopb")
target_link_libraries(server PRIVATE server_shared)
