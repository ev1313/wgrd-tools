#pragma once

#include <cassert>
#include <vector>
#include <map>
#include <set>
#include <ranges>

#include "spdlog/spdlog.h"

#include "pugixml.hpp"

#include <filesystem>
namespace fs = std::filesystem;

using namespace std::literals;

struct NDF;

struct NDFClass {
  std::map<std::string, uint32_t> properties;
};

struct NDFProperty {
  uint32_t property_idx;
  uint32_t property_type;
  std::string property_name;
  virtual ~NDFProperty() = default;
  static std::unique_ptr<NDFProperty> get_property_from_ndftype(uint32_t ndf_type);
  static std::unique_ptr<NDFProperty> get_property_from_ndfbin_xml(uint32_t ndf_type, const pugi::xml_node& ndf_node);
  static std::unique_ptr<NDFProperty> get_property_from_ndf_xml(uint32_t ndf_type, const pugi::xml_node& ndf_node);
  static std::unique_ptr<NDFProperty> get_property_from_ndfbin(uint32_t ndf_type, std::istream& stream);
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
  virtual void from_ndfbin(NDF*, std::istream&) {
    throw std::runtime_error("Not implemented");
  }
  virtual void to_ndfbin(NDF*, std::ostream&) {
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
private:
  #pragma pack(push, 1)
  struct NDF_Bool {
    uint8_t value;
  };
  #pragma pack(pop)
public:
  void from_ndfbin(NDF*, std::istream& stream) override {
    NDF_Bool ndf_bool;
    stream.read(reinterpret_cast<char*>(&ndf_bool), sizeof(NDF_Bool));
    value = ndf_bool.value;
  }
  void to_ndfbin(NDF*, std::ostream& stream) override {
    NDF_Bool ndf_bool;
    ndf_bool.value = value;
    stream.write(reinterpret_cast<char*>(&ndf_bool), sizeof(NDF_Bool));
  }
};

struct NDFPropertyInt8 : NDFProperty {
  uint8_t value;
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
private:
  #pragma pack(push, 1)
  struct NDF_Int8 {
    uint8_t value;
  };
  #pragma pack(pop)
public:
  void from_ndfbin(NDF*, std::istream& stream) override {
    NDF_Int8 ndf_int8;
    stream.read(reinterpret_cast<char*>(&ndf_int8), sizeof(NDF_Int8));
    value = ndf_int8.value;
  }
  void to_ndfbin(NDF*, std::ostream& stream) override {
    NDF_Int8 ndf_int8;
    ndf_int8.value = value;
    stream.write(reinterpret_cast<char*>(&ndf_int8), sizeof(NDF_Int8));
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
private:
  #pragma pack(push, 1)
  struct NDF_Int32 {
    int32_t value;
  };
  #pragma pack(pop)
public:
  void from_ndfbin(NDF*, std::istream& stream) override {
    NDF_Int32 ndf_int32;
    stream.read(reinterpret_cast<char*>(&ndf_int32), sizeof(NDF_Int32));
    value = ndf_int32.value;
  }
  void to_ndfbin(NDF*, std::ostream& stream) override {
    NDF_Int32 ndf_int32;
    ndf_int32.value = value;
    stream.write(reinterpret_cast<char*>(&ndf_int32), sizeof(NDF_Int32));
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
private:
  #pragma pack(push, 1)
  struct NDF_UInt32 {
    uint32_t value;
  };
  #pragma pack(pop)
public:
  void from_ndfbin(NDF*, std::istream& stream) override {
    NDF_UInt32 ndf_uint32;
    stream.read(reinterpret_cast<char*>(&ndf_uint32), sizeof(NDF_UInt32));
    value = ndf_uint32.value;
  }
  void to_ndfbin(NDF*, std::ostream& stream) override {
    NDF_UInt32 ndf_uint32;
    ndf_uint32.value = value;
    stream.write(reinterpret_cast<char*>(&ndf_uint32), sizeof(NDF_UInt32));
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
private:
  #pragma pack(push, 1)
  struct NDF_Float32 {
    float value;
  };
  #pragma pack(pop)
public:
  void from_ndfbin(NDF*, std::istream& stream) override {
    NDF_Float32 ndf_float32;
    stream.read(reinterpret_cast<char*>(&ndf_float32), sizeof(NDF_Float32));
    value = ndf_float32.value;
  }
  void to_ndfbin(NDF*, std::ostream& stream) override {
    NDF_Float32 ndf_float32;
    ndf_float32.value = value;
    stream.write(reinterpret_cast<char*>(&ndf_float32), sizeof(NDF_Float32));
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
private:
  #pragma pack(push, 1)
  struct NDF_Float64 {
    double value;
  };
  #pragma pack(pop)
public:
  void from_ndfbin(NDF*, std::istream& stream) override {
    NDF_Float64 ndf_float64;
    stream.read(reinterpret_cast<char*>(&ndf_float64), sizeof(NDF_Float64));
    value = ndf_float64.value;
  }
  void to_ndfbin(NDF*, std::ostream& stream) override {
    NDF_Float64 ndf_float64;
    ndf_float64.value = value;
    stream.write(reinterpret_cast<char*>(&ndf_float64), sizeof(NDF_Float64));
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
private:
  #pragma pack(push, 1)
  struct NDF_String {
    uint32_t string_index;
  };
  #pragma pack(pop)
public:
  void from_ndfbin(NDF* root, std::istream& stream) override;
  void to_ndfbin(NDF* root, std::ostream& stream) override;
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
private:
  #pragma pack(push, 1)
  struct NDF_WideString {
    uint32_t length;
  };
  #pragma pack(pop)
public:
  void from_ndfbin(NDF* root, std::istream& stream) override;
  void to_ndfbin(NDF* root, std::ostream& stream) override;
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
private:
  #pragma pack(push, 1)
  struct NDF_F32_vec3 {
    float x;
    float y;
    float z;
  };
  #pragma pack(pop)
public:
  void from_ndfbin(NDF*, std::istream& stream) override {
    NDF_F32_vec3 ndf_f32_vec3;
    stream.read(reinterpret_cast<char*>(&ndf_f32_vec3), sizeof(NDF_F32_vec3));
    x = ndf_f32_vec3.x;
    y = ndf_f32_vec3.y;
    z = ndf_f32_vec3.z;
  }
  void to_ndfbin(NDF*, std::ostream& stream) override {
    NDF_F32_vec3 ndf_f32_vec3;
    ndf_f32_vec3.x = x;
    ndf_f32_vec3.y = y;
    ndf_f32_vec3.z = z;
    stream.write(reinterpret_cast<char*>(&ndf_f32_vec3), sizeof(NDF_F32_vec3));
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
private:
  #pragma pack(push, 1)
  struct NDF_F32_vec4 {
    float x;
    float y;
    float z;
    float w;
  };
  #pragma pack(pop)
public:
  void from_ndfbin(NDF*, std::istream& stream) override {
    NDF_F32_vec4 ndf_f32_vec4;
    stream.read(reinterpret_cast<char*>(&ndf_f32_vec4), sizeof(NDF_F32_vec4));
    x = ndf_f32_vec4.x;
    y = ndf_f32_vec4.y;
    z = ndf_f32_vec4.z;
    w = ndf_f32_vec4.w;
  }
  void to_ndfbin(NDF*, std::ostream& stream) override {
    NDF_F32_vec4 ndf_f32_vec4;
    ndf_f32_vec4.x = x;
    ndf_f32_vec4.y = y;
    ndf_f32_vec4.z = z;
    ndf_f32_vec4.w = w;
    stream.write(reinterpret_cast<char*>(&ndf_f32_vec4), sizeof(NDF_F32_vec4));
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
private:
  #pragma pack(push, 1)
  struct NDF_Color {
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
  };
  #pragma pack(pop)
public:
  void from_ndfbin(NDF*, std::istream& stream) override {
    NDF_Color ndf_color;
    stream.read(reinterpret_cast<char*>(&ndf_color), sizeof(NDF_Color));
    r = ndf_color.r;
    g = ndf_color.g;
    b = ndf_color.b;
    a = ndf_color.a;
  }
  void to_ndfbin(NDF*, std::ostream& stream) override {
    NDF_Color ndf_color;
    ndf_color.r = r;
    ndf_color.g = g;
    ndf_color.b = b;
    ndf_color.a = a;
    stream.write(reinterpret_cast<char*>(&ndf_color), sizeof(NDF_Color));
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
private:
  #pragma pack(push, 1)
  struct NDF_S32_vec3 {
    int32_t x;
    int32_t y;
    int32_t z;
  };
  #pragma pack(pop)
public:
  void from_ndfbin(NDF*, std::istream& stream) override {
    NDF_S32_vec3 ndf_s32_vec3;
    stream.read(reinterpret_cast<char*>(&ndf_s32_vec3), sizeof(NDF_S32_vec3));
    x = ndf_s32_vec3.x;
    y = ndf_s32_vec3.y;
    z = ndf_s32_vec3.z;
  }
  void to_ndfbin(NDF*, std::ostream& stream) override {
    NDF_S32_vec3 ndf_s32_vec3;
    ndf_s32_vec3.x = x;
    ndf_s32_vec3.y = y;
    ndf_s32_vec3.z = z;
    stream.write(reinterpret_cast<char*>(&ndf_s32_vec3), sizeof(NDF_S32_vec3));
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
    reference_node.append_attribute("referenceType").set_value("object");
  }
  void from_ndf_xml(const pugi::xml_node& node) override {
    property_name = node.name();
    object_name = node.attribute("object").as_string();
    assert(node.attribute("typeId").as_uint() == property_type);
    assert(node.attribute("referenceType").as_string() == std::string("object"));
  }
private:
  #pragma pack(push, 1)
  struct NDF_ObjectReference {
    uint32_t object_index;
    uint32_t class_index;
  };
  #pragma pack(pop)
public:
  void from_ndfbin(NDF* root, std::istream& stream) override;
  void to_ndfbin(NDF* root, std::ostream& stream) override;
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
    reference_node.append_attribute("referenceType").set_value("import");
  }
  void from_ndf_xml(const pugi::xml_node& node) override {
    property_name = node.name();
    import_name = node.attribute("import").as_string();
    assert(node.attribute("typeId").as_uint() == property_type);
    assert(node.attribute("referenceType").as_string() == std::string("import"));
  }
private:
  #pragma pack(push, 1)
  struct NDF_ImportReference {
    uint32_t import_index;
  };
  #pragma pack(pop)
public:
  void from_ndfbin(NDF* root, std::istream& stream) override;
  void to_ndfbin(NDF* root, std::ostream& stream) override;
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
private:
  #pragma pack(push, 1)
  struct NDF_List {
    uint32_t count;
  };
  #pragma pack(pop)
public:
  void from_ndfbin(NDF*, std::istream&) override;
  void to_ndfbin(NDF*, std::ostream&) override;
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
private:
  #pragma pack(push, 1)
  struct NDF_Map {
    uint32_t count;
  };
  #pragma pack(pop)
public:
  void from_ndfbin(NDF*, std::istream&) override;
  void to_ndfbin(NDF*, std::ostream&) override;
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
private:
  #pragma pack(push, 1)
  struct NDF_S16 {
    int16_t value;
  };
  #pragma pack(pop)
public:
  void from_ndfbin(NDF*, std::istream& stream) override {
    NDF_S16 ndf_s16;
    stream.read(reinterpret_cast<char*>(&ndf_s16), sizeof(NDF_S16));
    value = ndf_s16.value;
  }
  void to_ndfbin(NDF*, std::ostream& stream) override {
    NDF_S16 ndf_s16;
    ndf_s16.value = value;
    stream.write(reinterpret_cast<char*>(&ndf_s16), sizeof(NDF_S16));
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
private:
  #pragma pack(push, 1)
  struct NDF_U16 {
    uint16_t value;
  };
  #pragma pack(pop)
public:
  void from_ndfbin(NDF*, std::istream& stream) override {
    NDF_U16 ndf_u16;
    stream.read(reinterpret_cast<char*>(&ndf_u16), sizeof(NDF_U16));
    value = ndf_u16.value;
  }
  void to_ndfbin(NDF*, std::ostream& stream) override {
    NDF_U16 ndf_u16;
    ndf_u16.value = value;
    stream.write(reinterpret_cast<char*>(&ndf_u16), sizeof(NDF_U16));
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
private:
  #pragma pack(push, 1)
  struct NDF_GUID {
    uint8_t guid[16];
  };
  #pragma pack(pop)
public:
  void from_ndfbin(NDF*, std::istream& stream) override {
    NDF_GUID ndf_guid;
    stream.read(reinterpret_cast<char*>(&ndf_guid), sizeof(NDF_GUID));
    guid = "";
    for(auto const &byte : ndf_guid.guid) {
      guid += std::format("{:02X}", byte);
    }
  }
  void to_ndfbin(NDF*, std::ostream& stream) override {
    NDF_GUID ndf_guid;
    for(uint32_t i = 0; i < 16; i++) {
      std::from_chars(guid.c_str() + i * 2, guid.c_str() + i * 2 + 2, ndf_guid.guid[i], 16);
    }
    stream.write(reinterpret_cast<char*>(&ndf_guid), sizeof(NDF_GUID));
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
private:
  #pragma pack(push, 1)
  struct NDF_PathReference {
    uint32_t path_index;
  };
  #pragma pack(pop)
public:
  void from_ndfbin(NDF*, std::istream&) override;
  void to_ndfbin(NDF*, std::ostream&) override;
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
private:
  #pragma pack(push, 1)
  struct NDF_LocalisationHash {
    uint8_t hash[8];
  };
  #pragma pack(pop)
public:
  void from_ndfbin(NDF*, std::istream& stream) override {
    NDF_LocalisationHash ndf_hash;
    stream.read(reinterpret_cast<char*>(&ndf_hash), sizeof(NDF_LocalisationHash));
    hash = "";
    for(auto const &byte : ndf_hash.hash) {
      hash += std::format("{:02X}", byte);
    }
  }
  void to_ndfbin(NDF*, std::ostream& stream) override {
    NDF_LocalisationHash ndf_hash;
    for(uint32_t i = 0; i < 8; i++) {
      std::from_chars(hash.c_str() + i * 2, hash.c_str() + i * 2 + 2, ndf_hash.hash[i], 16);
    }
    stream.write(reinterpret_cast<char*>(&ndf_hash), sizeof(NDF_LocalisationHash));
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
private:
  #pragma pack(push, 1)
  struct NDF_S32_vec2 {
    int32_t x;
    int32_t y;
  };
  #pragma pack(pop)
public:
  void from_ndfbin(NDF*, std::istream& stream) override {
    NDF_S32_vec2 ndf_s32_vec2;
    stream.read(reinterpret_cast<char*>(&ndf_s32_vec2), sizeof(NDF_S32_vec2));
    x = ndf_s32_vec2.x;
    y = ndf_s32_vec2.y;
  }
  void to_ndfbin(NDF*, std::ostream& stream) override {
    NDF_S32_vec2 ndf_s32_vec2;
    ndf_s32_vec2.x = x;
    ndf_s32_vec2.y = y;
    stream.write(reinterpret_cast<char*>(&ndf_s32_vec2), sizeof(NDF_S32_vec2));
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
private:
  #pragma pack(push, 1)
  struct NDF_F32_vec2 {
    float x;
    float y;
  };
  #pragma pack(pop)
public:
  void from_ndfbin(NDF*, std::istream& stream) override {
    NDF_F32_vec2 ndf_f32_vec2;
    stream.read(reinterpret_cast<char*>(&ndf_f32_vec2), sizeof(NDF_F32_vec2));
    x = ndf_f32_vec2.x;
    y = ndf_f32_vec2.y;
  }
  void to_ndfbin(NDF*, std::ostream& stream) override {
    NDF_F32_vec2 ndf_f32_vec2;
    ndf_f32_vec2.x = x;
    ndf_f32_vec2.y = y;
    stream.write(reinterpret_cast<char*>(&ndf_f32_vec2), sizeof(NDF_F32_vec2));
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
public:
  void from_ndfbin(NDF*, std::istream&) override;
  void to_ndfbin(NDF*, std::ostream&) override;
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
    uint32_t tran_idx = impr_node.attribute("tranIndex").as_uint();
    auto tran_iterator = root.child("TRAN").children().begin();
    std::advance(tran_iterator, tran_idx);

    uint32_t object_idx = impr_node.attribute("index").as_uint();

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
    uint32_t tran_idx = expr_node.attribute("tranIndex").as_uint();
    auto tran_iterator = root.child("TRAN").children().begin();
    std::advance(tran_iterator, tran_idx);
    
    uint32_t object_idx = expr_node.attribute("index").as_uint();

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
  // FIXME: load_from_ndfbin_xml should also use this table
  std::vector<std::string> class_table;
  std::vector<std::pair<std::string, uint32_t>> property_table;
  std::vector<std::string> tran_table;
  std::vector<NDFObject> objects;

  void load_from_ndfbin_xml(fs::path path);

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
        uint32_t ndf_type = prop.attribute("typeId").as_uint();
        std::unique_ptr<NDFProperty> property = NDFProperty::get_property_from_ndf_xml(ndf_type, prop);
        property->from_ndf_xml(prop);
        object.properties.push_back(std::move(property));
      }

      objects.push_back(std::move(object));
    }
  }

  void load_imprs(std::istream &stream, std::vector<std::string> current_import_path) {
    uint32_t tran_index;
    stream.read(reinterpret_cast<char*>(&tran_index), sizeof(uint32_t));
    uint32_t index;
    stream.read(reinterpret_cast<char*>(&index), sizeof(uint32_t));
    uint32_t count;
    stream.read(reinterpret_cast<char*>(&count), sizeof(uint32_t));

    uint32_t begin_offset = (uint32_t)stream.tellg();
    if(count > 0) {
      std::vector<uint32_t> offsets;
      offsets.resize(count);
      stream.read(reinterpret_cast<char*>(offsets.data()), sizeof(uint32_t) * count);

      for(uint32_t offset : offsets) {
        spdlog::debug("assertion @0x{:02X} is 0x{:02X} should be 0x{:02X}", begin_offset, ((uint32_t)stream.tellg() - begin_offset), offset);
        assert(offset == ((uint32_t)stream.tellg() - begin_offset));
        current_import_path.push_back(tran_table[tran_index]);
        load_imprs(stream, current_import_path);
        current_import_path.pop_back();
      }
    }

    if(index == 4294967295) {
      return;
    }

    import_name_table[index] = current_import_path | std::views::join_with('/') | std::ranges::to<std::string>();
    import_name_table[index] += std::string("/") + tran_table[tran_index];
    spdlog::debug("Import: {}", import_name_table[index]);
  }

  void load_exprs(std::istream &stream, std::vector<std::string> current_export_path) {
    uint32_t tran_index;
    stream.read(reinterpret_cast<char*>(&tran_index), sizeof(uint32_t));
    uint32_t index;
    stream.read(reinterpret_cast<char*>(&index), sizeof(uint32_t));
    uint32_t count;
    stream.read(reinterpret_cast<char*>(&count), sizeof(uint32_t));

    uint32_t begin_offset = (uint32_t)stream.tellg();
    if(count > 0) {
      std::vector<uint32_t> offsets;
      offsets.resize(count);
      stream.read(reinterpret_cast<char*>(offsets.data()), sizeof(uint32_t) * count);

      for(uint32_t offset : offsets) {
        assert(offset == ((uint32_t)stream.tellg() - begin_offset));
        current_export_path.push_back(tran_table[tran_index]);
        load_exprs(stream, current_export_path);
        current_export_path.pop_back();
      }
    }

    if(index == 4294967295) {
      return;
    }

    objects[index].export_path = current_export_path | std::views::join_with('/') | std::ranges::to<std::string>();
    objects[index].export_path += std::string("/") + tran_table[tran_index];
    spdlog::debug("Export: {}", objects[index].export_path);
  }

  void load_from_ndfbin(fs::path path);
private:
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

  void save_imprs(const std::map<std::vector<uint32_t>, uint32_t>& gen_table, pugi::xml_node& root_node, const std::string& first_name);
  void save_ndfbin_imprs(const std::map<std::vector<uint32_t>, uint32_t>& gen_table, std::ostream& stream);
public:
  uint32_t get_object(const std::string& str) {
    auto obj_idx = gen_object_table.find(str);
    if(obj_idx == gen_object_table.end()) {
      return 4294967295;
    }
    return obj_idx->second;
  }

  uint32_t get_class(const std::string& str) {
    auto obj_idx = gen_clas_items.find(str);
    if(obj_idx == gen_clas_items.end()) {
      return 4294967295;
    }
    return obj_idx->second;
  }

  uint32_t get_class_of_object(uint32_t object_idx) {
    if(object_idx == 4294967295) {
      return 4294967295;
    }
    return get_class(objects[object_idx].class_name);
  }

  uint32_t get_or_add_string(const std::string& str) {
    auto it = gen_string_table.find(str);
    if(it == gen_string_table.end()) {
      gen_string_items.push_back(str);
      gen_string_table.insert({str, gen_string_items.size() - 1});
      return gen_string_items.size() - 1;
    } else {
      return it->second;
    }
  }

  uint32_t get_or_add_tran(const std::string& str) {
    auto it = gen_tran_items.find(str);
    if(it == gen_tran_items.end()) {
      gen_tran_table.push_back(str);
      gen_tran_items.insert({str, gen_tran_table.size() - 1});
      return gen_tran_table.size() - 1;
    } else {
      return it->second;
    }
  }

  uint32_t get_or_add_impr_indices(const std::vector<uint32_t>& vec) {
    auto it = gen_import_table.find(vec);
    if(it == gen_import_table.end()) {
      gen_import_items.push_back(vec);
      gen_import_table.insert({vec, gen_import_items.size() - 1});
      return gen_import_items.size() - 1;
    } else {
      return it->second;
    }
  }

  uint32_t get_or_add_impr(std::string impr) {
    std::vector<uint32_t> vec;
    for(auto str : std::views::split(impr, '/')) {
      std::string foo = std::string(str.begin(), str.end());
      vec.push_back(get_or_add_tran(foo));
    }
    return get_or_add_impr_indices(vec);
  }

  uint32_t get_or_add_expr_indices(const std::vector<uint32_t>& vec, uint32_t object_idx) {
    auto it = gen_export_table.find(vec);
    if(it == gen_export_table.end()) {
      gen_export_items.push_back(vec);
      gen_export_table.insert({vec, object_idx});
      return gen_export_items.size() - 1;
    } else {
      return it->second;
    }
  }

  uint32_t get_or_add_expr(std::string expr, uint32_t object_idx) {
    std::vector<uint32_t> vec;
    for(auto str : std::views::split(expr, '/')) {
      std::string foo = std::string(str.begin(), str.end());
      vec.push_back(get_or_add_tran(foo));
    }
    return get_or_add_expr_indices(vec, object_idx);
  }

  void save_as_ndfbin_xml(fs::path);
  void save_as_ndfbin(fs::path);
};


