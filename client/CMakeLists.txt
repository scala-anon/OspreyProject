# Find the required packages
find_package(Protobuf REQUIRED)
find_package(gRPC REQUIRED)

# Include directories
include_directories(${Protobuf_INCLUDE_DIRS})
include_directories(${gRPC_INCLUDE_DIRS})

set(SOURCES
    src/FileReader.cpp
    src/main.cpp
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})


add_executable(client ${SOURCES})

target_link_libraries(client
    PRIVATE
        ProtoFiles
)

target_include_directories(client PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/../proto)
