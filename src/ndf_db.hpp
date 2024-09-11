#pragma once

#include "ndf_properties.hpp"
#include "sqlite3.h"
#include "sqlite_helpers.hpp"
#include <filesystem>
#include <unordered_set>

#include "ndf.hpp"

namespace fs = std::filesystem;

#define ndf_property_simple_def(NAME, DATATYPE)                                \
  SQLStatement<1, 0> stmt_insert_ndf_##NAME;                                   \
  SQLStatement<1, 1> stmt_get_##NAME##_value;                                  \
  SQLStatement<2, 0> stmt_set_##NAME##_value;                                  \
  SQLStatement<1, 1> stmt_get_distinct_##NAME##_value;                         \
  SQLStatement<1, 0> stmt_copy_##NAME##_value;

#define ndf_property_vec2_def(NAME, DATATYPE)                                  \
  SQLStatement<2, 0> stmt_insert_ndf_##NAME;                                   \
  SQLStatement<1, 2> stmt_get_##NAME##_value;                                  \
  SQLStatement<3, 0> stmt_set_##NAME##_value;                                  \
  SQLStatement<1, 2> stmt_get_distinct_##NAME##_value;                         \
  SQLStatement<1, 0> stmt_copy_##NAME##_value;

#define ndf_property_vec3_def(NAME, DATATYPE)                                  \
  SQLStatement<3, 0> stmt_insert_ndf_##NAME;                                   \
  SQLStatement<1, 3> stmt_get_##NAME##_value;                                  \
  SQLStatement<4, 0> stmt_set_##NAME##_value;                                  \
  SQLStatement<1, 3> stmt_get_distinct_##NAME##_value;                         \
  SQLStatement<1, 0> stmt_copy_##NAME##_value;

#define ndf_property_vec4_def(NAME, DATATYPE)                                  \
  SQLStatement<4, 0> stmt_insert_ndf_##NAME;                                   \
  SQLStatement<1, 4> stmt_get_##NAME##_value;                                  \
  SQLStatement<5, 0> stmt_set_##NAME##_value;                                  \
  SQLStatement<1, 4> stmt_get_distinct_##NAME##_value;                         \
  SQLStatement<1, 0> stmt_copy_##NAME##_value;

#define ndf_property_reference_def(NAME, OBJECT_REFERENCE)                     \
  SQLStatement<2, 0> stmt_insert_ndf_##NAME;                                   \
  SQLStatement<1, 2> stmt_get_##NAME##_value;                                  \
  SQLStatement<2, 0> stmt_set_##NAME##_value;                                  \
  SQLStatement<1, 2> stmt_get_distinct_##NAME##_value;                         \
  SQLStatement<1, 0> stmt_copy_##NAME##_value;                                 \
  SQLStatement<OBJECT_REFERENCE, 0> stmt_update_##NAME##_value;                \
  SQLStatement<1, 1> stmt_get_referencing_##NAME##_value;

class NDF_DB {
private:
  sqlite3 *db = nullptr;
  size_t stash_ndf_id = 0;

  // class db statements
  SQLStatement<1, 0> stmt_insert_class;
  SQLStatement<3, 0> stmt_insert_class_property;
  SQLStatement<2, 1> stmt_get_class_properties;
  // NDF File
  SQLStatement<5, 0> stmt_insert_ndf_file;
  SQLStatement<2, 1> stmt_get_file_from_paths;
  SQLStatement<1, 0> stmt_delete_ndf_file;
  // NDF Object
  SQLStatement<5, 0> stmt_insert_ndf_object;
  SQLStatement<1, 1> stmt_get_object_from_name;
  SQLStatement<1, 1> stmt_get_object_from_export_path;
  SQLStatement<1, 1> stmt_get_object_ndf_id;
  SQLStatement<1, 5> stmt_get_object_full_ndf_id;
  SQLStatement<1, 1> stmt_get_object_name;
  SQLStatement<1, 1> stmt_get_object_names;
  SQLStatement<1, 1> stmt_get_object_ids_and_names_filtered;
  SQLStatement<1, 1> stmt_get_object_class_names;
  SQLStatement<1, 1> stmt_get_object_export_path;
  SQLStatement<1, 1> stmt_get_object_top_object;
  SQLStatement<1, 5> stmt_get_object;
  SQLStatement<2, 0> stmt_set_object_ndf_id;
  SQLStatement<2, 0> stmt_set_object_name;
  SQLStatement<2, 0> stmt_set_object_export_path;
  SQLStatement<2, 0> stmt_set_object_top_object;
  SQLStatement<1, 0> stmt_delete_ndf_object;
  // NDF Property
  SQLStatement<8, 0> stmt_insert_ndf_property;
  SQLStatement<1, 1> stmt_get_object_properties;
  SQLStatement<1, 1> stmt_get_property_names;
  SQLStatement<1, 8> stmt_get_property;
  // accessor used by lists, maps and pairs, returns all associated property ids
  // in order
  SQLStatement<1, 1> stmt_get_list_items;

  // simple properties
  ndf_property_simple_def(bool, BOOLEAN);
  ndf_property_simple_def(uint8, INTEGER);
  ndf_property_simple_def(int8, INTEGER);
  ndf_property_simple_def(uint16, INTEGER);
  ndf_property_simple_def(int16, INTEGER);
  ndf_property_simple_def(uint32, INTEGER);
  ndf_property_simple_def(int32, INTEGER);
  ndf_property_simple_def(float32, REAL);
  ndf_property_simple_def(float64, REAL);
  ndf_property_simple_def(string, TEXT);
  ndf_property_simple_def(widestring, TEXT);
  ndf_property_simple_def(path_reference, TEXT);
  ndf_property_simple_def(GUID, TEXT);
  ndf_property_simple_def(localisation_hash, TEXT);
  ndf_property_simple_def(hash, TEXT);

  ndf_property_vec2_def(F32_vec2, REAL);
  ndf_property_vec2_def(S32_vec2, INTEGER);
  ndf_property_vec3_def(F32_vec3, REAL);
  ndf_property_vec3_def(S32_vec3, INTEGER);
  ndf_property_vec4_def(F32_vec4, REAL);
  ndf_property_vec4_def(S32_vec4, INTEGER);
  ndf_property_vec4_def(color, INTEGER);

  ndf_property_reference_def(object_reference, 1);
  ndf_property_reference_def(import_reference, 0);

  bool init_statements();

  friend struct NDFProperty;
  friend struct NDFPropertyBool;
  friend struct NDFPropertyUInt8;
  friend struct NDFPropertyInt8;
  friend struct NDFPropertyUInt16;
  friend struct NDFPropertyInt16;
  friend struct NDFPropertyUInt32;
  friend struct NDFPropertyInt32;
  friend struct NDFPropertyFloat32;
  friend struct NDFPropertyFloat64;
  friend struct NDFPropertyString;
  friend struct NDFPropertyWideString;
  friend struct NDFPropertyF32_vec2;
  friend struct NDFPropertyF32_vec3;
  friend struct NDFPropertyF32_vec4;
  friend struct NDFPropertyS32_vec2;
  friend struct NDFPropertyS32_vec3;
  friend struct NDFPropertyColor;
  friend struct NDFPropertyImportReference;
  friend struct NDFPropertyObjectReference;
  friend struct NDFPropertyGUID;
  friend struct NDFPropertyPathReference;
  friend struct NDFPropertyLocalisationHash;
  friend struct NDFPropertyHash;
  friend struct NDFPropertyList;
  friend struct NDFPropertyMap;
  friend struct NDFPropertyPair;

public:
  sqlite3 *get_db() { return db; }
  bool init();
  bool init(fs::path path);
  bool is_initialized() const { return db != nullptr; }
  ~NDF_DB();

  bool create_table(std::string name, std::string query);

  std::optional<size_t> get_file(std::string vfs_path, std::string fs_path);
  std::optional<size_t> insert_file(std::string vfs_path, std::string dat_path,
                                    std::string fs_path, std::string version,
                                    bool is_current = true);
  bool delete_file(size_t ndf_file);

  std::optional<size_t> insert_object(NDFObject &object);
  bool insert_objects(std::vector<NDFObject> &objects);
  std::optional<NDFObject> get_object(size_t object_idx);
  std::optional<std::vector<std::string>> get_object_names(size_t ndf_id);
  std::optional<std::vector<std::tuple<size_t, std::string>>>
  get_object_ids_and_names_filtered(size_t ndf_id, std::string object_filter,
                                    std::string class_filter);
  std::optional<size_t> copy_object(size_t obj_id, std::string new_name);
  bool move_object(size_t obj_id, size_t new_ndf_id = 0);
  bool remove_object(size_t obj_id);
  bool change_object_name(size_t object_idx, std::string new_name);
  bool change_export_path(size_t object_idx, std::string new_path);
  bool change_is_top_object(size_t object_idx, bool is_top_object);

  // std::optional<std::vector<NDFObject>> get_objects(int ndf_idx);
  std::optional<std::unique_ptr<NDFProperty>> get_property(size_t property_idx);
  bool insert_property(NDFProperty &property);
  bool fix_references(size_t ndf_id);

  // faster accessors for initialization from and to ndfbin or ndf xml, as this
  // only calls a single insert call (so sqlite can properly bulk insert)
  std::optional<size_t> insert_only_object(size_t ndf_idx,
                                           const NDFObject &object);
  std::optional<size_t> insert_only_property(const NDFProperty &property);
  std::optional<std::vector<NDFObject>> get_only_objects(size_t ndf_idx);
  std::optional<std::vector<std::unique_ptr<NDFProperty>>>
  get_only_properties(size_t object_idx);
};
