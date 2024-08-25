#pragma once

#include "spdlog/spdlog.h"
#include <memory>
#include <pugixml.hpp>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

class NDF;

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

class NDF_DB;

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
  get_property_from_ndf_db(uint32_t ndf_type, bool is_import_reference);
  static std::unique_ptr<NDFProperty>
  get_property_from_ndfbin(uint32_t ndf_type, std::istream &stream);
  virtual void to_ndf_xml(pugi::xml_node &) const {
    throw std::runtime_error("Not implemented");
  }
  virtual void from_ndf_xml(const pugi::xml_node &) {
    throw std::runtime_error("Not implemented");
  }
  virtual void from_ndfbin(NDF *, std::istream &) {
    throw std::runtime_error("Not implemented");
  }
  virtual void to_ndfbin(NDF *, std::ostream &) const {
    throw std::runtime_error("Not implemented");
  }
  virtual bool to_ndf_db(NDF_DB *, int, int = -1, int = -1) const {
    throw std::runtime_error("Not implemented");
  }
  virtual bool from_ndf_db(NDF_DB *, int) {
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

  // used by ndf_db
  int get_db_property_value(NDF_DB *db, int property_id);
  static std::unique_ptr<NDFProperty>
  get_db_property_type(NDF_DB *db, int prop_id, int pos = -1);
  std::optional<int> add_db_property(NDF_DB *db, int object_id, int parent,
                                     int position, int value_id,
                                     bool is_import_reference = false) const;
};

struct NDFPropertyBool : NDFProperty {
  bool value;
  NDFPropertyBool() { property_type = NDFPropertyType::Bool; }

  void to_ndf_xml(pugi::xml_node &node) const override;
  void from_ndf_xml(const pugi::xml_node &node) override;

  void from_ndfbin(NDF *, std::istream &stream) override;
  void to_ndfbin(NDF *, std::ostream &stream) const override;

  bool from_ndf_db(NDF_DB *db, int property_id) override;
  bool to_ndf_db(NDF_DB *db, int object_id, int parent = -1,
                 int position = -1) const override;

  std::unique_ptr<NDFProperty> get_copy() override {
    return std::make_unique<NDFPropertyBool>(*this);
  }
  std::string as_string() override { return value ? "true" : "false"; }
};

struct NDFPropertyUInt8 : NDFProperty {
  uint8_t value;
  NDFPropertyUInt8() { property_type = NDFPropertyType::UInt8; }
  void to_ndf_xml(pugi::xml_node &node) const override;
  void from_ndf_xml(const pugi::xml_node &node) override;

  void from_ndfbin(NDF *, std::istream &stream) override;
  void to_ndfbin(NDF *, std::ostream &stream) const override;

  bool from_ndf_db(NDF_DB *db, int property_id) override;
  bool to_ndf_db(NDF_DB *db, int object_id, int parent = -1,
                 int position = -1) const override;

  std::unique_ptr<NDFProperty> get_copy() override {
    return std::make_unique<NDFPropertyUInt8>(*this);
  }
  std::string as_string() override { return std::to_string(value); }
};

struct NDFPropertyInt32 : NDFProperty {
  int32_t value;
  NDFPropertyInt32() { property_type = NDFPropertyType::Int32; }

  void to_ndf_xml(pugi::xml_node &node) const override;
  void from_ndf_xml(const pugi::xml_node &node) override;

  void from_ndfbin(NDF *, std::istream &stream) override;
  void to_ndfbin(NDF *, std::ostream &stream) const override;

  bool from_ndf_db(NDF_DB *db, int property_id) override;
  bool to_ndf_db(NDF_DB *db, int object_id, int parent = -1,
                 int position = -1) const override;

  std::unique_ptr<NDFProperty> get_copy() override {
    return std::make_unique<NDFPropertyInt32>(*this);
  }
  std::string as_string() override { return std::to_string(value); }
};

struct NDFPropertyUInt32 : NDFProperty {
  uint32_t value;
  NDFPropertyUInt32() { property_type = NDFPropertyType::UInt32; }

  void to_ndf_xml(pugi::xml_node &node) const override;
  void from_ndf_xml(const pugi::xml_node &node) override;

  void from_ndfbin(NDF *, std::istream &stream) override;
  void to_ndfbin(NDF *, std::ostream &stream) const override;

  bool from_ndf_db(NDF_DB *db, int property_id) override;
  bool to_ndf_db(NDF_DB *db, int object_id, int parent = -1,
                 int position = -1) const override;

  std::unique_ptr<NDFProperty> get_copy() override {
    return std::make_unique<NDFPropertyUInt32>(*this);
  }
  std::string as_string() override { return std::to_string(value); }
};

struct NDFPropertyFloat32 : NDFProperty {
  float value;
  NDFPropertyFloat32() { property_type = NDFPropertyType::Float32; }

  void to_ndf_xml(pugi::xml_node &node) const override;
  void from_ndf_xml(const pugi::xml_node &node) override;

  void from_ndfbin(NDF *, std::istream &stream) override;
  void to_ndfbin(NDF *, std::ostream &stream) const override;

  bool from_ndf_db(NDF_DB *db, int property_id) override;
  bool to_ndf_db(NDF_DB *db, int object_id, int parent = -1,
                 int position = -1) const override;

  std::unique_ptr<NDFProperty> get_copy() override {
    return std::make_unique<NDFPropertyFloat32>(*this);
  }
  std::string as_string() override { return std::to_string(value); }
};

struct NDFPropertyFloat64 : NDFProperty {
  double value;
  NDFPropertyFloat64() { property_type = NDFPropertyType::Float64; }

  void to_ndf_xml(pugi::xml_node &node) const override;
  void from_ndf_xml(const pugi::xml_node &node) override;

  void from_ndfbin(NDF *, std::istream &stream) override;
  void to_ndfbin(NDF *, std::ostream &stream) const override;

  bool from_ndf_db(NDF_DB *db, int property_id) override;
  bool to_ndf_db(NDF_DB *db, int object_id, int parent = -1,
                 int position = -1) const override;

  std::unique_ptr<NDFProperty> get_copy() override {
    return std::make_unique<NDFPropertyFloat64>(*this);
  }
  std::string as_string() override { return std::to_string(value); }
};

struct NDFPropertyString : NDFProperty {
  std::string value;
  NDFPropertyString() { property_type = NDFPropertyType::String; }

  void to_ndf_xml(pugi::xml_node &node) const override;
  void from_ndf_xml(const pugi::xml_node &node) override;

  void from_ndfbin(NDF *root, std::istream &stream) override;
  void to_ndfbin(NDF *root, std::ostream &stream) const override;

  bool from_ndf_db(NDF_DB *db, int property_id) override;
  bool to_ndf_db(NDF_DB *db, int object_id, int parent = -1,
                 int position = -1) const override;

  std::unique_ptr<NDFProperty> get_copy() override {
    return std::make_unique<NDFPropertyString>(*this);
  }
  std::string as_string() override { return value; }
};

struct NDFPropertyWideString : NDFProperty {
  std::string value;
  NDFPropertyWideString() { property_type = NDFPropertyType::WideString; }
  void to_ndf_xml(pugi::xml_node &node) const override;
  void from_ndf_xml(const pugi::xml_node &node) override;

  void from_ndfbin(NDF *root, std::istream &stream) override;
  void to_ndfbin(NDF *root, std::ostream &stream) const override;

  bool from_ndf_db(NDF_DB *db, int property_id) override;
  bool to_ndf_db(NDF_DB *db, int object_id, int parent = -1,
                 int position = -1) const override;

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

  void to_ndf_xml(pugi::xml_node &node) const override;
  void from_ndf_xml(const pugi::xml_node &node) override;

  void from_ndfbin(NDF *, std::istream &stream) override;
  void to_ndfbin(NDF *, std::ostream &stream) const override;

  bool from_ndf_db(NDF_DB *db, int property_id) override;
  bool to_ndf_db(NDF_DB *db, int object_id, int parent = -1,
                 int position = -1) const override;

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

  void to_ndf_xml(pugi::xml_node &node) const override;
  void from_ndf_xml(const pugi::xml_node &node) override;

  void from_ndfbin(NDF *, std::istream &stream) override;
  void to_ndfbin(NDF *, std::ostream &stream) const override;

  bool from_ndf_db(NDF_DB *db, int property_id) override;
  bool to_ndf_db(NDF_DB *db, int object_id, int parent = -1,
                 int position = -1) const override;

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

  void to_ndf_xml(pugi::xml_node &node) const override;
  void from_ndf_xml(const pugi::xml_node &node) override;

  void from_ndfbin(NDF *, std::istream &stream) override;
  void to_ndfbin(NDF *, std::ostream &stream) const override;

  bool from_ndf_db(NDF_DB *db, int property_id) override;
  bool to_ndf_db(NDF_DB *db, int object_id, int parent = -1,
                 int position = -1) const override;

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

  void to_ndf_xml(pugi::xml_node &node) const override;
  void from_ndf_xml(const pugi::xml_node &node) override;

  void from_ndfbin(NDF *, std::istream &stream) override;
  void to_ndfbin(NDF *, std::ostream &stream) const override;

  bool from_ndf_db(NDF_DB *db, int property_id) override;
  bool to_ndf_db(NDF_DB *db, int object_id, int parent = -1,
                 int position = -1) const override;

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

  void to_ndf_xml(pugi::xml_node &node) const override;
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
  void to_ndfbin(NDF *root, std::ostream &stream) const override;

  bool from_ndf_db(NDF_DB *db, int property_id) override;
  bool to_ndf_db(NDF_DB *db, int object_id, int parent = -1,
                 int position = -1) const override;

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

  void to_ndf_xml(pugi::xml_node &node) const override;
  void from_ndf_xml(const pugi::xml_node &node) override;

  bool is_import_reference() override { return true; }
  std::unordered_set<std::string> get_import_references() override {
    return {import_name};
  }

  void from_ndfbin(NDF *root, std::istream &stream) override;
  void to_ndfbin(NDF *root, std::ostream &stream) const override;

  bool from_ndf_db(NDF_DB *db, int property_id) override;
  bool to_ndf_db(NDF_DB *db, int object_id, int parent = -1,
                 int position = -1) const override;

  std::unique_ptr<NDFProperty> get_copy() override {
    return std::make_unique<NDFPropertyImportReference>(*this);
  }
  std::string as_string() override { return import_name; }
};

struct NDFPropertyList : NDFProperty {
  std::vector<std::unique_ptr<NDFProperty>> values;
  NDFPropertyList() { property_type = NDFPropertyType::List; }

  void to_ndf_xml(pugi::xml_node &node) const override;
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
  void to_ndfbin(NDF *, std::ostream &) const override;

  bool from_ndf_db(NDF_DB *db, int property_id) override;
  bool to_ndf_db(NDF_DB *db, int object_id, int parent = -1,
                 int position = -1) const override;

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

  void to_ndf_xml(pugi::xml_node &node) const override;
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
  void to_ndfbin(NDF *, std::ostream &) const override;

  bool from_ndf_db(NDF_DB *db, int property_id) override;
  bool to_ndf_db(NDF_DB *db, int object_id, int parent = -1,
                 int position = -1) const override;

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

  void to_ndf_xml(pugi::xml_node &node) const override;
  void from_ndf_xml(const pugi::xml_node &node) override;

  void from_ndfbin(NDF *, std::istream &stream) override;
  void to_ndfbin(NDF *, std::ostream &stream) const override;

  bool from_ndf_db(NDF_DB *db, int property_id) override;
  bool to_ndf_db(NDF_DB *db, int object_id, int parent = -1,
                 int position = -1) const override;

  std::unique_ptr<NDFProperty> get_copy() override {
    return std::make_unique<NDFPropertyInt16>(*this);
  }
  std::string as_string() override { return std::to_string(value); }
};

struct NDFPropertyUInt16 : NDFProperty {
  uint16_t value;
  NDFPropertyUInt16() { property_type = NDFPropertyType::UInt16; }

  void to_ndf_xml(pugi::xml_node &node) const override;
  void from_ndf_xml(const pugi::xml_node &node) override;

  void from_ndfbin(NDF *, std::istream &stream) override;
  void to_ndfbin(NDF *, std::ostream &stream) const override;

  bool from_ndf_db(NDF_DB *db, int property_id) override;
  bool to_ndf_db(NDF_DB *db, int object_id, int parent = -1,
                 int position = -1) const override;

  std::unique_ptr<NDFProperty> get_copy() override {
    return std::make_unique<NDFPropertyUInt16>(*this);
  }
  std::string as_string() override { return std::to_string(value); }
};

// FIXME: error checking for GUIDs?
struct NDFPropertyGUID : NDFProperty {
  std::string guid;
  NDFPropertyGUID() { property_type = NDFPropertyType::NDFGUID; }

  void to_ndf_xml(pugi::xml_node &node) const override;
  void from_ndf_xml(const pugi::xml_node &node) override;

  void from_ndfbin(NDF *, std::istream &stream) override;
  void to_ndfbin(NDF *, std::ostream &stream) const override;

  bool from_ndf_db(NDF_DB *db, int property_id) override;
  bool to_ndf_db(NDF_DB *db, int object_id, int parent = -1,
                 int position = -1) const override;

  std::unique_ptr<NDFProperty> get_copy() override {
    return std::make_unique<NDFPropertyGUID>(*this);
  }
  std::string as_string() override { return guid; }
};

struct NDFPropertyPathReference : NDFProperty {
  std::string path;
  NDFPropertyPathReference() { property_type = NDFPropertyType::PathReference; }

  void to_ndf_xml(pugi::xml_node &node) const override;
  void from_ndf_xml(const pugi::xml_node &node) override;

  void from_ndfbin(NDF *, std::istream &) override;
  void to_ndfbin(NDF *, std::ostream &) const override;

  bool from_ndf_db(NDF_DB *db, int property_id) override;
  bool to_ndf_db(NDF_DB *db, int object_id, int parent = -1,
                 int position = -1) const override;

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

  void to_ndf_xml(pugi::xml_node &node) const override;
  void from_ndf_xml(const pugi::xml_node &node) override;

  void from_ndfbin(NDF *, std::istream &stream) override;
  void to_ndfbin(NDF *, std::ostream &stream) const override;

  bool from_ndf_db(NDF_DB *db, int property_id) override;
  bool to_ndf_db(NDF_DB *db, int object_id, int parent = -1,
                 int position = -1) const override;

  std::unique_ptr<NDFProperty> get_copy() override {
    return std::make_unique<NDFPropertyLocalisationHash>(*this);
  }
  std::string as_string() override { return hash; }
};

struct NDFPropertyS32_vec2 : NDFProperty {
  int32_t x;
  int32_t y;
  NDFPropertyS32_vec2() { property_type = NDFPropertyType::S32_vec2; }

  void to_ndf_xml(pugi::xml_node &node) const override;
  void from_ndf_xml(const pugi::xml_node &node) override;

  void from_ndfbin(NDF *, std::istream &stream) override;
  void to_ndfbin(NDF *, std::ostream &stream) const override;

  bool from_ndf_db(NDF_DB *db, int property_id) override;
  bool to_ndf_db(NDF_DB *db, int object_id, int parent = -1,
                 int position = -1) const override;

  std::unique_ptr<NDFProperty> get_copy() override {
    return std::make_unique<NDFPropertyS32_vec2>(*this);
  }
  std::string as_string() override { return fmt::format("({}, {})", x, y); }
};

struct NDFPropertyF32_vec2 : NDFProperty {
  float x;
  float y;
  NDFPropertyF32_vec2() { property_type = NDFPropertyType::F32_vec2; }

  void to_ndf_xml(pugi::xml_node &node) const override;
  void from_ndf_xml(const pugi::xml_node &node) override;

  void from_ndfbin(NDF *, std::istream &stream) override;
  void to_ndfbin(NDF *, std::ostream &stream) const override;

  bool from_ndf_db(NDF_DB *db, int property_id) override;
  bool to_ndf_db(NDF_DB *db, int object_id, int parent = -1,
                 int position = -1) const override;

  std::unique_ptr<NDFProperty> get_copy() override {
    return std::make_unique<NDFPropertyF32_vec2>(*this);
  }
  std::string as_string() override { return fmt::format("({}, {})", x, y); }
};

struct NDFPropertyPair : NDFProperty {
  std::unique_ptr<NDFProperty> first;
  std::unique_ptr<NDFProperty> second;
  NDFPropertyPair() { property_type = NDFPropertyType::Pair; }

  void to_ndf_xml(pugi::xml_node &node) const override;
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
  void to_ndfbin(NDF *, std::ostream &) const override;

  bool from_ndf_db(NDF_DB *db, int property_id) override;
  bool to_ndf_db(NDF_DB *db, int object_id, int parent = -1,
                 int position = -1) const override;

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

  void to_ndf_xml(pugi::xml_node &node) const override;
  void from_ndf_xml(const pugi::xml_node &node) override;

  void from_ndfbin(NDF *, std::istream &stream) override;
  void to_ndfbin(NDF *, std::ostream &stream) const override;

  bool from_ndf_db(NDF_DB *db, int property_id) override;
  bool to_ndf_db(NDF_DB *db, int object_id, int parent = -1,
                 int position = -1) const override;

  std::unique_ptr<NDFProperty> get_copy() override {
    return std::make_unique<NDFPropertyHash>(*this);
  }
  std::string as_string() override { return hash; }
};
