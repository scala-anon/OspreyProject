#
# Sources
#
set(SOURCES
    src/main.cpp
    src/ingest_client.cpp
    src/PacketParser.cpp
)

#
# Target
#
add_executable(client ${SOURCES})
target_link_libraries(client
    PRIVATE
        myproto
)

target_include_directories(client PRIVATE ${CMAKE_CURRENT_BINARY_DIR})


# Define the relative path to the specific data file
set(DATA_FILE "../../OspreyData/mic1-8-20240511/mic1-8-CH17-20240511-121442.dat")

# Copy the data file to the build directory
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/${DATA_FILE}
               ${CMAKE_CURRENT_BINARY_DIR}/data/mic1-8-CH17-20240511-121442.dat COPYONLY)

# Optionally install the data file
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/${DATA_FILE}
        DESTINATION share/myproject/data)

