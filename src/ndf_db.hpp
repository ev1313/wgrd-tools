#pragma once

#include "sqlite3.h"
#include "sqlite_helpers.hpp"
#include <filesystem>

#include "ndf.hpp"

namespace fs = std::filesystem;

class NDF_DB {
private:
  sqlite3 *db;
  // create table statements
  SQLStatement<0, 0> stmt_create_ndf_file_tbl;
  SQLStatement<0, 0> stmt_create_ndf_object_tbl;
  SQLStatement<0, 0> stmt_create_ndf_property_tbl;
  SQLStatement<0, 0> stmt_create_ndf_bool_tbl;
  SQLStatement<0, 0> stmt_create_ndf_int8_tbl;
  SQLStatement<0, 0> stmt_create_ndf_uint8_tbl;
  SQLStatement<0, 0> stmt_create_ndf_int16_tbl;
  SQLStatement<0, 0> stmt_create_ndf_uint16_tbl;
  SQLStatement<0, 0> stmt_create_ndf_int32_tbl;
  SQLStatement<0, 0> stmt_create_ndf_uint32_tbl;
  SQLStatement<0, 0> stmt_create_ndf_float32_tbl;
  SQLStatement<0, 0> stmt_create_ndf_float64_tbl;
  SQLStatement<0, 0> stmt_create_ndf_string_tbl;
  SQLStatement<0, 0> stmt_create_ndf_widestring_tbl;
  SQLStatement<0, 0> stmt_create_ndf_F32_vec2_tbl;
  SQLStatement<0, 0> stmt_create_ndf_F32_vec3_tbl;
  SQLStatement<0, 0> stmt_create_ndf_F32_vec4_tbl;
  SQLStatement<0, 0> stmt_create_ndf_S32_vec2_tbl;
  SQLStatement<0, 0> stmt_create_ndf_S32_vec3_tbl;
  SQLStatement<0, 0> stmt_create_ndf_S32_vec4_tbl;
  SQLStatement<0, 0> stmt_create_ndf_color_tbl;
  SQLStatement<0, 0> stmt_create_ndf_object_reference_tbl;
  SQLStatement<0, 0> stmt_create_ndf_import_reference_tbl;
  SQLStatement<0, 0> stmt_create_ndf_GUID_tbl;
  SQLStatement<0, 0> stmt_create_ndf_path_reference_tbl;
  SQLStatement<0, 0> stmt_create_ndf_localisation_hash_tbl;
  SQLStatement<0, 0> stmt_create_ndf_hash_tbl;
  // insertion statements
  SQLStatement<3, 0> stmt_insert_ndf_file;
  SQLStatement<5, 0> stmt_insert_ndf_object;
  SQLStatement<7, 0> stmt_insert_ndf_property;
  SQLStatement<1, 0> stmt_insert_ndf_bool;
  SQLStatement<1, 0> stmt_insert_ndf_int8;
  SQLStatement<1, 0> stmt_insert_ndf_uint8;
  SQLStatement<1, 0> stmt_insert_ndf_int16;
  SQLStatement<1, 0> stmt_insert_ndf_uint16;
  SQLStatement<1, 0> stmt_insert_ndf_int32;
  SQLStatement<1, 0> stmt_insert_ndf_uint32;
  SQLStatement<1, 0> stmt_insert_ndf_float32;
  SQLStatement<1, 0> stmt_insert_ndf_float64;
  SQLStatement<1, 0> stmt_insert_ndf_string;
  SQLStatement<1, 0> stmt_insert_ndf_widestring;
  SQLStatement<2, 0> stmt_insert_ndf_F32_vec2;
  SQLStatement<3, 0> stmt_insert_ndf_F32_vec3;
  SQLStatement<4, 0> stmt_insert_ndf_F32_vec4;
  SQLStatement<2, 0> stmt_insert_ndf_S32_vec2;
  SQLStatement<3, 0> stmt_insert_ndf_S32_vec3;
  SQLStatement<4, 0> stmt_insert_ndf_S32_vec4;
  SQLStatement<4, 0> stmt_insert_ndf_color;
  SQLStatement<1, 0> stmt_insert_ndf_object_reference;
  SQLStatement<1, 0> stmt_insert_ndf_import_reference;
  SQLStatement<1, 0> stmt_insert_ndf_GUID;
  SQLStatement<1, 0> stmt_insert_ndf_path_reference;
  SQLStatement<1, 0> stmt_insert_ndf_localisation_hash;
  SQLStatement<1, 0> stmt_insert_ndf_hash;
  // accessors for objects
  SQLStatement<1, 1> stmt_get_object_names;
  SQLStatement<1, 1> stmt_get_property_names;
  // accessors for values, should only return a single row each
  SQLStatement<1, 1> stmt_get_bool_value;
  SQLStatement<1, 1> stmt_get_int8_value;
  SQLStatement<1, 1> stmt_get_uint8_value;
  SQLStatement<1, 1> stmt_get_int16_value;
  SQLStatement<1, 1> stmt_get_uint16_value;
  SQLStatement<1, 1> stmt_get_int32_value;
  SQLStatement<1, 1> stmt_get_uint32_value;
  SQLStatement<1, 1> stmt_get_float32_value;
  SQLStatement<1, 1> stmt_get_float64_value;
  SQLStatement<1, 1> stmt_get_string_value;
  SQLStatement<1, 1> stmt_get_widestring_value;
  SQLStatement<1, 2> stmt_get_F32_vec2_value;
  SQLStatement<1, 3> stmt_get_F32_vec3_value;
  SQLStatement<1, 4> stmt_get_F32_vec4_value;
  SQLStatement<1, 2> stmt_get_S32_vec2_value;
  SQLStatement<1, 3> stmt_get_S32_vec3_value;
  SQLStatement<1, 4> stmt_get_S32_vec4_value;
  SQLStatement<1, 4> stmt_get_color_value;
  SQLStatement<1, 1> stmt_get_object_reference_value;
  SQLStatement<1, 1> stmt_get_import_reference_value;
  SQLStatement<1, 1> stmt_get_GUID_value;
  SQLStatement<1, 1> stmt_get_path_reference_value;
  SQLStatement<1, 1> stmt_get_localisation_hash_value;
  SQLStatement<1, 1> stmt_get_hash_value;
  // accessor used by lists, maps and pairs, returns all associated property ids
  // in order
  SQLStatement<1, 1> stmt_get_list_items;

  bool init_statements();

public:
  bool init();
  bool init(fs::path path);
  ~NDF_DB();

  bool insert_object(int ndf_idx, const NDFObject &object);
  bool insert_property(int object_idx, const NDFProperty &property,
                       int parent = -1, int position = -1);
};
