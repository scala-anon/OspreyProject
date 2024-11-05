#!/bin/bash

# Directory paths - update these to your needs
PROTO_DIR="/home/nicholas/OspreyProject/proto"  # Directory where your .proto files are located
OUTPUT_DIR="/home/nicholas/OspreyProject/generatedfiles"  # Directory where you want the generated files to be placed
GRPC_PLUGIN_PATH="/usr/bin/grpc_cpp_plugin"  # Path to grpc_cpp_plugin executable

# Ensure the output directory exists
mkdir -p "$OUTPUT_DIR"

# Compile all .proto files in the specified directory
for proto_file in "$PROTO_DIR"/*.proto; do
    echo "Compiling $proto_file..."
    protoc --proto_path="$PROTO_DIR" \
           --cpp_out="$OUTPUT_DIR" \
           --grpc_out="$OUTPUT_DIR" \
           --plugin=protoc-gen-grpc="$GRPC_PLUGIN_PATH" \
           "$proto_file"
done

echo "Compilation finished!"

