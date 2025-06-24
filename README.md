# OspreyProject(MLDP Backend) (C++)

**Author**: Nicholas Mamais  
**Affiliation**: SLAC National Accelerator Laboratory  
**Project**: Machine Learning Data Platform (MLDP) Backend Integration  
**Language**: C++17  
**Build System**: CMake  
**Dependencies**: gRPC, Protocol Buffers, MongoDB

---

## 🔍 Overview

This repository contains a modular C++ client designed to ingest structured data into the Osprey Machine Learning Data Platform (MLDP). The backend is optimized for reuse across labs and supports ingestion, annotation, and query workflows via gRPC and Protocol Buffers.

---

## 📁 Directory Structure

📦 OspreyProject/
├── 📁 client/                    # Client-side source logic
│   ├── CMakeLists.txt            # Client-specific build file
│   └── 📁 src/
│       ├── main.cpp              # Application entry point
│       ├── ingest_client.cpp     # Ingestion RPC client
│       ├── annotate_client.cpp   # Annotation RPC client
│       ├── query_client.cpp      # Query RPC client
│       ├── ingest_client.hpp     # Client interface header
│       ├── PacketParser.cpp      # Binary .dat file parser
│       └── PacketParser.h        # Parser header
│
├── 📁 proto/                     # Protobuf and gRPC interface definitions
│   ├── CMakeLists.txt            # Proto-specific CMake config
│   ├── common.proto              # Shared types (timestamps, data, etc.)
│   └── ingestion.proto           # Ingestion service interface
│
├── 📁 build/   (🛑 *ignored*)     # CMake build output — excluded via .gitignore
│   └── ...                       # Contains compiled binaries, objects, and generated files
│
├── CMakeLists.txt               # Top-level CMake configuration
├── README.md                    # Project documentation
└── .gitignore                   # Excludes build/ and other artifacts


---

## ⚙️ Dependencies

Make sure the following are installed:

```bash
sudo apt install \
  libprotobuf-dev \
  libgrpc++-dev \
  protobuf-compiler-grpc \
  cmake \
  g++  # Or clang++


🛠️ Build Instructions

Follow these steps to configure and build the project:

# Alter location for data 
cd /client/src
nano main.cpp
alter PacketParser parser("data/mic1-8-CH17-20240511-121442.dat");

# Future version will include cleaner data choice logic

# Create and enter a build directory
mkdir build && cd build

# Run CMake to configure the project
cmake ..

# Build the project using all available cores
make -j$(nproc)

