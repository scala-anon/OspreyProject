#
# Sources
#
set(SOURCES
    src/main.cpp
    ## src/DataProcessor.cpp # TODO: missing!
    src/FileReader.cpp
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})


#
# Target
#
add_executable(client ${SOURCES})

target_link_libraries(client
    PRIVATE
        myproto
)
target_include_directories(client PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
