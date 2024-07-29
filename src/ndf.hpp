#pragma once

#include <cassert>
#include <vector>
#include <map>
#include <set>
#include <ranges>

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

enum NDFPropertyType {
  Bool = 0x0,
  Int8 = 0x1,
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
  S16 = 0x18,
  U16 = 0x19,
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
  static std::unique_ptr<NDFProperty> get_property_from_ndftype(uint32_t ndf_type);
  static std::unique_ptr<NDFProperty> get_property_from_ndf_xml(uint32_t ndf_type, const pugi::xml_node& ndf_node);
  static std::unique_ptr<NDFProperty> get_property_from_ndfbin(uint32_t ndf_type, std::istream& stream);
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
  virtual bool is_object_reference() {
    return false;
  }
  virtual bool is_import_reference() {
    return false;
  }
  virtual std::unique_ptr<NDFProperty> get_copy() = 0;
};


struct NDFPropertyBool : NDFProperty {
  bool value;
  NDFPropertyBool() {
    property_type = NDFPropertyType::Bool;
  }
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
public:
  std::unique_ptr<NDFProperty> get_copy() override {
    return std::make_unique<NDFPropertyBool>(*this);
  }
};

struct NDFPropertyInt8 : NDFProperty {
  uint8_t value;
  NDFPropertyInt8() {
    property_type = NDFPropertyType::Int8;
  }
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
public:
  std::unique_ptr<NDFProperty> get_copy() override {
    return std::make_unique<NDFPropertyInt8>(*this);
  }
};

struct NDFPropertyInt32 : NDFProperty {
  int32_t value;
  NDFPropertyInt32() {
    property_type = NDFPropertyType::Int32;
}
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
public:
  std::unique_ptr<NDFProperty> get_copy() override {
    return std::make_unique<NDFPropertyInt32>(*this);
  }
};

struct NDFPropertyUInt32 : NDFProperty {
  uint32_t value;
  NDFPropertyUInt32() {
    property_type = NDFPropertyType::UInt32;
  }
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
public:
  std::unique_ptr<NDFProperty> get_copy() override {
    return std::make_unique<NDFPropertyUInt32>(*this);
  }
};

struct NDFPropertyFloat32 : NDFProperty {
  float value;
  NDFPropertyFloat32() {
    property_type = NDFPropertyType::Float32;
  }
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
public:
  std::unique_ptr<NDFProperty> get_copy() override {
    return std::make_unique<NDFPropertyFloat32>(*this);
  }
};

struct NDFPropertyFloat64 : NDFProperty {
  double value;
  NDFPropertyFloat64() {
    property_type = NDFPropertyType::Float64;
  }
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
public:
  std::unique_ptr<NDFProperty> get_copy() override {
    return std::make_unique<NDFPropertyFloat64>(*this);
  }
};

struct NDFPropertyString : NDFProperty {
  std::string value;
  NDFPropertyString() {
    property_type = NDFPropertyType::String;
  }
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
public:
  std::unique_ptr<NDFProperty> get_copy() override {
    return std::make_unique<NDFPropertyString>(*this);
  }
};

struct NDFPropertyWideString : NDFProperty {
  std::string value;
  NDFPropertyWideString() {
    property_type = NDFPropertyType::WideString;
  }
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
public:
  std::unique_ptr<NDFProperty> get_copy() override {
    return std::make_unique<NDFPropertyWideString>(*this);
  }
};

struct NDFPropertyF32_vec3 : NDFProperty {
  float x;
  float y;
  float z;
  NDFPropertyF32_vec3() {
    property_type = NDFPropertyType::F32_vec3;
  }
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
public:
  std::unique_ptr<NDFProperty> get_copy() override {
    return std::make_unique<NDFPropertyF32_vec3>(*this);
  }
};

struct NDFPropertyF32_vec4 : NDFProperty {
  float x;
  float y;
  float z;
  float w;
  NDFPropertyF32_vec4() {
    property_type = NDFPropertyType::F32_vec4;
  }
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
public:
  std::unique_ptr<NDFProperty> get_copy() override {
    return std::make_unique<NDFPropertyF32_vec4>(*this);
  }
};

struct NDFPropertyColor : NDFProperty {
  uint8_t r;
  uint8_t g;
  uint8_t b;
  uint8_t a;
  NDFPropertyColor() {
    property_type = NDFPropertyType::Color;
  }
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
public:
  std::unique_ptr<NDFProperty> get_copy() override {
    return std::make_unique<NDFPropertyColor>(*this);
  }
};

struct NDFPropertyS32_vec3 : NDFProperty {
  int32_t x;
  int32_t y;
  int32_t z;
  NDFPropertyS32_vec3() {
    property_type = NDFPropertyType::S32_vec3;
  }
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
public:
  std::unique_ptr<NDFProperty> get_copy() override {
    return std::make_unique<NDFPropertyS32_vec3>(*this);
  }
};

enum ReferenceType {
  Import = 2863311530,
  Object = 3149642683
};

struct NDFPropertyObjectReference : NDFProperty {
  std::string object_name;
  NDFPropertyObjectReference() {
    property_type = NDFPropertyType::ObjectReference;
  }
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
  bool is_object_reference() override {
    return true;
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
public:
  std::unique_ptr<NDFProperty> get_copy() override {
    return std::make_unique<NDFPropertyObjectReference>(*this);
  }
};

struct NDFPropertyImportReference : NDFProperty {
  std::string import_name;
  NDFPropertyImportReference() {
    property_type = NDFPropertyType::ImportReference;
  }
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
  bool is_import_reference() override {
    return true;
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
public:
  std::unique_ptr<NDFProperty> get_copy() override {
    return std::make_unique<NDFPropertyImportReference>(*this);
  }
};

struct NDFPropertyList : NDFProperty {
  std::vector<std::unique_ptr<NDFProperty>> values;
  NDFPropertyList() {
    property_type = NDFPropertyType::List;
  }
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
public:
  std::unique_ptr<NDFProperty> get_copy() override {
    auto ret = std::make_unique<NDFPropertyList>();
    for(auto const &value : values) {
      ret->values.push_back(value->get_copy());
    }
    return ret;
  }
};

struct NDFPropertyMap : NDFProperty {
  std::vector<std::pair<std::unique_ptr<NDFProperty>, std::unique_ptr<NDFProperty>>> values;
  NDFPropertyMap() {
    property_type = 0x12;
  }
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
public:
  std::unique_ptr<NDFProperty> get_copy() override {
    auto ret = std::make_unique<NDFPropertyMap>();
    for(auto const &[key, value] : values) {
      ret->values.push_back({key->get_copy(), value->get_copy()});
    }
    return ret;
  }
};

struct NDFPropertyS16 : NDFProperty {
  int16_t value;
  NDFPropertyS16() {
    property_type = NDFPropertyType::S16;
  }
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
public:
  std::unique_ptr<NDFProperty> get_copy() override {
    return std::make_unique<NDFPropertyS16>(*this);
  }
};

struct NDFPropertyU16 : NDFProperty {
  uint16_t value;
  NDFPropertyU16() {
    property_type = NDFPropertyType::U16;
  }
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
public:
  std::unique_ptr<NDFProperty> get_copy() override {
    return std::make_unique<NDFPropertyU16>(*this);
  }
};

// FIXME: error checking for GUIDs?
struct NDFPropertyGUID : NDFProperty {
  std::string guid;
  NDFPropertyGUID() {
    property_type = NDFPropertyType::NDFGUID;
  }
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
public:
  std::unique_ptr<NDFProperty> get_copy() override {
    return std::make_unique<NDFPropertyGUID>(*this);
  }
};

struct NDFPropertyPathReference : NDFProperty {
  std::string path;
  NDFPropertyPathReference() {
    property_type = NDFPropertyType::PathReference;
  }
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
public:
  std::unique_ptr<NDFProperty> get_copy() override {
    return std::make_unique<NDFPropertyPathReference>(*this);
  }
};

// FIXME: error checking for hashes?
struct NDFPropertyLocalisationHash : NDFProperty {
  std::string hash;
  NDFPropertyLocalisationHash() {
    property_type = NDFPropertyType::LocalisationHash;
  }
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
public:
  std::unique_ptr<NDFProperty> get_copy() override {
    return std::make_unique<NDFPropertyLocalisationHash>(*this);
  }
};

struct NDFPropertyS32_vec2 : NDFProperty {
  int32_t x;
  int32_t y;
  NDFPropertyS32_vec2() {
    property_type = NDFPropertyType::S32_vec2;
  }
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
public:
  std::unique_ptr<NDFProperty> get_copy() override {
    return std::make_unique<NDFPropertyS32_vec2>(*this);
  }
};

struct NDFPropertyF32_vec2 : NDFProperty {
  float x;
  float y;
  NDFPropertyF32_vec2() {
    property_type = NDFPropertyType::F32_vec2;
  }
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
public:
  std::unique_ptr<NDFProperty> get_copy() override {
    return std::make_unique<NDFPropertyF32_vec2>(*this);
  }
};

struct NDFPropertyPair : NDFProperty {
  std::unique_ptr<NDFProperty> first;
  std::unique_ptr<NDFProperty> second;
  NDFPropertyPair() {
    property_type = NDFPropertyType::Pair;
  }
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
public:
  std::unique_ptr<NDFProperty> get_copy() override {
    auto ret = std::make_unique<NDFPropertyPair>();
    ret->first = first->get_copy();
    ret->second = second->get_copy();
    return ret;
  }
};

// FIXME: error checking for hashes?
struct NDFPropertyHash : NDFProperty {
  std::string hash;
  NDFPropertyHash() {
    property_type = NDFPropertyType::Hash;
  }
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
  struct NDF_Hash {
    uint8_t hash[16];
  };
  #pragma pack(pop)
public:
  void from_ndfbin(NDF*, std::istream& stream) override {
    NDF_Hash ndf_hash;
    stream.read(reinterpret_cast<char*>(&ndf_hash), sizeof(NDF_Hash));
    hash = "";
    for(auto const &byte : ndf_hash.hash) {
      hash += std::format("{:02X}", byte);
    }
  }
  void to_ndfbin(NDF*, std::ostream& stream) override {
    NDF_Hash ndf_hash;
    for(uint32_t i = 0; i < 8; i++) {
      std::from_chars(hash.c_str() + i * 2, hash.c_str() + i * 2 + 2, ndf_hash.hash[i], 16);
    }
    stream.write(reinterpret_cast<char*>(&ndf_hash), sizeof(NDF_Hash));
  }
public:
  std::unique_ptr<NDFProperty> get_copy() override {
    return std::make_unique<NDFPropertyHash>(*this);
  }
};

struct NDFObject {
  std::string name;
  std::string class_name;
  bool is_top_object;
  std::string export_path;
  std::vector<std::unique_ptr<NDFProperty>> properties;
  std::map<std::string, uint32_t> property_map;
  std::unique_ptr<NDFProperty>& get_property(const std::string& name) {
    return properties.at(property_map.at(name));
  }
public:
  NDFObject get_copy() {
    NDFObject ret;
    ret.name = name;
    ret.class_name = class_name;
    ret.is_top_object = is_top_object;
    ret.export_path = export_path;
    for(auto const &prop : properties) {
      ret.properties.push_back(prop->get_copy());
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
  void load_imprs(std::istream &stream, std::vector<std::string> current_import_path);
  void load_exprs(std::istream &stream, std::vector<std::string> current_export_path);
  void load_from_ndf_xml(fs::path path);
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

  void save_ndfbin_imprs(const std::map<std::vector<uint32_t>, uint32_t>& gen_table, std::ostream& stream);

  // gets called by every save_as_ndfbin
  void fill_gen_object() {
    for(const auto& [idx, it] : object_map | std::views::enumerate) {
      gen_object_items.push_back(it.second.name);
      gen_object_table.insert({it.second.name, gen_object_items.size()-1});
    }
  }

  // used for object reference
  uint32_t get_object_index(const std::string& name) {
    auto obj_idx = gen_object_table.find(name);
    if(obj_idx == gen_object_table.end()) {
      return 4294967295;
    }
    return obj_idx->second;
  }
  // used for object references
  uint32_t get_class_of_object(const std::string& name) {
    auto object_idx = get_object_index(name);
    if(object_idx == 4294967295) {
      return 4294967295;
    }
    return get_class(get_object(name).class_name);
  }

  uint32_t get_class(const std::string& str) {
    auto obj_idx = gen_clas_items.find(str);
    if(obj_idx == gen_clas_items.end()) {
      return 4294967295;
    }
    return obj_idx->second;
  }
  friend class NDFPropertyObjectReference;
  friend class NDFPropertyImportReference;

public:
  NDFObject& get_object(const std::string& str) {
    return object_map.at(str);
  }

  bool change_object_name(const std::string& previous_name, const std::string& name) {
    if(object_map.contains(name)) {
      spdlog::warn("change_object_name: object {} does already exist", name);
      return false;
    }
    if(!object_map.contains(previous_name)) {
      spdlog::warn("change_object_name: object {} does not exist", previous_name);
      return false;
    }
    auto& object = get_object(previous_name);
    const auto it = object_map.find(previous_name);
    object_map[name] = object.get_copy();
    object_map[name].name = name;
    object_map.erase(it);
    return true;
  }

  bool copy_object(const std::string& obj_name, const std::string& new_name) {
    if(object_map.contains(new_name)) {
      spdlog::warn("copy_object: object {} does already exist", new_name);
      return false;
    }
    if(!object_map.contains(obj_name)) {
      spdlog::warn("copy_object: object {} does not exist", obj_name);
      return false;
    }
    auto& object = get_object(obj_name);
    auto new_object = object.get_copy();
    new_object.name = new_name;
    object_map.insert({new_name, std::move(new_object)});
    return true;
  }

  bool remove_object(const std::string& name) {
    if(!object_map.contains(name)) {
      spdlog::warn("remove_object: object {} does not exist", name);
      return false;
    }
    object_map.erase(name);
    return true;
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

  void load_from_ndfbin(fs::path path);
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


