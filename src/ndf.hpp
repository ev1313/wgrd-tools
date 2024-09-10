#pragma once

#include <cassert>
#include <map>
#include <ranges>
#include <set>
#include <unordered_set>
#include <vector>

#include "tsl/ordered_map.h"

#include "spdlog/spdlog.h"

#include "pugixml.hpp"

#include "ndf_properties.hpp"

#include <filesystem>
namespace fs = std::filesystem;

using namespace std::literals;

struct NDF;

struct NDFClass {
  std::map<std::string, uint32_t> properties;
};

struct NDFObject {
  std::string name;
  std::string class_name;
  bool is_top_object;
  std::string export_path;
  std::vector<std::unique_ptr<NDFProperty>> properties;
  std::map<std::string, uint32_t> property_map;
  std::unique_ptr<NDFProperty> &get_property(const std::string &name) {
    return properties.at(property_map.at(name));
  }

  // DB
  size_t db_id = 0;
  size_t db_ndf_id = 0;

public:
  NDFObject get_copy() {
    NDFObject ret;
    ret.name = name;
    ret.class_name = class_name;
    ret.is_top_object = is_top_object;
    ret.export_path = export_path;
    for (auto const &prop : properties) {
      ret.property_map.insert({prop->property_name, ret.properties.size()});
      ret.properties.push_back(prop->get_copy());
    }
    return ret;
  }
  void add_property(std::unique_ptr<NDFProperty> property) {
    property_map.insert({property->property_name, properties.size()});
    properties.push_back(std::move(property));
  }
};

struct NDF {
private:
  NDF_DB *db = nullptr;
  size_t ndf_id = 0;
  std::unordered_map<uint32_t, std::vector<NDFProperty *>> db_property_map;

public:
  std::map<unsigned int, std::string> import_name_table;
  std::vector<std::string> string_table;
  std::vector<std::string> class_table;
  std::vector<std::pair<std::string, uint32_t>> property_table;
  std::vector<std::string> tran_table;
  tsl::ordered_map<std::string, NDFObject> object_map;

  void save_as_ndf_xml(fs::path path);
  void load_imprs(std::istream &stream,
                  std::vector<std::string> current_import_path);
  void load_exprs(std::istream &stream,
                  std::vector<std::string> current_export_path);
  void load_from_ndf_xml(fs::path path);

  void add_object(NDFObject object) {
    object_map.insert({object.name, std::move(object)});
  }

  // used for saving to the db
  void insert_into_db(NDF_DB *db, size_t ndf_id);
  // used for initializing from the db (e.g. when exporting to ndfbin oder ndf
  // xml)
  void load_from_db(NDF_DB *db, size_t ndf_id);
  // used for just using an existing db w/o initializing anything
  void set_db(NDF_DB *db, size_t ndf_id) {
    this->db = db;
    this->ndf_id = ndf_id;
  }

private:
  std::vector<std::string> gen_object_items;
  std::map<std::string, uint32_t> gen_object_table;

  std::vector<std::string> gen_string_items;
  std::map<std::string, uint32_t> gen_string_table;

  std::map<std::string, uint32_t> gen_clas_items;
  std::vector<std::string> gen_clas_table;

  std::vector<uint32_t> gen_topo_table;

  std::map<std::string, uint32_t> gen_tran_items;
  std::vector<std::string> gen_tran_table;

  std::map<std::vector<uint32_t>, uint32_t> gen_import_table;
  std::vector<std::vector<uint32_t>> gen_import_items;
  std::map<std::vector<uint32_t>, uint32_t> gen_export_table;
  std::vector<std::vector<uint32_t>> gen_export_items;

  std::vector<NDFClass> gen_property_table;
  std::vector<std::pair<std::string, uint32_t>> gen_property_items;
  std::set<std::pair<std::string, uint32_t>> gen_property_set;

  void
  save_ndfbin_imprs(const std::map<std::vector<uint32_t>, uint32_t> &gen_table,
                    std::ostream &stream);

  // gets called by every save_as_ndfbin
  void fill_gen_object() {
    for (const auto &[idx, it] : object_map | std::views::enumerate) {
      gen_object_items.push_back(it.second.name);
      gen_object_table.insert({it.second.name, gen_object_items.size() - 1});
    }
  }

  // used for object reference
  uint32_t get_object_index(const std::string &name) {
    auto obj_idx = gen_object_table.find(name);
    if (obj_idx == gen_object_table.end()) {
      return 4294967295;
    }
    return obj_idx->second;
  }
  // used for object references
  uint32_t get_class_of_object(const std::string &name) {
    auto object_idx = get_object_index(name);
    if (object_idx == 4294967295) {
      return 4294967295;
    }
    return get_class(get_object(name).class_name);
  }

  uint32_t get_class(const std::string &str) {
    auto obj_idx = gen_clas_items.find(str);
    if (obj_idx == gen_clas_items.end()) {
      return 4294967295;
    }
    return obj_idx->second;
  }
  friend struct NDFPropertyObjectReference;
  friend struct NDFPropertyImportReference;

public:
  NDFObject &get_object(const std::string &str) { return object_map.at(str); }

private:
  uint32_t get_or_add_string(const std::string &str) {
    auto it = gen_string_table.find(str);
    if (it == gen_string_table.end()) {
      gen_string_items.push_back(str);
      gen_string_table.insert({str, gen_string_items.size() - 1});
      return gen_string_items.size() - 1;
    } else {
      return it->second;
    }
  }

  uint32_t get_or_add_tran(const std::string &str) {
    auto it = gen_tran_items.find(str);
    if (it == gen_tran_items.end()) {
      gen_tran_table.push_back(str);
      gen_tran_items.insert({str, gen_tran_table.size() - 1});
      return gen_tran_table.size() - 1;
    } else {
      return it->second;
    }
  }

  uint32_t get_or_add_impr_indices(const std::vector<uint32_t> &vec) {
    auto it = gen_import_table.find(vec);
    if (it == gen_import_table.end()) {
      gen_import_items.push_back(vec);
      gen_import_table.insert({vec, gen_import_items.size() - 1});
      return gen_import_items.size() - 1;
    } else {
      return it->second;
    }
  }

  uint32_t get_or_add_impr(std::string impr) {
    std::vector<uint32_t> vec;
    for (auto str : std::views::split(impr, '/')) {
      std::string foo = std::string(str.begin(), str.end());
      vec.push_back(get_or_add_tran(foo));
    }
    return get_or_add_impr_indices(vec);
  }

  uint32_t get_or_add_expr_indices(const std::vector<uint32_t> &vec,
                                   uint32_t object_idx) {
    auto it = gen_export_table.find(vec);
    if (it == gen_export_table.end()) {
      gen_export_items.push_back(vec);
      gen_export_table.insert({vec, object_idx});
      return gen_export_items.size() - 1;
    } else {
      return it->second;
    }
  }

  uint32_t get_or_add_expr(std::string expr, uint32_t object_idx) {
    std::vector<uint32_t> vec;
    for (auto str : std::views::split(expr, '/')) {
      std::string foo = std::string(str.begin(), str.end());
      vec.push_back(get_or_add_tran(foo));
    }
    return get_or_add_expr_indices(vec, object_idx);
  }
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
  void load_from_ndfbin_stream(std::istream &stream);
  void load_from_ndfbin(fs::path path);
  void save_as_ndfbin_stream(std::ostream &stream);
  void save_as_ndfbin(fs::path);

  void clear() {
    import_name_table.clear();
    string_table.clear();
    class_table.clear();
    property_table.clear();
    tran_table.clear();
    object_map.clear();
    gen_object_items.clear();
    gen_string_items.clear();
    gen_string_table.clear();
    gen_clas_items.clear();
    gen_clas_table.clear();
    gen_tran_items.clear();
    gen_tran_table.clear();
    gen_import_table.clear();
    gen_import_items.clear();
    gen_export_table.clear();
    gen_export_items.clear();
    gen_property_table.clear();
  }
  // db accessors
public:
  std::optional<NDFObject> get_object(size_t id);
  std::optional<std::unique_ptr<NDFProperty>> get_property(size_t id);
  std::optional<size_t> copy_object(size_t id);
  void remove_object(size_t object_id, bool permanently = false);
  void restore_object(size_t object_id);
  void change_object_name(size_t object_id, std::string name);
  void change_export_path(size_t object_id, std::string export_path);
  void change_top_object(size_t object_id, bool top_object);
};
