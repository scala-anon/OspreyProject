#include "H5Cpp.h"
#include <iostream>
#include <string>

void openHDF5File(const std::string &filename) {
  try {
    H5::H5File file(filename, H5F_ACC_RDONLY);
    std::cout << "Opened HDF5 file: " << filename << std::endl;
  } catch (const H5LLFileIException &e) {
  }
}

int main()
