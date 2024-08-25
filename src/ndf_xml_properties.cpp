#include "ndf.hpp"
#include "utf.hpp"

std::unique_ptr<NDFProperty>
NDFProperty::get_property_from_ndftype(uint32_t ndf_type) {
  switch (ndf_type) {
  case 0x0: {
    return std::make_unique<NDFPropertyBool>();
  }
  case 0x1: {
    return std::make_unique<NDFPropertyUInt8>();
  }
  case 0x2: {
    return std::make_unique<NDFPropertyInt32>();
  }
  case 0x3: {
    return std::make_unique<NDFPropertyUInt32>();
  }
  case 0x5: {
    return std::make_unique<NDFPropertyFloat32>();
  }
  case 0x6: {
    return std::make_unique<NDFPropertyFloat64>();
  }
  case 0x7: {
    return std::make_unique<NDFPropertyString>();
  }
  case 0x8: {
    return std::make_unique<NDFPropertyWideString>();
  }
  case 0xB: {
    return std::make_unique<NDFPropertyF32_vec3>();
  }
  case 0xC: {
    return std::make_unique<NDFPropertyF32_vec4>();
  }
  case 0xD: {
    return std::make_unique<NDFPropertyColor>();
  }
  case 0xE: {
    return std::make_unique<NDFPropertyS32_vec3>();
  }
  case 0x11: {
    return std::make_unique<NDFPropertyList>();
  }
  case 0x12: {
    return std::make_unique<NDFPropertyMap>();
  }
  case 0x18: {
    return std::make_unique<NDFPropertyInt16>();
  }
  case 0x19: {
    return std::make_unique<NDFPropertyUInt16>();
  }
  case 0x1A: {
    return std::make_unique<NDFPropertyGUID>();
  }
  case 0x1C: {
    return std::make_unique<NDFPropertyPathReference>();
  }
  case 0x1D: {
    return std::make_unique<NDFPropertyLocalisationHash>();
  }
  case 0x1F: {
    return std::make_unique<NDFPropertyS32_vec2>();
  }
  case 0x21: {
    return std::make_unique<NDFPropertyF32_vec2>();
  }
  case 0x22: {
    return std::make_unique<NDFPropertyPair>();
  }
  case 0x25: {
    return std::make_unique<NDFPropertyHash>();
  }
  default: {
    throw std::runtime_error(std::format("Unknown NDFType: {}", ndf_type));
  }
  }
}

std::unique_ptr<NDFProperty>
NDFProperty::get_property_from_ndf_xml(uint32_t ndf_type,
                                       const pugi::xml_node &ndf_node) {
  if (ndf_type == 0x9) {
    std::string reference_type =
        ndf_node.attribute("referenceType").as_string();
    if (reference_type == "object") {
      return std::make_unique<NDFPropertyObjectReference>();
    } else if (reference_type == "import") {
      return std::make_unique<NDFPropertyImportReference>();
    } else {
      throw std::runtime_error(
          std::format("Unknown ReferenceType: {}", reference_type));
    }
  }
  return get_property_from_ndftype(ndf_type);
}

void NDFPropertyBool::to_ndf_xml(pugi::xml_node &node) const {
  auto bool_node = node.append_child(property_name.c_str());
  bool_node.append_attribute("value").set_value(value);
  bool_node.append_attribute("typeId").set_value(property_type);
}
void NDFPropertyBool::from_ndf_xml(const pugi::xml_node &node) {
  property_name = node.name();
  value = node.attribute("value").as_bool();
  assert(node.attribute("typeId").as_uint() == property_type);
}

void NDFPropertyUInt8::to_ndf_xml(pugi::xml_node &node) const {
  auto int8_node = node.append_child(property_name.c_str());
  int8_node.append_attribute("value").set_value(value);
  int8_node.append_attribute("typeId").set_value(property_type);
}
void NDFPropertyUInt8::from_ndf_xml(const pugi::xml_node &node) {
  property_name = node.name();
  value = node.attribute("value").as_int();
  assert(node.attribute("typeId").as_uint() == property_type);
}

void NDFPropertyInt32::to_ndf_xml(pugi::xml_node &node) const {
  auto int32_node = node.append_child(property_name.c_str());
  int32_node.append_attribute("value").set_value(value);
  int32_node.append_attribute("typeId").set_value(property_type);
}
void NDFPropertyInt32::from_ndf_xml(const pugi::xml_node &node) {
  property_name = node.name();
  value = node.attribute("value").as_int();
  assert(node.attribute("typeId").as_uint() == property_type);
}

void NDFPropertyUInt32::to_ndf_xml(pugi::xml_node &node) const {
  auto uint32_node = node.append_child(property_name.c_str());
  uint32_node.append_attribute("value").set_value(value);
  uint32_node.append_attribute("typeId").set_value(property_type);
}
void NDFPropertyUInt32::from_ndf_xml(const pugi::xml_node &node) {
  property_name = node.name();
  value = node.attribute("value").as_uint();
  assert(node.attribute("typeId").as_uint() == property_type);
}

void NDFPropertyFloat32::to_ndf_xml(pugi::xml_node &node) const {
  auto float32_node = node.append_child(property_name.c_str());
  float32_node.append_attribute("value").set_value(value);
  float32_node.append_attribute("typeId").set_value(property_type);
}
void NDFPropertyFloat32::from_ndf_xml(const pugi::xml_node &node) {
  property_name = node.name();
  value = node.attribute("value").as_float();
  assert(node.attribute("typeId").as_uint() == property_type);
}

void NDFPropertyFloat64::to_ndf_xml(pugi::xml_node &node) const {
  auto float64_node = node.append_child(property_name.c_str());
  float64_node.append_attribute("value").set_value(value);
  float64_node.append_attribute("typeId").set_value(property_type);
}
void NDFPropertyFloat64::from_ndf_xml(const pugi::xml_node &node) {
  property_name = node.name();
  value = node.attribute("value").as_double();
  assert(node.attribute("typeId").as_uint() == property_type);
}

void NDFPropertyString::to_ndf_xml(pugi::xml_node &node) const {
  auto string_node = node.append_child(property_name.c_str());
  string_node.append_attribute("value").set_value(value.c_str());
  string_node.append_attribute("typeId").set_value(property_type);
}
void NDFPropertyString::from_ndf_xml(const pugi::xml_node &node) {
  property_name = node.name();
  value = node.attribute("value").as_string();
  assert(node.attribute("typeId").as_uint() == property_type);
}

void NDFPropertyWideString::to_ndf_xml(pugi::xml_node &node) const {
  auto wide_string_node = node.append_child(property_name.c_str());
  wide_string_node.append_attribute("str").set_value(value.c_str());
  wide_string_node.append_attribute("typeId").set_value(property_type);
}
void NDFPropertyWideString::from_ndf_xml(const pugi::xml_node &node) {
  property_name = node.name();
  value = node.attribute("str").as_string();
  assert(node.attribute("typeId").as_uint() == property_type);
}

void NDFPropertyF32_vec3::to_ndf_xml(pugi::xml_node &node) const {
  auto f32_vec3_node = node.append_child(property_name.c_str());
  f32_vec3_node.append_attribute("x").set_value(x);
  f32_vec3_node.append_attribute("y").set_value(y);
  f32_vec3_node.append_attribute("z").set_value(z);
  f32_vec3_node.append_attribute("typeId").set_value(property_type);
}
void NDFPropertyF32_vec3::from_ndf_xml(const pugi::xml_node &node) {
  property_name = node.name();
  x = node.attribute("x").as_float();
  y = node.attribute("y").as_float();
  z = node.attribute("z").as_float();
  assert(node.attribute("typeId").as_uint() == property_type);
}

void NDFPropertyF32_vec4::to_ndf_xml(pugi::xml_node &node) const {
  auto f32_vec4_node = node.append_child(property_name.c_str());
  f32_vec4_node.append_attribute("x").set_value(x);
  f32_vec4_node.append_attribute("y").set_value(y);
  f32_vec4_node.append_attribute("z").set_value(z);
  f32_vec4_node.append_attribute("w").set_value(w);
  f32_vec4_node.append_attribute("typeId").set_value(property_type);
}
void NDFPropertyF32_vec4::from_ndf_xml(const pugi::xml_node &node) {
  property_name = node.name();
  x = node.attribute("x").as_float();
  y = node.attribute("y").as_float();
  z = node.attribute("z").as_float();
  w = node.attribute("w").as_float();
  assert(node.attribute("typeId").as_uint() == property_type);
}

void NDFPropertyColor::to_ndf_xml(pugi::xml_node &node) const {
  auto color_node = node.append_child(property_name.c_str());
  color_node.append_attribute("r").set_value(r);
  color_node.append_attribute("g").set_value(g);
  color_node.append_attribute("b").set_value(b);
  color_node.append_attribute("a").set_value(a);
  color_node.append_attribute("typeId").set_value(property_type);
}
void NDFPropertyColor::from_ndf_xml(const pugi::xml_node &node) {
  property_name = node.name();
  r = node.attribute("r").as_uint();
  g = node.attribute("g").as_uint();
  b = node.attribute("b").as_uint();
  a = node.attribute("a").as_uint();
  assert(node.attribute("typeId").as_uint() == property_type);
}

void NDFPropertyS32_vec3::to_ndf_xml(pugi::xml_node &node) const {
  auto s32_vec3_node = node.append_child(property_name.c_str());
  s32_vec3_node.append_attribute("x").set_value(x);
  s32_vec3_node.append_attribute("y").set_value(y);
  s32_vec3_node.append_attribute("z").set_value(z);
  s32_vec3_node.append_attribute("typeId").set_value(property_type);
}
void NDFPropertyS32_vec3::from_ndf_xml(const pugi::xml_node &node) {
  property_name = node.name();
  x = node.attribute("x").as_int();
  y = node.attribute("y").as_int();
  z = node.attribute("z").as_int();
  assert(node.attribute("typeId").as_uint() == property_type);
}

void NDFPropertyObjectReference::to_ndf_xml(pugi::xml_node &node) const {
  auto reference_node = node.append_child(property_name.c_str());
  reference_node.append_attribute("object").set_value(object_name.c_str());
  reference_node.append_attribute("typeId").set_value(property_type);
  reference_node.append_attribute("referenceType").set_value("object");
}
void NDFPropertyObjectReference::from_ndf_xml(const pugi::xml_node &node) {
  property_name = node.name();
  object_name = node.attribute("object").as_string();
  assert(node.attribute("typeId").as_uint() == property_type);
  assert(node.attribute("referenceType").as_string() == std::string("object"));
}

void NDFPropertyImportReference::to_ndf_xml(pugi::xml_node &node) const {
  auto reference_node = node.append_child(property_name.c_str());
  reference_node.append_attribute("import").set_value(import_name.c_str());
  reference_node.append_attribute("typeId").set_value(property_type);
  reference_node.append_attribute("referenceType").set_value("import");
}
void NDFPropertyImportReference::from_ndf_xml(const pugi::xml_node &node) {
  property_name = node.name();
  import_name = node.attribute("import").as_string();
  assert(node.attribute("typeId").as_uint() == property_type);
  assert(node.attribute("referenceType").as_string() == std::string("import"));
}

void NDFPropertyList::to_ndf_xml(pugi::xml_node &node) const {
  auto list_node = node.append_child(property_name.c_str());
  list_node.append_attribute("typeId").set_value(property_type);
  for (auto const &value : values) {
    value->to_ndf_xml(list_node);
  }
}
void NDFPropertyList::from_ndf_xml(const pugi::xml_node &node) {
  property_name = node.name();
  for (auto const &value_node : node.children()) {
    values.push_back(get_property_from_ndf_xml(
        value_node.attribute("typeId").as_uint(), value_node));
    values.back()->from_ndf_xml(value_node);
  }
}

void NDFPropertyMap::to_ndf_xml(pugi::xml_node &node) const {
  auto map_node = node.append_child(property_name.c_str());
  map_node.append_attribute("typeId").set_value(property_type);
  for (auto const &[key, value] : values) {
    auto map_items_node = map_node.append_child("MapItem");
    key->to_ndf_xml(map_items_node);
    value->to_ndf_xml(map_items_node);
  }
}
void NDFPropertyMap::from_ndf_xml(const pugi::xml_node &node) {
  property_name = node.name();
  for (auto const &map_item_node : node.children()) {
    auto key_node = map_item_node.child("Key");
    auto value_node = map_item_node.child("Value");

    auto key = get_property_from_ndf_xml(key_node.attribute("typeId").as_uint(),
                                         key_node);
    key->from_ndf_xml(key_node);

    auto value = get_property_from_ndf_xml(
        value_node.attribute("typeId").as_uint(), value_node);
    value->from_ndf_xml(value_node);

    values.push_back({std::move(key), std::move(value)});
  }
}

void NDFPropertyInt16::to_ndf_xml(pugi::xml_node &node) const {
  auto s16_node = node.append_child(property_name.c_str());
  s16_node.append_attribute("value").set_value(value);
  s16_node.append_attribute("typeId").set_value(property_type);
}
void NDFPropertyInt16::from_ndf_xml(const pugi::xml_node &node) {
  property_name = node.name();
  value = node.attribute("value").as_int();
  assert(node.attribute("typeId").as_uint() == property_type);
}

void NDFPropertyUInt16::to_ndf_xml(pugi::xml_node &node) const {
  auto u16_node = node.append_child(property_name.c_str());
  u16_node.append_attribute("value").set_value(value);
  u16_node.append_attribute("typeId").set_value(property_type);
}
void NDFPropertyUInt16::from_ndf_xml(const pugi::xml_node &node) {
  property_name = node.name();
  value = node.attribute("value").as_uint();
  assert(node.attribute("typeId").as_uint() == property_type);
}

void NDFPropertyGUID::to_ndf_xml(pugi::xml_node &node) const {
  auto guid_node = node.append_child(property_name.c_str());
  guid_node.append_attribute("guid").set_value(guid.c_str());
  guid_node.append_attribute("typeId").set_value(property_type);
}
void NDFPropertyGUID::from_ndf_xml(const pugi::xml_node &node) {
  property_name = node.name();
  guid = node.attribute("guid").as_string();
  assert(node.attribute("typeId").as_uint() == property_type);
}

void NDFPropertyPathReference::to_ndf_xml(pugi::xml_node &node) const {
  auto path_node = node.append_child(property_name.c_str());
  path_node.append_attribute("typeId").set_value(property_type);
  path_node.append_attribute("path").set_value(path.c_str());
}
void NDFPropertyPathReference::from_ndf_xml(const pugi::xml_node &node) {
  property_name = node.name();
  path = node.attribute("path").as_string();
  assert(node.attribute("typeId").as_uint() == property_type);
}

void NDFPropertyLocalisationHash::to_ndf_xml(pugi::xml_node &node) const {
  auto hash_node = node.append_child(property_name.c_str());
  hash_node.append_attribute("hash").set_value(hash.c_str());
  hash_node.append_attribute("typeId").set_value(property_type);
}
void NDFPropertyLocalisationHash::from_ndf_xml(const pugi::xml_node &node) {
  property_name = node.name();
  hash = node.attribute("hash").as_string();
  assert(node.attribute("typeId").as_uint() == property_type);
}

void NDFPropertyS32_vec2::to_ndf_xml(pugi::xml_node &node) const {
  auto s32_vec2_node = node.append_child(property_name.c_str());
  s32_vec2_node.append_attribute("x").set_value(x);
  s32_vec2_node.append_attribute("y").set_value(y);
  s32_vec2_node.append_attribute("typeId").set_value(property_type);
}
void NDFPropertyS32_vec2::from_ndf_xml(const pugi::xml_node &node) {
  property_name = node.name();
  x = node.attribute("x").as_int();
  y = node.attribute("y").as_int();
  assert(node.attribute("typeId").as_uint() == property_type);
}

void NDFPropertyF32_vec2::to_ndf_xml(pugi::xml_node &node) const {
  auto f32_vec2_node = node.append_child(property_name.c_str());
  f32_vec2_node.append_attribute("x").set_value(x);
  f32_vec2_node.append_attribute("y").set_value(y);
  f32_vec2_node.append_attribute("typeId").set_value(property_type);
}
void NDFPropertyF32_vec2::from_ndf_xml(const pugi::xml_node &node) {
  property_name = node.name();
  x = node.attribute("x").as_float();
  y = node.attribute("y").as_float();
  assert(node.attribute("typeId").as_uint() == property_type);
}

void NDFPropertyPair::to_ndf_xml(pugi::xml_node &node) const {
  auto pair_node = node.append_child(property_name.c_str());
  pair_node.append_attribute("typeId").set_value(property_type);
  first->to_ndf_xml(pair_node);
  second->to_ndf_xml(pair_node);
}
void NDFPropertyPair::from_ndf_xml(const pugi::xml_node &node) {
  property_name = node.name();
  assert(node.attribute("typeId").as_uint() == property_type);
  auto first_node = node.child("First");
  auto second_node = node.child("Second");

  first = get_property_from_ndf_xml(first_node.attribute("typeId").as_uint(),
                                    first_node);
  first->from_ndf_xml(first_node);

  second = get_property_from_ndf_xml(second_node.attribute("typeId").as_uint(),
                                     second_node);
  second->from_ndf_xml(second_node);
}

void NDFPropertyHash::to_ndf_xml(pugi::xml_node &node) const {
  auto hash_node = node.append_child(property_name.c_str());
  hash_node.append_attribute("hash").set_value(hash.c_str());
  hash_node.append_attribute("typeId").set_value(property_type);
}
void NDFPropertyHash::from_ndf_xml(const pugi::xml_node &node) {
  property_name = node.name();
  hash = node.attribute("hash").as_string();
  assert(node.attribute("typeId").as_uint() == property_type);
}
