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
  void fix_references(const std::string &old_name,
                      const std::string &new_name) {
    for (auto &prop : properties) {
      if (prop->is_list() || prop->is_map() || prop->is_pair() ||
          prop->is_object_reference()) {
        prop->fix_references(old_name, new_name);
      }
    }
  }
  void
  fix_references(const std::unordered_map<std::string, std::string> &renames) {
    for (auto &prop : properties) {
      if (prop->is_list() || prop->is_map() || prop->is_pair() ||
          prop->is_object_reference()) {
        prop->fix_references(renames);
      }
    }
  }
  std::unordered_set<std::string> get_object_references() {
    std::unordered_set<std::string> ret;
    for (auto const &prop : properties) {
      auto refs = prop->get_object_references();
      ret.insert(refs.begin(), refs.end());
    }
    return ret;
  }
  std::unordered_set<std::string> get_import_references() {
    std::unordered_set<std::string> ret;
    for (auto const &prop : properties) {
      auto refs = prop->get_import_references();
      ret.insert(refs.begin(), refs.end());
    }
    return ret;
  }
};

struct NDF {
private:
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
  friend class NDFPropertyObjectReference;
  friend class NDFPropertyImportReference;

public:
  NDFObject &get_object(const std::string &str) { return object_map.at(str); }

  bool change_object_name(const std::string &previous_name,
                          const std::string &name, bool fix_references = true) {
    if (object_map.contains(name)) {
      spdlog::warn("change_object_name: object {} does already exist", name);
      return false;
    }
    if (!object_map.contains(previous_name)) {
      spdlog::warn("change_object_name: object {} does not exist",
                   previous_name);
      return false;
    }
    auto &object = get_object(previous_name);
    const auto it = object_map.find(previous_name);
    object_map[name] = object.get_copy();
    object_map[name].name = name;
    object_map.erase(it);

    if (fix_references) {
      for (auto it = object_map.begin(); it != object_map.end(); ++it) {
        it.value().fix_references(previous_name, name);
      }
    }
    return true;
  }

  bool bulk_rename_objects(
      const std::unordered_map<std::string, std::string> &renames,
      bool fix_references = true) {
    for (const auto &[previous_name, name] : renames) {
      if (object_map.contains(name)) {
        spdlog::warn("change_object_name: object {} does already exist", name);
        return false;
      }
      if (!object_map.contains(previous_name)) {
        spdlog::warn("change_object_name: object {} does not exist",
                     previous_name);
        return false;
      }
      auto &object = get_object(previous_name);
      const auto it = object_map.find(previous_name);
      object_map[name] = object.get_copy();
      object_map[name].name = name;
      object_map.erase(it);
    }

    if (fix_references) {
      for (auto it = object_map.begin(); it != object_map.end(); ++it) {
        it.value().fix_references(renames);
      }
    }

    return true;
  }

  bool copy_object(const std::string &obj_name, const std::string &new_name) {
    if (object_map.contains(new_name)) {
      spdlog::warn("copy_object: object {} does already exist", new_name);
      return false;
    }
    if (!object_map.contains(obj_name)) {
      spdlog::warn("copy_object: object {} does not exist", obj_name);
      return false;
    }
    auto &object = get_object(obj_name);
    auto new_object = object.get_copy();
    new_object.name = new_name;
    object_map.insert({new_name, std::move(new_object)});
    return true;
  }

  bool remove_object(const std::string &name) {
    if (!object_map.contains(name)) {
      spdlog::warn("remove_object: object {} does not exist", name);
      return false;
    }
    object_map.erase(name);
    return true;
  }

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
};
