#pragma once

#include <cassert>
#include <vector>
#include <map>
#include <ranges>
#include <format>

#include "spdlog/spdlog.h"

#include "pugixml.hpp"

#include <filesystem>
namespace fs = std::filesystem;

using namespace std::literals;

struct NDF;

struct NDFClass {
  std::map<std::string, size_t> properties;
};

struct NDFProperty {
  size_t property_idx;
  size_t property_type;
  std::string property_name;
  virtual ~NDFProperty() = default;
  static std::unique_ptr<NDFProperty> get_property_from_ndfbin_xml(size_t ndf_type, const pugi::xml_node& ndf_node);
  static std::unique_ptr<NDFProperty> get_property_from_ndf_xml(size_t ndf_type, const pugi::xml_node& ndf_node);
  virtual void from_ndfbin_xml(const NDF*, const pugi::xml_node&) {
    throw std::runtime_error("Not implemented");
  }
  virtual void to_ndfbin_xml(NDF*, pugi::xml_node&) {
    throw std::runtime_error("Not implemented");
  }
  virtual void to_ndf_xml(pugi::xml_node&) {
    throw std::runtime_error("Not implemented");
  }
  virtual void from_ndf_xml(const pugi::xml_node&) {
    throw std::runtime_error("Not implemented");
  }
};

struct NDFPropertyBool : NDFProperty {
  bool value;
  NDFPropertyBool() {
    property_type = 0x0;
  }
  void from_ndfbin_xml(const NDF* root, const pugi::xml_node& node) override;
  void to_ndfbin_xml(NDF* root, pugi::xml_node& node) override;
  void to_ndf_xml(pugi::xml_node& node) override {
    auto bool_node = node.append_child(property_name.c_str());
    bool_node.append_attribute("value").set_value(value);
    bool_node.append_attribute("typeId").set_value(property_type);
  }
  void from_ndf_xml(const pugi::xml_node& node) override {
    property_name = node.name();
    value = node.attribute("value").as_bool();
    assert(node.attribute("typeId").as_uint() == property_type);
  }
};

struct NDFPropertyInt8 : NDFProperty {
  int8_t value;
  NDFPropertyInt8() {
    property_type = 0x1;
  }
  void from_ndfbin_xml(const NDF* root, const pugi::xml_node& node) override;
  void to_ndfbin_xml(NDF* root, pugi::xml_node& node) override;
  void to_ndf_xml(pugi::xml_node& node) override {
    auto int8_node = node.append_child(property_name.c_str());
    int8_node.append_attribute("value").set_value(value);
    int8_node.append_attribute("typeId").set_value(property_type);
  }
  void from_ndf_xml(const pugi::xml_node& node) override {
    property_name = node.name();
    value = node.attribute("value").as_int();
    assert(node.attribute("typeId").as_uint() == property_type);
  }
};

struct NDFPropertyInt32 : NDFProperty {
  int32_t value;
  NDFPropertyInt32() {
    property_type = 0x2;
  }
  void from_ndfbin_xml(const NDF* root, const pugi::xml_node& node) override;
  void to_ndfbin_xml(NDF* root, pugi::xml_node& node) override;
  void to_ndf_xml(pugi::xml_node& node) override {
    auto int32_node = node.append_child(property_name.c_str());
    int32_node.append_attribute("value").set_value(value);
    int32_node.append_attribute("typeId").set_value(property_type);
  }
  void from_ndf_xml(const pugi::xml_node& node) override {
    property_name = node.name();
    value = node.attribute("value").as_int();
    assert(node.attribute("typeId").as_uint() == property_type);
  }
};

struct NDFPropertyUInt32 : NDFProperty {
  uint32_t value;
  NDFPropertyUInt32() {
    property_type = 0x3;
  }
  void from_ndfbin_xml(const NDF* root, const pugi::xml_node& node) override;
  void to_ndfbin_xml(NDF* root, pugi::xml_node& node) override;
  void to_ndf_xml(pugi::xml_node& node) override {
    auto uint32_node = node.append_child(property_name.c_str());
    uint32_node.append_attribute("value").set_value(value);
    uint32_node.append_attribute("typeId").set_value(property_type);
  }
  void from_ndf_xml(const pugi::xml_node& node) override {
    property_name = node.name();
    value = node.attribute("value").as_uint();
    assert(node.attribute("typeId").as_uint() == property_type);
  }
};

struct NDFPropertyFloat32 : NDFProperty {
  float value;
  NDFPropertyFloat32() {
    property_type = 0x5;
  }
  void from_ndfbin_xml(const NDF* root, const pugi::xml_node& node) override;
  void to_ndfbin_xml(NDF* root, pugi::xml_node& node) override;
  void to_ndf_xml(pugi::xml_node& node) override {
    auto float32_node = node.append_child(property_name.c_str());
    float32_node.append_attribute("value").set_value(value);
    float32_node.append_attribute("typeId").set_value(property_type);
  }
  void from_ndf_xml(const pugi::xml_node& node) override {
    property_name = node.name();
    value = node.attribute("value").as_float();
    assert(node.attribute("typeId").as_uint() == property_type);
  }
};

struct NDFPropertyFloat64 : NDFProperty {
  double value;
  NDFPropertyFloat64() {
    property_type = 0x6;
  }
  void from_ndfbin_xml(const NDF* root, const pugi::xml_node& node) override;
  void to_ndfbin_xml(NDF* root, pugi::xml_node& node) override;
  void to_ndf_xml(pugi::xml_node& node) override {
    auto float64_node = node.append_child(property_name.c_str());
    float64_node.append_attribute("value").set_value(value);
    float64_node.append_attribute("typeId").set_value(property_type);
  }
  void from_ndf_xml(const pugi::xml_node& node) override {
    property_name = node.name();
    value = node.attribute("value").as_double();
    assert(node.attribute("typeId").as_uint() == property_type);
  }
};

struct NDFPropertyString : NDFProperty {
  std::string value;
  NDFPropertyString() {
    property_type = 0x7;
  }
  void from_ndfbin_xml(const NDF* root, const pugi::xml_node& node) override;
  void to_ndfbin_xml(NDF* root, pugi::xml_node& node) override;
  void to_ndf_xml(pugi::xml_node& node) override {
    auto string_node = node.append_child(property_name.c_str());
    string_node.append_attribute("value").set_value(value.c_str());
    string_node.append_attribute("typeId").set_value(property_type);
  }
  void from_ndf_xml(const pugi::xml_node& node) override {
    property_name = node.name();
    value = node.attribute("value").as_string();
    assert(node.attribute("typeId").as_uint() == property_type);
  }
};

struct NDFPropertyWideString : NDFProperty {
  std::string value;
  NDFPropertyWideString() {
    property_type = 0x8;
  }
  void from_ndfbin_xml(const NDF* root, const pugi::xml_node& node) override;
  void to_ndfbin_xml(NDF* root, pugi::xml_node& node) override;
  void to_ndf_xml(pugi::xml_node& node) override {
    auto wide_string_node = node.append_child(property_name.c_str());
    wide_string_node.append_attribute("str").set_value(value.c_str());
    wide_string_node.append_attribute("typeId").set_value(property_type);
  }
  void from_ndf_xml(const pugi::xml_node& node) override {
    property_name = node.name();
    value = node.attribute("str").as_string();
    assert(node.attribute("typeId").as_uint() == property_type);
  }
};

struct NDFPropertyF32_vec3 : NDFProperty {
  float x;
  float y;
  float z;
  NDFPropertyF32_vec3() {
    property_type = 0xB;
  }
  void from_ndfbin_xml(const NDF* root, const pugi::xml_node& node) override;
  void to_ndfbin_xml(NDF* root, pugi::xml_node& node) override;
  void to_ndf_xml(pugi::xml_node& node) override {
    auto f32_vec3_node = node.append_child(property_name.c_str());
    f32_vec3_node.append_attribute("x").set_value(x);
    f32_vec3_node.append_attribute("y").set_value(y);
    f32_vec3_node.append_attribute("z").set_value(z);
    f32_vec3_node.append_attribute("typeId").set_value(property_type);
  }
  void from_ndf_xml(const pugi::xml_node& node) override {
    property_name = node.name();
    x = node.attribute("x").as_float();
    y = node.attribute("y").as_float();
    z = node.attribute("z").as_float();
    assert(node.attribute("typeId").as_uint() == property_type);
  }
};

struct NDFPropertyF32_vec4 : NDFProperty {
  float x;
  float y;
  float z;
  float w;
  NDFPropertyF32_vec4() {
    property_type = 0xC;
  }
  void from_ndfbin_xml(const NDF* root, const pugi::xml_node& node) override;
  void to_ndfbin_xml(NDF* root, pugi::xml_node& node) override;
  void to_ndf_xml(pugi::xml_node& node) override {
    auto f32_vec4_node = node.append_child(property_name.c_str());
    f32_vec4_node.append_attribute("x").set_value(x);
    f32_vec4_node.append_attribute("y").set_value(y);
    f32_vec4_node.append_attribute("z").set_value(z);
    f32_vec4_node.append_attribute("w").set_value(w);
    f32_vec4_node.append_attribute("typeId").set_value(property_type);
  }
  void from_ndf_xml(const pugi::xml_node& node) override {
    property_name = node.name();
    x = node.attribute("x").as_float();
    y = node.attribute("y").as_float();
    z = node.attribute("z").as_float();
    w = node.attribute("w").as_float();
    assert(node.attribute("typeId").as_uint() == property_type);
  }
};

struct NDFPropertyColor : NDFProperty {
  uint8_t r;
  uint8_t g;
  uint8_t b;
  uint8_t a;
  NDFPropertyColor() {
    property_type = 0xD;
  }
  void from_ndfbin_xml(const NDF* root, const pugi::xml_node& node) override;
  void to_ndfbin_xml(NDF* root, pugi::xml_node& node) override;
  void to_ndf_xml(pugi::xml_node& node) override {
    auto color_node = node.append_child(property_name.c_str());
    color_node.append_attribute("r").set_value(r);
    color_node.append_attribute("g").set_value(g);
    color_node.append_attribute("b").set_value(b);
    color_node.append_attribute("a").set_value(a);
    color_node.append_attribute("typeId").set_value(property_type);
  }
  void from_ndf_xml(const pugi::xml_node& node) override {
    property_name = node.name();
    r = node.attribute("r").as_uint();
    g = node.attribute("g").as_uint();
    b = node.attribute("b").as_uint();
    a = node.attribute("a").as_uint();
    assert(node.attribute("typeId").as_uint() == property_type);
  }
};

struct NDFPropertyS32_vec3 : NDFProperty {
  int32_t x;
  int32_t y;
  int32_t z;
  NDFPropertyS32_vec3() {
    property_type = 0xE;
  }
  void from_ndfbin_xml(const NDF* root, const pugi::xml_node& node) override;
  void to_ndfbin_xml(NDF* root, pugi::xml_node& node) override;
  void to_ndf_xml(pugi::xml_node& node) override {
    auto s32_vec3_node = node.append_child(property_name.c_str());
    s32_vec3_node.append_attribute("x").set_value(x);
    s32_vec3_node.append_attribute("y").set_value(y);
    s32_vec3_node.append_attribute("z").set_value(z);
    s32_vec3_node.append_attribute("typeId").set_value(property_type);
  }
  void from_ndf_xml(const pugi::xml_node& node) override {
    property_name = node.name();
    x = node.attribute("x").as_int();
    y = node.attribute("y").as_int();
    z = node.attribute("z").as_int();
    assert(node.attribute("typeId").as_uint() == property_type);
  }
};

enum ReferenceType {
  Import = 2863311530,
  Object = 3149642683
};

struct NDFPropertyObjectReference : NDFProperty {
  std::string object_name;
  NDFPropertyObjectReference() {
    property_type = 0x9;
  }
  void from_ndfbin_xml(const NDF* root, const pugi::xml_node& node) override;
  void to_ndfbin_xml(NDF* root, pugi::xml_node& node) override;
  void to_ndf_xml(pugi::xml_node& node) override {
    auto reference_node = node.append_child(property_name.c_str());
    reference_node.append_attribute("object").set_value(object_name.c_str());
    reference_node.append_attribute("typeId").set_value(property_type);
    reference_node.append_attribute("referenceType").set_value(ReferenceType::Object);
  }
  void from_ndf_xml(const pugi::xml_node& node) override {
    property_name = node.name();
    object_name = node.attribute("object").as_string();
    assert(node.attribute("typeId").as_uint() == property_type);
    assert(node.attribute("referenceType").as_uint() == ReferenceType::Object);
  }
};

struct NDFPropertyImportReference : NDFProperty {
  std::string import_name;
  NDFPropertyImportReference() {
    property_type = 0x9;
  }
  void from_ndfbin_xml(const NDF* root, const pugi::xml_node& node) override;
  void to_ndfbin_xml(NDF* root, pugi::xml_node& node) override;
  void to_ndf_xml(pugi::xml_node& node) override {
    auto reference_node = node.append_child(property_name.c_str());
    reference_node.append_attribute("import").set_value(import_name.c_str());
    reference_node.append_attribute("typeId").set_value(property_type);
    reference_node.append_attribute("referenceType").set_value(ReferenceType::Import);
  }
  void from_ndf_xml(const pugi::xml_node& node) override {
    property_name = node.name();
    import_name = node.attribute("import").as_string();
    assert(node.attribute("typeId").as_uint() == property_type);
    assert(node.attribute("referenceType").as_uint() == ReferenceType::Import);
  }
};

struct NDFPropertyList : NDFProperty {
  std::vector<std::unique_ptr<NDFProperty>> values;
  NDFPropertyList() {
    property_type = 0x11;
  }
  void from_ndfbin_xml(const NDF* root, const pugi::xml_node& node) override;
  void to_ndfbin_xml(NDF* root, pugi::xml_node& node) override;
  void to_ndf_xml(pugi::xml_node& node) override {
    auto list_node = node.append_child(property_name.c_str());
    list_node.append_attribute("typeId").set_value(property_type);
    for(auto const &value : values) {
      value->to_ndf_xml(list_node);
    }
  }
  void from_ndf_xml(const pugi::xml_node& node) override {
    property_name = node.name();
    for(auto const &value_node : node.children()) {
      values.push_back(get_property_from_ndf_xml(value_node.attribute("typeId").as_uint(), value_node));
      values.back()->from_ndf_xml(value_node);
    }
  }
};

struct NDFPropertyMap : NDFProperty {
  std::vector<std::pair<std::unique_ptr<NDFProperty>, std::unique_ptr<NDFProperty>>> values;
  NDFPropertyMap() {
    property_type = 0x12;
  }
  void from_ndfbin_xml(const NDF* root, const pugi::xml_node& node) override;
  void to_ndfbin_xml(NDF* root, pugi::xml_node& node) override;
  void to_ndf_xml(pugi::xml_node& node) override {
    auto map_node = node.append_child(property_name.c_str());
    map_node.append_attribute("typeId").set_value(property_type);
    for(auto const &[key, value] : values) {
      auto map_items_node = map_node.append_child("MapItem");
      key->to_ndf_xml(map_items_node);
      value->to_ndf_xml(map_items_node);
    }
  }
  void from_ndf_xml(const pugi::xml_node& node) override {
    property_name = node.name();
    for(auto const &map_item_node : node.children()) {
      auto key_node = map_item_node.child("Key");
      auto value_node = map_item_node.child("Value");

      auto key = get_property_from_ndf_xml(key_node.attribute("typeId").as_uint(), key_node);
      key->from_ndf_xml(key_node);

      auto value = get_property_from_ndf_xml(value_node.attribute("typeId").as_uint(), value_node);
      value->from_ndf_xml(value_node);

      values.push_back({std::move(key), std::move(value)});
    }
  }
};

struct NDFPropertyS16 : NDFProperty {
  int16_t value;
  NDFPropertyS16() {
    property_type = 0x18;
  }
  void from_ndfbin_xml(const NDF* root, const pugi::xml_node& node) override;
  void to_ndfbin_xml(NDF* root, pugi::xml_node& node) override;
  void to_ndf_xml(pugi::xml_node& node) override {
    auto s16_node = node.append_child(property_name.c_str());
    s16_node.append_attribute("value").set_value(value);
    s16_node.append_attribute("typeId").set_value(property_type);
  }
  void from_ndf_xml(const pugi::xml_node& node) override {
    property_name = node.name();
    value = node.attribute("value").as_int();
    assert(node.attribute("typeId").as_uint() == property_type);
  }
};

struct NDFPropertyU16 : NDFProperty {
  uint16_t value;
  NDFPropertyU16() {
    property_type = 0x19;
  }
  void from_ndfbin_xml(const NDF* root, const pugi::xml_node& node) override;
  void to_ndfbin_xml(NDF* root, pugi::xml_node& node) override;
  void to_ndf_xml(pugi::xml_node& node) override {
    auto u16_node = node.append_child(property_name.c_str());
    u16_node.append_attribute("value").set_value(value);
    u16_node.append_attribute("typeId").set_value(property_type);
  }
  void from_ndf_xml(const pugi::xml_node& node) override {
    property_name = node.name();
    value = node.attribute("value").as_uint();
    assert(node.attribute("typeId").as_uint() == property_type);
  }
};

// FIXME: error checking for GUIDs?
struct NDFPropertyGUID : NDFProperty {
  std::string guid;
  NDFPropertyGUID() {
    property_type = 0x1A;
  }
  void from_ndfbin_xml(const NDF* root, const pugi::xml_node& node) override;
  void to_ndfbin_xml(NDF* root, pugi::xml_node& node) override;
  void to_ndf_xml(pugi::xml_node& node) override {
    auto guid_node = node.append_child(property_name.c_str());
    guid_node.append_attribute("guid").set_value(guid.c_str());
    guid_node.append_attribute("typeId").set_value(property_type);
  }
  void from_ndf_xml(const pugi::xml_node& node) override {
    property_name = node.name();
    guid = node.attribute("guid").as_string();
    assert(node.attribute("typeId").as_uint() == property_type);
  }
};

struct NDFPropertyPathReference : NDFProperty {
  std::string path;
  NDFPropertyPathReference() {
    property_type = 0x1C;
  }
  void from_ndfbin_xml(const NDF* root, const pugi::xml_node& node) override;
  void to_ndfbin_xml(NDF* root, pugi::xml_node& node) override;
  void to_ndf_xml(pugi::xml_node& node) override {
    auto path_node = node.append_child(property_name.c_str());
    path_node.append_attribute("typeId").set_value(property_type);
    path_node.append_attribute("path").set_value(path.c_str());
  }
  void from_ndf_xml(const pugi::xml_node& node) override {
    property_name = node.name();
    path = node.attribute("path").as_string();
    assert(node.attribute("typeId").as_uint() == property_type);
  }
};

// FIXME: error checking for hashes?
struct NDFPropertyLocalisationHash : NDFProperty {
  std::string hash;
  NDFPropertyLocalisationHash() {
    property_type = 0x1D;
  }
  void from_ndfbin_xml(const NDF* root, const pugi::xml_node& node) override;
  void to_ndfbin_xml(NDF* root, pugi::xml_node& node) override;
  void to_ndf_xml(pugi::xml_node& node) override {
    auto hash_node = node.append_child(property_name.c_str());
    hash_node.append_attribute("hash").set_value(hash.c_str());
    hash_node.append_attribute("typeId").set_value(property_type);
  }
  void from_ndf_xml(const pugi::xml_node& node) override {
    property_name = node.name();
    hash = node.attribute("hash").as_string();
    assert(node.attribute("typeId").as_uint() == property_type);
  }
};

struct NDFPropertyS32_vec2 : NDFProperty {
  int32_t x;
  int32_t y;
  NDFPropertyS32_vec2() {
    property_type = 0x1F;
  }
  void from_ndfbin_xml(const NDF* root, const pugi::xml_node& node) override;
  void to_ndfbin_xml(NDF* root, pugi::xml_node& node) override;
  void to_ndf_xml(pugi::xml_node& node) override {
    auto s32_vec2_node = node.append_child(property_name.c_str());
    s32_vec2_node.append_attribute("x").set_value(x);
    s32_vec2_node.append_attribute("y").set_value(y);
    s32_vec2_node.append_attribute("typeId").set_value(property_type);
  }
  void from_ndf_xml(const pugi::xml_node& node) override {
    property_name = node.name();
    x = node.attribute("x").as_int();
    y = node.attribute("y").as_int();
    assert(node.attribute("typeId").as_uint() == property_type);
  }
};

struct NDFPropertyF32_vec2 : NDFProperty {
  float x;
  float y;
  NDFPropertyF32_vec2() {
    property_type = 0x21;
  }
  void from_ndfbin_xml(const NDF* root, const pugi::xml_node& node) override;
  void to_ndfbin_xml(NDF* root, pugi::xml_node& node) override;
  void to_ndf_xml(pugi::xml_node& node) override {
    auto f32_vec2_node = node.append_child(property_name.c_str());
    f32_vec2_node.append_attribute("x").set_value(x);
    f32_vec2_node.append_attribute("y").set_value(y);
    f32_vec2_node.append_attribute("typeId").set_value(property_type);
  }
  void from_ndf_xml(const pugi::xml_node& node) override {
    property_name = node.name();
    x = node.attribute("x").as_float();
    y = node.attribute("y").as_float();
    assert(node.attribute("typeId").as_uint() == property_type);
  }
};

struct NDFPropertyPair : NDFProperty {
  std::unique_ptr<NDFProperty> first;
  std::unique_ptr<NDFProperty> second;
  NDFPropertyPair() {
    property_type = 0x22;
  }
  void from_ndfbin_xml(const NDF* root, const pugi::xml_node& node) override;
  void to_ndfbin_xml(NDF* root, pugi::xml_node& node) override;
  void to_ndf_xml(pugi::xml_node& node) override {
    auto pair_node = node.append_child(property_name.c_str());
    pair_node.append_attribute("typeId").set_value(property_type);
    first->to_ndf_xml(pair_node);
    second->to_ndf_xml(pair_node);
  }
  void from_ndf_xml(const pugi::xml_node& node) override {
    property_name = node.name();
    assert(node.attribute("typeId").as_uint() == property_type);
    auto first_node = node.child("First");
    auto second_node = node.child("Second");

    first = get_property_from_ndf_xml(first_node.attribute("typeId").as_uint(), first_node);
    first->from_ndf_xml(first_node);

    second = get_property_from_ndf_xml(second_node.attribute("typeId").as_uint(), second_node);
    second->from_ndf_xml(second_node);
  }
};

struct NDFObject {
  std::string name;
  std::string class_name;
  bool is_top_object;
  std::string export_path;
  std::vector<std::unique_ptr<NDFProperty>> properties;
};

struct NDF {
private:
  void iterate_imprs(const pugi::xml_node& root, const pugi::xml_node& impr_node, std::vector<std::string> current_import_path) {
    size_t tran_idx = impr_node.attribute("tranIndex").as_uint();
    auto tran_iterator = root.child("TRAN").children().begin();
    std::advance(tran_iterator, tran_idx);

    size_t object_idx = impr_node.attribute("index").as_uint();

    for(auto const & impr_child_node : impr_node.children()) {
      current_import_path.push_back(tran_iterator->attribute("str").as_string());
      iterate_imprs(root, impr_child_node, current_import_path);
      current_import_path.pop_back();
    }

    if(object_idx == 4294967295) {
      return;
    }

    import_name_table[object_idx] = current_import_path | std::views::join_with('/') | std::ranges::to<std::string>();
    import_name_table[object_idx] += std::string("/") + tran_iterator->attribute("str").as_string();
  }

  void iterate_exprs(const pugi::xml_node& root, const pugi::xml_node& expr_node, std::vector<std::string> current_export_path) {
    size_t tran_idx = expr_node.attribute("tranIndex").as_uint();
    auto tran_iterator = root.child("TRAN").children().begin();
    std::advance(tran_iterator, tran_idx);
    
    size_t object_idx = expr_node.attribute("index").as_uint();

    for(auto const & expr_child_node : expr_node.children()) {
      current_export_path.push_back(tran_iterator->attribute("str").as_string());
      iterate_exprs(root, expr_child_node, current_export_path);
      current_export_path.pop_back();
    }
    
    if(object_idx == 4294967295) {
      return;
    }

    objects[object_idx].export_path = current_export_path | std::views::join_with('/') | std::ranges::to<std::string>();
    objects[object_idx].export_path += std::string("/") + tran_iterator->attribute("str").as_string();
  }

public:
  std::map<unsigned int, std::string> import_name_table;
  std::vector<std::string> string_table;
  std::vector<NDFObject> objects;

  void load_from_ndfbin_xml(fs::path path) {
    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_file(path.c_str());
    spdlog::debug("Load result: {}", result.description());
    assert(result.status == pugi::status_ok);

    auto root = doc.child("root").child("NdfBin").child("toc0header");
    assert(root);

    for (auto const impr_node : root.child("IMPR").children()) {
      iterate_imprs(root, impr_node, {});
    }

    for(auto const string_node : root.child("STRG").children()) {
      string_table.push_back(string_node.attribute("str").as_string());
    }

    for (auto const [object_idx, object_node] : std::views::enumerate(root.child("OBJE").children())) {
      NDFObject object;
      object.name = std::format("Object_{}", object_idx);

      size_t class_idx = object_node.attribute("classIndex").as_uint();
      auto class_iterator = root.child("CLAS").children().begin();
      std::advance(class_iterator, class_idx);
      object.class_name = class_iterator->attribute("str").as_string();

      for(auto const prop_node : object_node.children()) {
        size_t property_idx = prop_node.attribute("propertyIndex").as_uint();

        if(property_idx == 2880154539) {
          continue;
        }

        spdlog::debug("Property Index: {}", property_idx);
        auto property_iterator = root.child("PROP").children().begin();
        std::advance(property_iterator, property_idx);

        auto ndf_node = prop_node.child("NDFType");
        size_t ndf_type = ndf_node.attribute("typeId").as_uint();

        std::unique_ptr<NDFProperty> property = NDFProperty::get_property_from_ndfbin_xml(ndf_type, ndf_node);
        property->from_ndfbin_xml(this, ndf_node);

        property->property_name = property_iterator->attribute("str").as_string();

        object.properties.push_back(std::move(property));
      }

      objects.push_back(std::move(object));
    }

    for(auto const &topo_node : root.child("TOPO").children()) {
      size_t object_idx = topo_node.attribute("objectIndex").as_uint();
      auto object_iterator = objects.begin();
      std::advance(object_iterator, object_idx);
      object_iterator->is_top_object = true;
    }

    std::vector<std::string> current_export_path;
    iterate_exprs(root, root.child("EXPR").child("Export"), current_export_path);
  }

  void save_as_ndf_xml(fs::path path) {
    pugi::xml_document doc;
    auto root = doc.append_child("NDF");

    for(const auto &obj : objects) {
      auto object_node = root.append_child(obj.name.c_str());
      object_node.append_attribute("class") = obj.class_name.c_str();
      object_node.append_attribute("export_path") = obj.export_path.c_str();
      object_node.append_attribute("is_top_object") = obj.is_top_object;

      for(const auto &prop : obj.properties) {
        prop->to_ndf_xml(object_node);
      }
    }

    doc.save_file(path.c_str());
  }

  void load_from_ndf_xml(fs::path path) {
    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_file(path.c_str());
    spdlog::debug("Load result: {}", result.description());
    assert(result.status == pugi::status_ok);

    for(const auto &obj : doc.child("NDF").children()) {
      NDFObject object;
      object.name = obj.name();
      object.class_name = obj.attribute("class").as_string();
      object.export_path = obj.attribute("export_path").as_string();
      object.is_top_object = obj.attribute("is_top_object").as_bool();

      for(const auto &prop : obj.children()) {
        size_t ndf_type = prop.attribute("typeId").as_uint();
        std::unique_ptr<NDFProperty> property = NDFProperty::get_property_from_ndf_xml(ndf_type, prop);
        property->from_ndf_xml(prop);
        object.properties.push_back(std::move(property));
      }

      objects.push_back(std::move(object));
    }
  }
private:
  std::map<std::string, size_t> gen_object_table;

  std::vector<std::string> gen_string_items;
  std::map<std::string, size_t> gen_string_table;

  std::map<std::string, size_t> gen_clas_items;
  std::vector<std::string> gen_clas_table;
  
  std::map<std::string, size_t> gen_tran_items;
  std::vector<std::string> gen_tran_table;

  std::map<std::vector<size_t>, size_t> gen_import_table;
  std::vector<std::vector<size_t>> gen_import_items;
  std::map<std::vector<size_t>, size_t> gen_export_table;
  std::vector<std::vector<size_t>> gen_export_items;

  std::vector<NDFClass> gen_property_table;

  void save_imprs(const std::map<std::vector<size_t>, size_t>& gen_table, pugi::xml_node& root_node, const std::string& first_name) {
    std::vector<size_t> previous_import_path = {};
    std::vector<pugi::xml_node> previous_import_nodes = {};
    for(const auto&[k, v] : gen_table) {
      {
        std::string key_str = "";
        for(auto const &foo : k) {
          key_str += gen_tran_table[foo] + "/";
        }
        spdlog::debug("Import: {} -> {}", key_str, v);
      }

      // first we get the diff to the last run
      int diff_point = 0;
      std::vector<size_t> current_path;
      for(unsigned int x = 0; x < k.size(); x++) {
        if(x >= previous_import_path.size()) {
          diff_point = x;
          spdlog::debug("diff_point size: {} {}", previous_import_path.size(), x);
          break;
        }
        if(previous_import_path[x] != k[x]) {
          diff_point = x;
          spdlog::debug("diff_point diff {} {}", previous_import_path[x], k[x]);
          break;
        }
        // we do not add the item that differs
        current_path.push_back(k[x]);
      }
      // now shorten the previous import_path and import xml nodes to the diff point
    spdlog::debug("diff_point: {}", diff_point);
        {
          std::string key_str = "";
          for(auto const &foo : previous_import_path) {
            key_str += gen_tran_table[foo] + "/";
          }
          spdlog::debug("before shorten prev Import path: {} -> {}", key_str, v);
        }
      int diff = previous_import_nodes.size() - diff_point;
      for(int x = 0; x < diff; x++) {
        previous_import_path.pop_back();
        previous_import_nodes.pop_back();
      }
        {
          std::string key_str = "";
          for(auto const &foo : previous_import_path) {
            key_str += gen_tran_table[foo] + "/";
          }
          spdlog::debug("after shorten prev Import path: {} -> {}", key_str, v);
        }
      assert(previous_import_nodes.size() == previous_import_path.size());

      pugi::xml_node current_node = root_node;
      if(previous_import_nodes.size() > 0) {
        current_node = previous_import_nodes.back();
      }

      for(unsigned int x = diff_point; x < k.size(); x++) {
        // same layer -> add new node
        std::string name = "imprs";
        if(x == 0) {
          name = first_name;
        }
        auto impr_node = current_node.append_child(name.c_str());

        // we now update the current_path and search for a possible import
        current_path.push_back(k[x]);

        impr_node.append_attribute("tranIndex").set_value(k[x]);
        auto impr_index_it = gen_table.find(current_path); 
        if(impr_index_it != gen_table.end()) {
          impr_node.append_attribute("index").set_value(impr_index_it->second);
        } else {
          impr_node.append_attribute("index").set_value(4294967295);
        }

        previous_import_nodes.push_back(impr_node);
        current_node = impr_node;
        {
          std::string key_str = "";
          for(auto const &foo : current_path) {
            key_str += gen_tran_table[foo] + "/";
          }
          spdlog::debug("current Import path: {} -> {}", key_str, v);
        }
      }
      
      previous_import_path = current_path;
    }
  }

public:
  size_t get_object(const std::string& str) {
    auto obj_idx = gen_object_table.find(str);
    if(obj_idx == gen_object_table.end()) {
      return 4294967295;
    }
    return obj_idx->second;
  }

  size_t get_or_add_string(const std::string& str) {
    auto it = gen_string_table.find(str);
    if(it == gen_string_table.end()) {
      gen_string_items.push_back(str);
      gen_string_table.insert({str, gen_string_items.size() - 1});
      return gen_string_items.size() - 1;
    } else {
      return it->second;
    }
  }

  size_t get_or_add_tran(const std::string& str) {
    auto it = gen_tran_items.find(str);
    if(it == gen_tran_items.end()) {
      gen_tran_table.push_back(str);
      gen_tran_items.insert({str, gen_tran_table.size() - 1});
      return gen_tran_table.size() - 1;
    } else {
      return it->second;
    }
  }

  size_t get_or_add_impr_indices(const std::vector<size_t>& vec) {
    auto it = gen_import_table.find(vec);
    if(it == gen_import_table.end()) {
      gen_import_items.push_back(vec);
      gen_import_table.insert({vec, gen_import_items.size() - 1});
      return gen_import_items.size() - 1;
    } else {
      return it->second;
    }
  }

  size_t get_or_add_impr(std::string impr) {
    std::vector<size_t> vec;
    for(auto str : std::views::split(impr, '/')) {
      std::string foo = std::string(str.begin(), str.end());
      vec.push_back(get_or_add_tran(foo));
    }
    return get_or_add_impr_indices(vec);
  }

  size_t get_or_add_expr_indices(const std::vector<size_t>& vec) {
    auto it = gen_export_table.find(vec);
    if(it == gen_export_table.end()) {
      gen_export_items.push_back(vec);
      gen_export_table.insert({vec, gen_export_items.size() - 1});
      return gen_export_items.size() - 1;
    } else {
      return it->second;
    }
  }

  size_t get_or_add_expr(std::string expr) {
    std::vector<size_t> vec;
    for(auto str : std::views::split(expr, '/')) {
      std::string foo = std::string(str.begin(), str.end());
      vec.push_back(get_or_add_tran(foo));
    }
    return get_or_add_expr_indices(vec);
  }

  void save_as_ndfbin_xml(fs::path path) {
    gen_string_items.clear();
    gen_string_table.clear();
    gen_clas_items.clear();
    gen_clas_table.clear();
    gen_property_table.clear();
    gen_object_table.clear();

    pugi::xml_document doc;
    auto root = doc.append_child("root").append_child("NdfBin").append_child("toc0header");

    auto obje = root.append_child("OBJE");

    for(const auto &[obj_idx, obj] : std::views::enumerate(objects)) {
      gen_object_table.insert({obj.name, obj_idx});
    }

    auto topo = root.append_child("TOPO");
    auto clas = root.append_child("CLAS");
    
    // generate chunk
    auto chnk = root.append_child("CHNK");
    auto chunk = chnk.append_child("Chunk");
    chunk.append_attribute("unk0").set_value(0);
    chunk.append_attribute("objectCount").set_value(objects.size());

    auto prop = root.append_child("PROP");
    size_t property_idx = 0;

    auto strg = root.append_child("STRG");
    auto tran = root.append_child("TRAN");
    auto impr = root.append_child("IMPR");
    auto expr = root.append_child("EXPR");

    size_t object_idx = 0;
    for(const auto &obj : objects) {
      auto obje_node = obje.append_child("Object");

      size_t class_idx;
      auto clas_it = gen_clas_items.find(obj.class_name);
      if(clas_it == gen_clas_items.end()) {
        gen_clas_table.push_back(obj.class_name);
        gen_clas_items.insert({obj.class_name, gen_clas_table.size() - 1});
        class_idx = gen_clas_table.size() - 1;
        gen_property_table.emplace_back();
        for(auto& property : obj.properties) {
          // add PROP entry
          auto prop_node = prop.append_child("Property");
          prop_node.append_attribute("str").set_value(property->property_name.c_str());
          prop_node.append_attribute("classIndex").set_value(class_idx);

          gen_property_table[class_idx].properties.insert({property->property_name, property_idx});

          property_idx += 1;
        }
      } else {
        class_idx = clas_it->second;
      }
      
      for(auto& property : obj.properties) {
        // save property data
        size_t property_idx = gen_property_table[class_idx].properties[property->property_name];
        auto prop_node = obje_node.append_child("Property");
        prop_node.append_attribute("propertyIndex").set_value(property_idx);
        auto ndf_type_node = prop_node.append_child("NDFType");
        ndf_type_node.append_attribute("typeId").set_value(property->property_type);
        property->to_ndfbin_xml(this, ndf_type_node);
      }

      obje_node.append_attribute("classIndex").set_value(class_idx);

      if(obj.is_top_object) {
        auto topo_node = topo.append_child("TOPObject");
        topo_node.append_attribute("objectIndex").set_value(object_idx);
      }

      if(obj.export_path.size()) {
        get_or_add_expr(obj.export_path);
      }

      object_idx += 1;
    }

    for(const auto& str : gen_string_items) {
      auto str_node = strg.append_child("String");
      str_node.append_attribute("str").set_value(str.c_str());
    }

    for(const auto &clas_name : gen_clas_table) {
      auto clas_node = clas.append_child("Class");
      clas_node.append_attribute("str").set_value(clas_name.c_str());
    }

    for(const auto &tran_name : gen_tran_table) {
      auto tran_node = tran.append_child("Tran");
      tran_node.append_attribute("str").set_value(tran_name.c_str());
    }

    save_imprs(gen_import_table, impr, "Import");
    save_imprs(gen_export_table, expr, "Export");

    doc.save_file(path.c_str());
  }
};


