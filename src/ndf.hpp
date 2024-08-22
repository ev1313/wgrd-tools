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

#include <filesystem>
namespace fs = std::filesystem;

using namespace std::literals;

struct NDF;

struct NDFClass {
  std::map<std::string, uint32_t> properties;
};

enum NDFPropertyType : uint32_t {
  Bool = 0x0,
  UInt8 = 0x1,
  Int32 = 0x2,
  UInt32 = 0x3,
  Float32 = 0x5,
  Float64 = 0x6,
  String = 0x7,
  WideString = 0x8,
  ObjectReference = 0x9,
  ImportReference = 0x9,
  F32_vec3 = 0xB,
  F32_vec4 = 0xC,
  Color = 0xD,
  S32_vec3 = 0xE,
  Matrix = 0xF,
  List = 0x11,
  Map = 0x12,
  Long = 0x13,
  Blob = 0x14,
  Int16 = 0x18,
  UInt16 = 0x19,
  NDFGUID = 0x1A,
  PathReference = 0x1C,
  LocalisationHash = 0x1D,
  S32_vec2 = 0x1F,
  F32_vec2 = 0x21,
  Pair = 0x22,
  Hash = 0x25
};

struct NDFProperty {
  uint32_t property_idx;
  uint32_t property_type;
  std::string property_name;
  NDFProperty() = default;
  virtual ~NDFProperty() = default;
  static std::unique_ptr<NDFProperty>
  get_property_from_ndftype(uint32_t ndf_type);
  static std::unique_ptr<NDFProperty>
  get_property_from_ndf_xml(uint32_t ndf_type, const pugi::xml_node &ndf_node);
  static std::unique_ptr<NDFProperty>
  get_property_from_ndfbin(uint32_t ndf_type, std::istream &stream);
  virtual void to_ndf_xml(pugi::xml_node &) {
    throw std::runtime_error("Not implemented");
  }
  virtual void from_ndf_xml(const pugi::xml_node &) {
    throw std::runtime_error("Not implemented");
  }
  virtual void from_ndfbin(NDF *, std::istream &) {
    throw std::runtime_error("Not implemented");
  }
  virtual void to_ndfbin(NDF *, std::ostream &) {
    throw std::runtime_error("Not implemented");
  }
  virtual bool is_object_reference() { return false; }
  virtual bool is_import_reference() { return false; }
  virtual bool is_list() { return false; }
  virtual bool is_map() { return false; }
  virtual bool is_pair() { return false; }
  virtual std::unique_ptr<NDFProperty> get_copy() = 0;
  virtual void fix_references(const std::string &, const std::string &) {}
  virtual void
  fix_references(const std::unordered_map<std::string, std::string> &) {}
  virtual std::string as_string() = 0;
  virtual std::unordered_set<std::string> get_object_references() { return {}; }
  virtual std::unordered_set<std::string> get_import_references() { return {}; }
};

struct NDFPropertyBool : NDFProperty {
  bool value;
  NDFPropertyBool() { property_type = NDFPropertyType::Bool; }

  void to_ndf_xml(pugi::xml_node &node) override;
  void from_ndf_xml(const pugi::xml_node &node) override;

  void from_ndfbin(NDF *, std::istream &stream) override;
  void to_ndfbin(NDF *, std::ostream &stream) override;

  std::unique_ptr<NDFProperty> get_copy() override {
    return std::make_unique<NDFPropertyBool>(*this);
  }
  std::string as_string() override { return value ? "true" : "false"; }
};

struct NDFPropertyUInt8 : NDFProperty {
  uint8_t value;
  NDFPropertyUInt8() { property_type = NDFPropertyType::UInt8; }
  void to_ndf_xml(pugi::xml_node &node) override;
  void from_ndf_xml(const pugi::xml_node &node) override;

  void from_ndfbin(NDF *, std::istream &stream) override;
  void to_ndfbin(NDF *, std::ostream &stream) override;

  std::unique_ptr<NDFProperty> get_copy() override {
    return std::make_unique<NDFPropertyUInt8>(*this);
  }
  std::string as_string() override { return std::to_string(value); }
};

struct NDFPropertyInt32 : NDFProperty {
  int32_t value;
  NDFPropertyInt32() { property_type = NDFPropertyType::Int32; }

  void to_ndf_xml(pugi::xml_node &node) override;
  void from_ndf_xml(const pugi::xml_node &node) override;

  void from_ndfbin(NDF *, std::istream &stream) override;
  void to_ndfbin(NDF *, std::ostream &stream) override;

  std::unique_ptr<NDFProperty> get_copy() override {
    return std::make_unique<NDFPropertyInt32>(*this);
  }
  std::string as_string() override { return std::to_string(value); }
};

struct NDFPropertyUInt32 : NDFProperty {
  uint32_t value;
  NDFPropertyUInt32() { property_type = NDFPropertyType::UInt32; }

  void to_ndf_xml(pugi::xml_node &node) override;
  void from_ndf_xml(const pugi::xml_node &node) override;

  void from_ndfbin(NDF *, std::istream &stream) override;
  void to_ndfbin(NDF *, std::ostream &stream) override;

  std::unique_ptr<NDFProperty> get_copy() override {
    return std::make_unique<NDFPropertyUInt32>(*this);
  }
  std::string as_string() override { return std::to_string(value); }
};

struct NDFPropertyFloat32 : NDFProperty {
  float value;
  NDFPropertyFloat32() { property_type = NDFPropertyType::Float32; }

  void to_ndf_xml(pugi::xml_node &node) override;
  void from_ndf_xml(const pugi::xml_node &node) override;

  void from_ndfbin(NDF *, std::istream &stream) override;
  void to_ndfbin(NDF *, std::ostream &stream) override;

  std::unique_ptr<NDFProperty> get_copy() override {
    return std::make_unique<NDFPropertyFloat32>(*this);
  }
  std::string as_string() override { return std::to_string(value); }
};

struct NDFPropertyFloat64 : NDFProperty {
  double value;
  NDFPropertyFloat64() { property_type = NDFPropertyType::Float64; }

  void to_ndf_xml(pugi::xml_node &node) override;
  void from_ndf_xml(const pugi::xml_node &node) override;

  void from_ndfbin(NDF *, std::istream &stream) override;
  void to_ndfbin(NDF *, std::ostream &stream) override;

  std::unique_ptr<NDFProperty> get_copy() override {
    return std::make_unique<NDFPropertyFloat64>(*this);
  }
  std::string as_string() override { return std::to_string(value); }
};

struct NDFPropertyString : NDFProperty {
  std::string value;
  NDFPropertyString() { property_type = NDFPropertyType::String; }

  void to_ndf_xml(pugi::xml_node &node) override;
  void from_ndf_xml(const pugi::xml_node &node) override;

public:
  void from_ndfbin(NDF *root, std::istream &stream) override;
  void to_ndfbin(NDF *root, std::ostream &stream) override;

public:
  std::unique_ptr<NDFProperty> get_copy() override {
    return std::make_unique<NDFPropertyString>(*this);
  }
  std::string as_string() override { return value; }
};

struct NDFPropertyWideString : NDFProperty {
  std::string value;
  NDFPropertyWideString() { property_type = NDFPropertyType::WideString; }
  void to_ndf_xml(pugi::xml_node &node) override;
  void from_ndf_xml(const pugi::xml_node &node) override;

  void from_ndfbin(NDF *root, std::istream &stream) override;
  void to_ndfbin(NDF *root, std::ostream &stream) override;

  std::unique_ptr<NDFProperty> get_copy() override {
    return std::make_unique<NDFPropertyWideString>(*this);
  }
  std::string as_string() override { return value; }
};

struct NDFPropertyF32_vec3 : NDFProperty {
  float x;
  float y;
  float z;
  NDFPropertyF32_vec3() { property_type = NDFPropertyType::F32_vec3; }

  void to_ndf_xml(pugi::xml_node &node) override;
  void from_ndf_xml(const pugi::xml_node &node) override;

  void from_ndfbin(NDF *, std::istream &stream) override;
  void to_ndfbin(NDF *, std::ostream &stream) override;

  std::unique_ptr<NDFProperty> get_copy() override {
    return std::make_unique<NDFPropertyF32_vec3>(*this);
  }
  std::string as_string() override {
    return fmt::format("({}, {}, {})", x, y, z);
  }
};

struct NDFPropertyF32_vec4 : NDFProperty {
  float x;
  float y;
  float z;
  float w;
  NDFPropertyF32_vec4() { property_type = NDFPropertyType::F32_vec4; }

  void to_ndf_xml(pugi::xml_node &node) override;
  void from_ndf_xml(const pugi::xml_node &node) override;

  void from_ndfbin(NDF *, std::istream &stream) override;
  void to_ndfbin(NDF *, std::ostream &stream) override;

  std::unique_ptr<NDFProperty> get_copy() override {
    return std::make_unique<NDFPropertyF32_vec4>(*this);
  }
  std::string as_string() override {
    return fmt::format("({}, {}, {}, {})", x, y, z, w);
  }
};

struct NDFPropertyColor : NDFProperty {
  uint8_t b;
  uint8_t g;
  uint8_t r;
  uint8_t a;
  NDFPropertyColor() { property_type = NDFPropertyType::Color; }

  void to_ndf_xml(pugi::xml_node &node) override;
  void from_ndf_xml(const pugi::xml_node &node) override;

  void from_ndfbin(NDF *, std::istream &stream) override;
  void to_ndfbin(NDF *, std::ostream &stream) override;

  std::unique_ptr<NDFProperty> get_copy() override {
    return std::make_unique<NDFPropertyColor>(*this);
  }
  std::string as_string() override {
    return fmt::format("({}, {}, {}, {})", r, g, b, a);
  }
};

struct NDFPropertyS32_vec3 : NDFProperty {
  int32_t x;
  int32_t y;
  int32_t z;
  NDFPropertyS32_vec3() { property_type = NDFPropertyType::S32_vec3; }

  void to_ndf_xml(pugi::xml_node &node) override;
  void from_ndf_xml(const pugi::xml_node &node) override;

  void from_ndfbin(NDF *, std::istream &stream) override;
  void to_ndfbin(NDF *, std::ostream &stream) override;

  std::unique_ptr<NDFProperty> get_copy() override {
    return std::make_unique<NDFPropertyS32_vec3>(*this);
  }
  std::string as_string() override {
    return fmt::format("({}, {}, {})", x, y, z);
  }
};

enum ReferenceType { Import = 2863311530, Object = 3149642683 };

struct NDFPropertyObjectReference : NDFProperty {
  std::string object_name;
  NDFPropertyObjectReference() {
    property_type = NDFPropertyType::ObjectReference;
  }

  void to_ndf_xml(pugi::xml_node &node) override;
  void from_ndf_xml(const pugi::xml_node &node) override;

  bool is_object_reference() override { return true; }
  void fix_references(const std::string &old_name,
                      const std::string &new_name) override {
    if (object_name == old_name) {
      object_name = new_name;
    }
  }
  void fix_references(
      const std::unordered_map<std::string, std::string> &renames) override {
    if (renames.contains(object_name)) {
      object_name = renames.at(object_name);
    }
  }
  std::unordered_set<std::string> get_object_references() override {
    return {object_name};
  }

  void from_ndfbin(NDF *root, std::istream &stream) override;
  void to_ndfbin(NDF *root, std::ostream &stream) override;

  std::unique_ptr<NDFProperty> get_copy() override {
    return std::make_unique<NDFPropertyObjectReference>(*this);
  }
  std::string as_string() override { return object_name; }
};

struct NDFPropertyImportReference : NDFProperty {
  std::string import_name;
  NDFPropertyImportReference() {
    property_type = NDFPropertyType::ImportReference;
  }

  void to_ndf_xml(pugi::xml_node &node) override;
  void from_ndf_xml(const pugi::xml_node &node) override;

  bool is_import_reference() override { return true; }
  std::unordered_set<std::string> get_import_references() override {
    return {import_name};
  }

  void from_ndfbin(NDF *root, std::istream &stream) override;
  void to_ndfbin(NDF *root, std::ostream &stream) override;

  std::unique_ptr<NDFProperty> get_copy() override {
    return std::make_unique<NDFPropertyImportReference>(*this);
  }
  std::string as_string() override { return import_name; }
};

struct NDFPropertyList : NDFProperty {
  std::vector<std::unique_ptr<NDFProperty>> values;
  NDFPropertyList() { property_type = NDFPropertyType::List; }

  void to_ndf_xml(pugi::xml_node &node) override;
  void from_ndf_xml(const pugi::xml_node &node) override;

  bool is_list() override { return true; }
  void fix_references(const std::string &old_name,
                      const std::string &new_name) override {
    for (auto &value : values) {
      value->fix_references(old_name, new_name);
    }
  }
  void fix_references(
      const std::unordered_map<std::string, std::string> &renames) override {
    for (auto &value : values) {
      value->fix_references(renames);
    }
  }
  std::unordered_set<std::string> get_object_references() override {
    std::unordered_set<std::string> ret;
    for (auto const &value : values) {
      auto value_refs = value->get_object_references();
      ret.insert(value_refs.begin(), value_refs.end());
    }
    return ret;
  }
  std::unordered_set<std::string> get_import_references() override {
    std::unordered_set<std::string> ret;
    for (auto const &value : values) {
      auto value_refs = value->get_import_references();
      ret.insert(value_refs.begin(), value_refs.end());
    }
    return ret;
  }

  void from_ndfbin(NDF *, std::istream &) override;
  void to_ndfbin(NDF *, std::ostream &) override;

  std::unique_ptr<NDFProperty> get_copy() override {
    auto ret = std::make_unique<NDFPropertyList>();
    for (auto const &value : values) {
      ret->values.push_back(value->get_copy());
      ret->property_name = property_name;
      ret->property_idx = property_idx;
      ret->property_type = property_type;
    }
    return ret;
  }
  std::string as_string() override {
    return "size " + std::to_string(values.size());
  }
};

struct NDFPropertyMap : NDFProperty {
  std::vector<
      std::pair<std::unique_ptr<NDFProperty>, std::unique_ptr<NDFProperty>>>
      values;
  NDFPropertyMap() { property_type = NDFPropertyType::Map; }

  void to_ndf_xml(pugi::xml_node &node) override;
  void from_ndf_xml(const pugi::xml_node &node) override;

  bool is_map() override { return true; }
  void fix_references(const std::string &old_name,
                      const std::string &new_name) override {
    for (auto const &[key, value] : values) {
      key->fix_references(old_name, new_name);
      value->fix_references(old_name, new_name);
    }
  }
  void fix_references(
      const std::unordered_map<std::string, std::string> &renames) override {
    for (auto const &[key, value] : values) {
      key->fix_references(renames);
      value->fix_references(renames);
    }
  }
  std::unordered_set<std::string> get_object_references() override {
    std::unordered_set<std::string> ret;
    for (auto const &[key, value] : values) {
      auto key_refs = key->get_object_references();
      ret.insert(key_refs.begin(), key_refs.end());
      auto value_refs = value->get_object_references();
      ret.insert(value_refs.begin(), value_refs.end());
    }
    return ret;
  }
  std::unordered_set<std::string> get_import_references() override {
    std::unordered_set<std::string> ret;
    for (auto const &[key, value] : values) {
      auto key_refs = key->get_import_references();
      ret.insert(key_refs.begin(), key_refs.end());
      auto value_refs = value->get_import_references();
      ret.insert(value_refs.begin(), value_refs.end());
    }
    return ret;
  }

  void from_ndfbin(NDF *, std::istream &) override;
  void to_ndfbin(NDF *, std::ostream &) override;

  std::unique_ptr<NDFProperty> get_copy() override {
    auto ret = std::make_unique<NDFPropertyMap>();
    for (auto const &[key, value] : values) {
      ret->values.push_back({key->get_copy(), value->get_copy()});
    }
    ret->property_name = property_name;
    ret->property_idx = property_idx;
    ret->property_type = property_type;
    return ret;
  }
  std::string as_string() override {
    return "size " + std::to_string(values.size());
  }
};

struct NDFPropertyInt16 : NDFProperty {
  int16_t value;
  NDFPropertyInt16() { property_type = NDFPropertyType::Int16; }

  void to_ndf_xml(pugi::xml_node &node) override;
  void from_ndf_xml(const pugi::xml_node &node) override;

  void from_ndfbin(NDF *, std::istream &stream) override;
  void to_ndfbin(NDF *, std::ostream &stream) override;

  std::unique_ptr<NDFProperty> get_copy() override {
    return std::make_unique<NDFPropertyInt16>(*this);
  }
  std::string as_string() override { return std::to_string(value); }
};

struct NDFPropertyUInt16 : NDFProperty {
  uint16_t value;
  NDFPropertyUInt16() { property_type = NDFPropertyType::UInt16; }

  void to_ndf_xml(pugi::xml_node &node) override;
  void from_ndf_xml(const pugi::xml_node &node) override;

  void from_ndfbin(NDF *, std::istream &stream) override;
  void to_ndfbin(NDF *, std::ostream &stream) override;

  std::unique_ptr<NDFProperty> get_copy() override {
    return std::make_unique<NDFPropertyUInt16>(*this);
  }
  std::string as_string() override { return std::to_string(value); }
};

// FIXME: error checking for GUIDs?
struct NDFPropertyGUID : NDFProperty {
  std::string guid;
  NDFPropertyGUID() { property_type = NDFPropertyType::NDFGUID; }

  void to_ndf_xml(pugi::xml_node &node) override;
  void from_ndf_xml(const pugi::xml_node &node) override;

  void from_ndfbin(NDF *, std::istream &stream) override;
  void to_ndfbin(NDF *, std::ostream &stream) override;

  std::unique_ptr<NDFProperty> get_copy() override {
    return std::make_unique<NDFPropertyGUID>(*this);
  }
  std::string as_string() override { return guid; }
};

struct NDFPropertyPathReference : NDFProperty {
  std::string path;
  NDFPropertyPathReference() { property_type = NDFPropertyType::PathReference; }

  void to_ndf_xml(pugi::xml_node &node) override;
  void from_ndf_xml(const pugi::xml_node &node) override;

  void from_ndfbin(NDF *, std::istream &) override;
  void to_ndfbin(NDF *, std::ostream &) override;

  std::unique_ptr<NDFProperty> get_copy() override {
    return std::make_unique<NDFPropertyPathReference>(*this);
  }
  std::string as_string() override { return path; }
};

// FIXME: error checking for hashes?
struct NDFPropertyLocalisationHash : NDFProperty {
  std::string hash;
  NDFPropertyLocalisationHash() {
    property_type = NDFPropertyType::LocalisationHash;
  }

  void to_ndf_xml(pugi::xml_node &node) override;
  void from_ndf_xml(const pugi::xml_node &node) override;

  void from_ndfbin(NDF *, std::istream &stream) override;
  void to_ndfbin(NDF *, std::ostream &stream) override;

  std::unique_ptr<NDFProperty> get_copy() override {
    return std::make_unique<NDFPropertyLocalisationHash>(*this);
  }
  std::string as_string() override { return hash; }
};

struct NDFPropertyS32_vec2 : NDFProperty {
  int32_t x;
  int32_t y;
  NDFPropertyS32_vec2() { property_type = NDFPropertyType::S32_vec2; }

  void to_ndf_xml(pugi::xml_node &node) override;
  void from_ndf_xml(const pugi::xml_node &node) override;

  void from_ndfbin(NDF *, std::istream &stream) override;
  void to_ndfbin(NDF *, std::ostream &stream) override;

  std::unique_ptr<NDFProperty> get_copy() override {
    return std::make_unique<NDFPropertyS32_vec2>(*this);
  }
  std::string as_string() override { return fmt::format("({}, {})", x, y); }
};

struct NDFPropertyF32_vec2 : NDFProperty {
  float x;
  float y;
  NDFPropertyF32_vec2() { property_type = NDFPropertyType::F32_vec2; }

  void to_ndf_xml(pugi::xml_node &node) override;
  void from_ndf_xml(const pugi::xml_node &node) override;

  void from_ndfbin(NDF *, std::istream &stream) override;
  void to_ndfbin(NDF *, std::ostream &stream) override;

  std::unique_ptr<NDFProperty> get_copy() override {
    return std::make_unique<NDFPropertyF32_vec2>(*this);
  }
  std::string as_string() override { return fmt::format("({}, {})", x, y); }
};

struct NDFPropertyPair : NDFProperty {
  std::unique_ptr<NDFProperty> first;
  std::unique_ptr<NDFProperty> second;
  NDFPropertyPair() { property_type = NDFPropertyType::Pair; }

  void to_ndf_xml(pugi::xml_node &node) override;
  void from_ndf_xml(const pugi::xml_node &node) override;

  bool is_pair() override { return true; }
  void fix_references(const std::string &old_name,
                      const std::string &new_name) override {
    first->fix_references(old_name, new_name);
    second->fix_references(old_name, new_name);
  }
  void fix_references(
      const std::unordered_map<std::string, std::string> &renames) override {
    first->fix_references(renames);
    second->fix_references(renames);
  }
  std::unordered_set<std::string> get_object_references() override {
    std::unordered_set<std::string> ret;
    auto first_refs = first->get_object_references();
    ret.insert(first_refs.begin(), first_refs.end());
    auto second_refs = second->get_object_references();
    ret.insert(second_refs.begin(), second_refs.end());
    return ret;
  }
  std::unordered_set<std::string> get_import_references() override {
    std::unordered_set<std::string> ret;
    auto first_refs = first->get_import_references();
    ret.insert(first_refs.begin(), first_refs.end());
    auto second_refs = second->get_import_references();
    ret.insert(second_refs.begin(), second_refs.end());
    return ret;
  }

  void from_ndfbin(NDF *, std::istream &) override;
  void to_ndfbin(NDF *, std::ostream &) override;

  std::unique_ptr<NDFProperty> get_copy() override {
    auto ret = std::make_unique<NDFPropertyPair>();
    ret->first = first->get_copy();
    ret->second = second->get_copy();
    ret->property_name = property_name;
    ret->property_idx = property_idx;
    ret->property_type = property_type;
    return ret;
  }
  std::string as_string() override {
    return fmt::format("({}, {})", first->as_string(), second->as_string());
  }
};

// FIXME: error checking for hashes?
struct NDFPropertyHash : NDFProperty {
  std::string hash;
  NDFPropertyHash() { property_type = NDFPropertyType::Hash; }

  void to_ndf_xml(pugi::xml_node &node) override;
  void from_ndf_xml(const pugi::xml_node &node) override;

  void from_ndfbin(NDF *, std::istream &stream) override;
  void to_ndfbin(NDF *, std::ostream &stream) override;

  std::unique_ptr<NDFProperty> get_copy() override {
    return std::make_unique<NDFPropertyHash>(*this);
  }
  std::string as_string() override { return hash; }
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
