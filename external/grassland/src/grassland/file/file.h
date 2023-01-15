#pragma once
#include <fstream>
#include <vector>

namespace grassland::file {
std::string ReadFile(const char *file_path);
std::vector<uint8_t> ReadFileBinary(const char *file_path);
}  // namespace grassland::file
