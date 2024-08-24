#pragma once

#include "ndf.hpp"

#include <filesystem>
namespace fs = std::filesystem;

namespace ndf_generator {

void add_random_object(NDF &ndf);
void add_random_uint8(NDFObject &obj);
void add_random_uint16(NDFObject &obj);
void add_random_uint32(NDFObject &obj);

void create_edat(fs::path path,
                 std::unordered_map<std::string, std::string> files);

void create_test_files(fs::path output_folder);
} // namespace ndf_generator
