#
# Protobuf/Grpc source files
#


#
# Add Library target with protobuf sources
#
add_library(proto 
    common.proto
    ingestion.proto)

target_link_libraries(proto
    PUBLIC
        protobuf::libprotobuf
        gRPC::grpc
        gRPC::grpc++
)
target_include_directories(proto PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

#
# Compile protobuf and grpc files in myproto target to cpp
#
get_target_property(grpc_cpp_plugin_location gRPC::grpc_cpp_plugin LOCATION)

protobuf_generate(TARGET proto LANGUAGE cpp APPEND_PATH)
protobuf_generate(TARGET proto LANGUAGE grpc APPEND_PATH GENERATE_EXTENSIONS .grpc.pb.h .grpc.pb.cc PLUGIN "protoc-gen-grpc=${grpc_cpp_plugin_location}")

