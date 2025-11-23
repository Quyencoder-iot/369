#!/bin/bash

# Build script for Qt Model-View-Delegate Tutorial

echo "Building Qt Model-View-Delegate Tutorial Demo..."

# Create build directory
mkdir -p build
cd build

# Run CMake
echo "Running CMake..."
cmake ../src

if [ $? -ne 0 ]; then
    echo "CMake configuration failed!"
    exit 1
fi

# Build
echo "Building project..."
make -j$(nproc)

if [ $? -ne 0 ]; then
    echo "Build failed!"
    exit 1
fi

echo "Build completed successfully!"
echo "Run the demo with: ./build/model_view_demo"