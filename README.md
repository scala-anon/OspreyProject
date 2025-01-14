# OspreyProject 

## Overview
This project is a C++ application that utilizes gRPC for communication. It requires a specific setup and dataset to build and run successfully.

---

## Prerequisites
To run and build this application, you will need the following:
- A **C++ compiler** (e.g., GCC, Clang)
- **CMake** (Minimum version 3.15 recommended)
- **gRPC** with C++ bindings

---

## Setup Instructions

### 1. Data Preparation
- You will need your own set of data files.
- Modify the `client/CMakeLists.txt` to ensure the application has access to your data files.

### 2. Build the Application
Follow these steps to build the application:
1. Create a build directory:
   ```bash
   mkdir build
   cd build
2. Configure the build using CMake:
   ```bash
   cmake ..
3. Build the project:
   ```bash
   make
4. Run the application:
   ```bash
   ./server
   ./client
