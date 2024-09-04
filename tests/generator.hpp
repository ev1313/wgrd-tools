#pragma once

#include "ndf.hpp"

#include <filesystem>
#include <memory>
namespace fs = std::filesystem;

namespace ndf_generator {

NDFObject gen_random_object(int id = 0);
void add_random_object(NDF &ndf);
void add_random_objects(NDF &ndf, size_t size);
std::unique_ptr<NDFProperty> gen_random_string(int idx);
void add_random_string(NDFObject &obj);
std::unique_ptr<NDFProperty> gen_random_uint8(int idx);
void add_random_uint8(NDFObject &obj);
std::unique_ptr<NDFProperty> gen_random_uint16(int idx);
void add_random_uint16(NDFObject &obj);
std::unique_ptr<NDFProperty> gen_random_uint32(int idx);
void add_random_uint32(NDFObject &obj);
std::unique_ptr<NDFProperty> gen_random_int32(int idx);
void add_random_int32(NDFObject &obj);
std::unique_ptr<NDFProperty> gen_random_list(int idx);
void add_random_list(NDFObject &obj);
std::unique_ptr<NDFProperty> gen_random_map(int idx);
void add_random_map(NDFObject &obj);
std::unique_ptr<NDFProperty> gen_object_reference(int idx, std::string ref);
void add_object_reference(NDFObject &obj, std::string ref);
std::unique_ptr<NDFProperty> gen_import_reference(int idx, std::string ref);
void add_import_reference(NDFObject &obj, std::string ref);

std::unique_ptr<NDFProperty> gen_random_property(int idx);
void add_random_property(NDFObject &obj);
std::vector<std::unique_ptr<NDFProperty>> gen_random_properties(int idx,
                                                                int count);
void add_random_properties(NDFObject &obj, int count);

void create_edat(fs::path path,
                 std::unordered_map<std::string, std::string> files);

void create_test_files(fs::path output_folder);
} // namespace ndf_generator
