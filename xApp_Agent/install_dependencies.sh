#!/bin/bash
echo "Downloading libtorch (C++ backend for PyTorch)..."

# Ensure wget and unzip are installed
if ! command -v wget &> /dev/null; then
    echo "Error: wget could not be found. Please install wget."
    exit 1
fi

if ! command -v unzip &> /dev/null; then
    echo "Error: unzip could not be found. Please install unzip."
    exit 1
fi

# Download libtorch for CPU (ABI shared)
wget -qO libtorch.zip https://download.pytorch.org/libtorch/cpu/libtorch-cxx11-abi-shared-with-deps-2.0.1%2Bcpu.zip

echo "Extracting libtorch..."
unzip -q libtorch.zip

# Clean up the zip file
rm libtorch.zip

echo "libtorch installed successfully in ./libtorch"
