cmake_minimum_required(VERSION 3.10)

# Set your project name
project(OspreyProject)

# Find the required packages
find_package(Protobuf REQUIRED)
find_package(gRPC REQUIRED)

# Include directories
include_directories(${Protobuf_INCLUDE_DIRS})
include_directories(${gRPC_INCLUDE_DIRS})

set(SOURCES
    src/main.cpp
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})


add_executable(server ${SOURCES})

target_link_libraries(server
    PRIVATE
        proto
)
target_include_directories(client PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/../proto)
