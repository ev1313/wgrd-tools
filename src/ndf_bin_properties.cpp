#include "ndf.hpp"
#include "utf.hpp"

std::unique_ptr<NDFProperty>
NDFProperty::get_property_from_ndfbin(uint32_t ndf_type, std::istream &stream) {
  spdlog::debug("NDFType: {} @0x{:02X}", ndf_type, (uint32_t)stream.tellg());
  if (ndf_type == 0x9) {
    uint32_t reference_type;
    stream.read(reinterpret_cast<char *>(&reference_type), sizeof(uint32_t));
    if (reference_type == ReferenceType::Object) {
      return std::make_unique<NDFPropertyObjectReference>();
    } else if (reference_type == ReferenceType::Import) {
      return std::make_unique<NDFPropertyImportReference>();
    } else {
      throw std::runtime_error(
          std::format("Unknown ReferenceType: {}", reference_type));
    }
  }
  return get_property_from_ndftype(ndf_type);
}

#pragma pack(push, 1)
struct NDF_Bool {
  uint8_t value;
};
#pragma pack(pop)

void NDFPropertyBool::from_ndfbin(NDF *, std::istream &stream) {
  NDF_Bool ndf_bool;
  stream.read(reinterpret_cast<char *>(&ndf_bool), sizeof(NDF_Bool));
  value = ndf_bool.value;
}
void NDFPropertyBool::to_ndfbin(NDF *, std::ostream &stream) {
  NDF_Bool ndf_bool;
  ndf_bool.value = value;
  stream.write(reinterpret_cast<char *>(&ndf_bool), sizeof(NDF_Bool));
}

#pragma pack(push, 1)
struct NDF_UInt8 {
  uint8_t value;
};
#pragma pack(pop)

void NDFPropertyUInt8::from_ndfbin(NDF *, std::istream &stream) {
  NDF_UInt8 ndf_int8;
  stream.read(reinterpret_cast<char *>(&ndf_int8), sizeof(NDF_UInt8));
  value = ndf_int8.value;
}
void NDFPropertyUInt8::to_ndfbin(NDF *, std::ostream &stream) {
  NDF_UInt8 ndf_int8;
  ndf_int8.value = value;
  stream.write(reinterpret_cast<char *>(&ndf_int8), sizeof(NDF_UInt8));
}

#pragma pack(push, 1)
struct NDF_Int32 {
  int32_t value;
};
#pragma pack(pop)

void NDFPropertyInt32::from_ndfbin(NDF *, std::istream &stream) {
  NDF_Int32 ndf_int32;
  stream.read(reinterpret_cast<char *>(&ndf_int32), sizeof(NDF_Int32));
  value = ndf_int32.value;
}
void NDFPropertyInt32::to_ndfbin(NDF *, std::ostream &stream) {
  NDF_Int32 ndf_int32;
  ndf_int32.value = value;
  stream.write(reinterpret_cast<char *>(&ndf_int32), sizeof(NDF_Int32));
}

#pragma pack(push, 1)
struct NDF_UInt32 {
  uint32_t value;
};
#pragma pack(pop)

void NDFPropertyUInt32::from_ndfbin(NDF *, std::istream &stream) {
  NDF_UInt32 ndf_uint32;
  stream.read(reinterpret_cast<char *>(&ndf_uint32), sizeof(NDF_UInt32));
  value = ndf_uint32.value;
}
void NDFPropertyUInt32::to_ndfbin(NDF *, std::ostream &stream) {
  NDF_UInt32 ndf_uint32;
  ndf_uint32.value = value;
  stream.write(reinterpret_cast<char *>(&ndf_uint32), sizeof(NDF_UInt32));
}

#pragma pack(push, 1)
struct NDF_Float32 {
  float value;
};
#pragma pack(pop)

void NDFPropertyFloat32::from_ndfbin(NDF *, std::istream &stream) {
  NDF_Float32 ndf_float32;
  stream.read(reinterpret_cast<char *>(&ndf_float32), sizeof(NDF_Float32));
  value = ndf_float32.value;
}
void NDFPropertyFloat32::to_ndfbin(NDF *, std::ostream &stream) {
  NDF_Float32 ndf_float32;
  ndf_float32.value = value;
  stream.write(reinterpret_cast<char *>(&ndf_float32), sizeof(NDF_Float32));
}

#pragma pack(push, 1)
struct NDF_Float64 {
  double value;
};
#pragma pack(pop)

void NDFPropertyFloat64::from_ndfbin(NDF *, std::istream &stream) {
  NDF_Float64 ndf_float64;
  stream.read(reinterpret_cast<char *>(&ndf_float64), sizeof(NDF_Float64));
  value = ndf_float64.value;
}
void NDFPropertyFloat64::to_ndfbin(NDF *, std::ostream &stream) {
  NDF_Float64 ndf_float64;
  ndf_float64.value = value;
  stream.write(reinterpret_cast<char *>(&ndf_float64), sizeof(NDF_Float64));
}

#pragma pack(push, 1)
struct NDF_String {
  uint32_t string_index;
};
#pragma pack(pop)

void NDFPropertyString::from_ndfbin(NDF *root, std::istream &stream) {
  NDF_String ndf_string;
  stream.read(reinterpret_cast<char *>(&ndf_string), sizeof(NDF_String));
  value = root->string_table[ndf_string.string_index];
}

void NDFPropertyString::to_ndfbin(NDF *root, std::ostream &stream) {
  NDF_String ndf_string;
  ndf_string.string_index = root->get_or_add_string(value);
  stream.write(reinterpret_cast<char *>(&ndf_string), sizeof(NDF_String));
}

#pragma pack(push, 1)
struct NDF_WideString {
  uint32_t length;
};
#pragma pack(pop)

void NDFPropertyWideString::from_ndfbin(NDF *, std::istream &stream) {
  NDF_WideString ndf_wide_string;
  stream.read(reinterpret_cast<char *>(&ndf_wide_string),
              sizeof(NDF_WideString));
  assert(ndf_wide_string.length % 2 == 0);
  std::u16string str;
  for (uint32_t i = 0; i < ndf_wide_string.length / 2; i++) {
    char16_t c;
    stream.read(reinterpret_cast<char *>(&c), sizeof(char16_t));
    str.push_back(c);
  }
  // convert to UTF-8
  value = Utf32To8(Utf16To32(str));
  spdlog::debug("WideString: {}", value);
}

void NDFPropertyWideString::to_ndfbin(NDF *, std::ostream &stream) {
  NDF_WideString ndf_wide_string;
  ndf_wide_string.length = value.size() * 2;
  stream.write(reinterpret_cast<char *>(&ndf_wide_string),
               sizeof(NDF_WideString));
  // convert to UTF-16
  std::u16string buffer = Utf32To16(Utf8To32(value));
  stream.write((char *)buffer.data(), buffer.size() * sizeof(char16_t));
}

#pragma pack(push, 1)
struct NDF_F32_vec3 {
  float x;
  float y;
  float z;
};
#pragma pack(pop)

void NDFPropertyF32_vec3::from_ndfbin(NDF *, std::istream &stream) {
  NDF_F32_vec3 ndf_f32_vec3;
  stream.read(reinterpret_cast<char *>(&ndf_f32_vec3), sizeof(NDF_F32_vec3));
  x = ndf_f32_vec3.x;
  y = ndf_f32_vec3.y;
  z = ndf_f32_vec3.z;
}
void NDFPropertyF32_vec3::to_ndfbin(NDF *, std::ostream &stream) {
  NDF_F32_vec3 ndf_f32_vec3;
  ndf_f32_vec3.x = x;
  ndf_f32_vec3.y = y;
  ndf_f32_vec3.z = z;
  stream.write(reinterpret_cast<char *>(&ndf_f32_vec3), sizeof(NDF_F32_vec3));
}

#pragma pack(push, 1)
struct NDF_F32_vec4 {
  float x;
  float y;
  float z;
  float w;
};
#pragma pack(pop)

void NDFPropertyF32_vec4::from_ndfbin(NDF *, std::istream &stream) {
  NDF_F32_vec4 ndf_f32_vec4;
  stream.read(reinterpret_cast<char *>(&ndf_f32_vec4), sizeof(NDF_F32_vec4));
  x = ndf_f32_vec4.x;
  y = ndf_f32_vec4.y;
  z = ndf_f32_vec4.z;
  w = ndf_f32_vec4.w;
}
void NDFPropertyF32_vec4::to_ndfbin(NDF *, std::ostream &stream) {
  NDF_F32_vec4 ndf_f32_vec4;
  ndf_f32_vec4.x = x;
  ndf_f32_vec4.y = y;
  ndf_f32_vec4.z = z;
  ndf_f32_vec4.w = w;
  stream.write(reinterpret_cast<char *>(&ndf_f32_vec4), sizeof(NDF_F32_vec4));
}

#pragma pack(push, 1)
struct NDF_Color {
  uint8_t b;
  uint8_t g;
  uint8_t r;
  uint8_t a;
};
#pragma pack(pop)

void NDFPropertyColor::from_ndfbin(NDF *, std::istream &stream) {
  NDF_Color ndf_color;
  stream.read(reinterpret_cast<char *>(&ndf_color), sizeof(NDF_Color));
  r = ndf_color.r;
  g = ndf_color.g;
  b = ndf_color.b;
  a = ndf_color.a;
}
void NDFPropertyColor::to_ndfbin(NDF *, std::ostream &stream) {
  NDF_Color ndf_color;
  ndf_color.r = r;
  ndf_color.g = g;
  ndf_color.b = b;
  ndf_color.a = a;
  stream.write(reinterpret_cast<char *>(&ndf_color), sizeof(NDF_Color));
}

#pragma pack(push, 1)
struct NDF_S32_vec3 {
  int32_t x;
  int32_t y;
  int32_t z;
};
#pragma pack(pop)

void NDFPropertyS32_vec3::from_ndfbin(NDF *, std::istream &stream) {
  NDF_S32_vec3 ndf_s32_vec3;
  stream.read(reinterpret_cast<char *>(&ndf_s32_vec3), sizeof(NDF_S32_vec3));
  x = ndf_s32_vec3.x;
  y = ndf_s32_vec3.y;
  z = ndf_s32_vec3.z;
}
void NDFPropertyS32_vec3::to_ndfbin(NDF *, std::ostream &stream) {
  NDF_S32_vec3 ndf_s32_vec3;
  ndf_s32_vec3.x = x;
  ndf_s32_vec3.y = y;
  ndf_s32_vec3.z = z;
  stream.write(reinterpret_cast<char *>(&ndf_s32_vec3), sizeof(NDF_S32_vec3));
}

#pragma pack(push, 1)
struct NDF_ObjectReference {
  uint32_t object_index;
  uint32_t class_index;
};
#pragma pack(pop)

void NDFPropertyObjectReference::from_ndfbin(NDF *, std::istream &stream) {
  NDF_ObjectReference ndf_object_reference;
  stream.read(reinterpret_cast<char *>(&ndf_object_reference),
              sizeof(NDF_ObjectReference));
  object_name = "Object_" + std::to_string(ndf_object_reference.object_index);
}

void NDFPropertyObjectReference::to_ndfbin(NDF *root, std::ostream &stream) {
  uint32_t reference_type = ReferenceType::Object;
  stream.write(reinterpret_cast<char *>(&reference_type),
               sizeof(reference_type));
  NDF_ObjectReference ndf_object_reference;
  ndf_object_reference.object_index = root->get_object_index(object_name);
  ndf_object_reference.class_index = root->get_class_of_object(object_name);
  stream.write(reinterpret_cast<char *>(&ndf_object_reference),
               sizeof(NDF_ObjectReference));
}

#pragma pack(push, 1)
struct NDF_ImportReference {
  uint32_t import_index;
};
#pragma pack(pop)

void NDFPropertyImportReference::from_ndfbin(NDF *root, std::istream &stream) {
  NDF_ImportReference ndf_import_reference;
  stream.read(reinterpret_cast<char *>(&ndf_import_reference),
              sizeof(NDF_ImportReference));
  import_name = root->import_name_table[ndf_import_reference.import_index];
}

void NDFPropertyImportReference::to_ndfbin(NDF *root, std::ostream &stream) {
  uint32_t reference_type = ReferenceType::Import;
  stream.write(reinterpret_cast<char *>(&reference_type),
               sizeof(reference_type));
  NDF_ImportReference ndf_import_reference;
  ndf_import_reference.import_index = root->get_or_add_impr(import_name);
  stream.write(reinterpret_cast<char *>(&ndf_import_reference),
               sizeof(NDF_ImportReference));
}

#pragma pack(push, 1)
struct NDF_List {
  uint32_t count;
};
#pragma pack(pop)

void NDFPropertyList::from_ndfbin(NDF *root, std::istream &stream) {
  NDF_List ndf_list;
  stream.read(reinterpret_cast<char *>(&ndf_list), sizeof(NDF_List));
  for (uint32_t i = 0; i < ndf_list.count; i++) {
    uint32_t ndf_type = 0xFFFFFFFF;
    stream.read(reinterpret_cast<char *>(&ndf_type), sizeof(ndf_type));
    auto property = NDFProperty::get_property_from_ndfbin(ndf_type, stream);
    property->from_ndfbin(root, stream);
    property->property_name = "ListItem";
    values.push_back(std::move(property));
  }
}

void NDFPropertyList::to_ndfbin(NDF *root, std::ostream &stream) {
  NDF_List ndf_list;
  ndf_list.count = values.size();
  stream.write(reinterpret_cast<char *>(&ndf_list), sizeof(NDF_List));
  for (auto &property : values) {
    uint32_t ndf_type = property->property_type;
    stream.write(reinterpret_cast<char *>(&ndf_type), sizeof(ndf_type));
    property->to_ndfbin(root, stream);
  }
}

#pragma pack(push, 1)
struct NDF_Map {
  uint32_t count;
};
#pragma pack(pop)

void NDFPropertyMap::from_ndfbin(NDF *root, std::istream &stream) {
  NDF_Map ndf_map;
  stream.read(reinterpret_cast<char *>(&ndf_map), sizeof(NDF_Map));
  for (uint32_t i = 0; i < ndf_map.count; i++) {
    uint32_t ndf_type = 0xFFFFFFFF;
    stream.read(reinterpret_cast<char *>(&ndf_type), sizeof(ndf_type));
    auto key = NDFProperty::get_property_from_ndfbin(ndf_type, stream);
    key->from_ndfbin(root, stream);
    key->property_name = "Key";
    stream.read(reinterpret_cast<char *>(&ndf_type), sizeof(ndf_type));
    auto value = NDFProperty::get_property_from_ndfbin(ndf_type, stream);
    value->from_ndfbin(root, stream);
    value->property_name = "Value";
    values.push_back(std::make_pair(std::move(key), std::move(value)));
  }
}

void NDFPropertyMap::to_ndfbin(NDF *root, std::ostream &stream) {
  NDF_Map ndf_map;
  ndf_map.count = values.size();
  stream.write(reinterpret_cast<char *>(&ndf_map), sizeof(NDF_Map));
  for (auto &[key, value] : values) {
    uint32_t ndf_type = key->property_type;
    stream.write(reinterpret_cast<char *>(&ndf_type), sizeof(ndf_type));
    key->to_ndfbin(root, stream);
    ndf_type = value->property_type;
    stream.write(reinterpret_cast<char *>(&ndf_type), sizeof(ndf_type));
    value->to_ndfbin(root, stream);
  }
}

#pragma pack(push, 1)
struct NDF_Int16 {
  int16_t value;
};
#pragma pack(pop)

void NDFPropertyInt16::from_ndfbin(NDF *, std::istream &stream) {
  NDF_Int16 ndf_s16;
  stream.read(reinterpret_cast<char *>(&ndf_s16), sizeof(NDF_Int16));
  value = ndf_s16.value;
}
void NDFPropertyInt16::to_ndfbin(NDF *, std::ostream &stream) {
  NDF_Int16 ndf_s16;
  ndf_s16.value = value;
  stream.write(reinterpret_cast<char *>(&ndf_s16), sizeof(NDF_Int16));
}

#pragma pack(push, 1)
struct NDF_UInt16 {
  uint16_t value;
};
#pragma pack(pop)

void NDFPropertyUInt16::from_ndfbin(NDF *, std::istream &stream) {
  NDF_UInt16 ndf_u16;
  stream.read(reinterpret_cast<char *>(&ndf_u16), sizeof(NDF_UInt16));
  value = ndf_u16.value;
}
void NDFPropertyUInt16::to_ndfbin(NDF *, std::ostream &stream) {
  NDF_UInt16 ndf_u16;
  ndf_u16.value = value;
  stream.write(reinterpret_cast<char *>(&ndf_u16), sizeof(NDF_UInt16));
}

#pragma pack(push, 1)
struct NDF_GUID {
  uint8_t guid[16];
};
#pragma pack(pop)

void NDFPropertyGUID::from_ndfbin(NDF *, std::istream &stream) {
  NDF_GUID ndf_guid;
  stream.read(reinterpret_cast<char *>(&ndf_guid), sizeof(NDF_GUID));
  guid = "";
  for (auto const &byte : ndf_guid.guid) {
    guid += std::format("{:02X}", byte);
  }
}
void NDFPropertyGUID::to_ndfbin(NDF *, std::ostream &stream) {
  NDF_GUID ndf_guid;
  for (uint32_t i = 0; i < 16; i++) {
    std::from_chars(guid.c_str() + i * 2, guid.c_str() + i * 2 + 2,
                    ndf_guid.guid[i], 16);
  }
  stream.write(reinterpret_cast<char *>(&ndf_guid), sizeof(NDF_GUID));
}

#pragma pack(push, 1)
struct NDF_PathReference {
  uint32_t path_index;
};
#pragma pack(pop)

void NDFPropertyPathReference::from_ndfbin(NDF *root, std::istream &stream) {
  NDF_PathReference ndf_path_reference;
  stream.read(reinterpret_cast<char *>(&ndf_path_reference),
              sizeof(NDF_PathReference));
  path = root->string_table[ndf_path_reference.path_index];
}

void NDFPropertyPathReference::to_ndfbin(NDF *root, std::ostream &stream) {
  NDF_PathReference ndf_path_reference;
  ndf_path_reference.path_index = root->get_or_add_string(path);
  stream.write(reinterpret_cast<char *>(&ndf_path_reference),
               sizeof(NDF_PathReference));
}

#pragma pack(push, 1)
struct NDF_LocalisationHash {
  uint8_t hash[8];
};
#pragma pack(pop)

void NDFPropertyLocalisationHash::from_ndfbin(NDF *, std::istream &stream) {
  NDF_LocalisationHash ndf_hash;
  stream.read(reinterpret_cast<char *>(&ndf_hash),
              sizeof(NDF_LocalisationHash));
  hash = "";
  for (auto const &byte : ndf_hash.hash) {
    hash += std::format("{:02X}", byte);
  }
}
void NDFPropertyLocalisationHash::to_ndfbin(NDF *, std::ostream &stream) {
  NDF_LocalisationHash ndf_hash;
  for (uint32_t i = 0; i < 8; i++) {
    std::from_chars(hash.c_str() + i * 2, hash.c_str() + i * 2 + 2,
                    ndf_hash.hash[i], 16);
  }
  stream.write(reinterpret_cast<char *>(&ndf_hash),
               sizeof(NDF_LocalisationHash));
}

#pragma pack(push, 1)
struct NDF_S32_vec2 {
  int32_t x;
  int32_t y;
};
#pragma pack(pop)

void NDFPropertyS32_vec2::from_ndfbin(NDF *, std::istream &stream) {
  NDF_S32_vec2 ndf_s32_vec2;
  stream.read(reinterpret_cast<char *>(&ndf_s32_vec2), sizeof(NDF_S32_vec2));
  x = ndf_s32_vec2.x;
  y = ndf_s32_vec2.y;
}
void NDFPropertyS32_vec2::to_ndfbin(NDF *, std::ostream &stream) {
  NDF_S32_vec2 ndf_s32_vec2;
  ndf_s32_vec2.x = x;
  ndf_s32_vec2.y = y;
  stream.write(reinterpret_cast<char *>(&ndf_s32_vec2), sizeof(NDF_S32_vec2));
}

#pragma pack(push, 1)
struct NDF_F32_vec2 {
  float x;
  float y;
};
#pragma pack(pop)

void NDFPropertyF32_vec2::from_ndfbin(NDF *, std::istream &stream) {
  NDF_F32_vec2 ndf_f32_vec2;
  stream.read(reinterpret_cast<char *>(&ndf_f32_vec2), sizeof(NDF_F32_vec2));
  x = ndf_f32_vec2.x;
  y = ndf_f32_vec2.y;
}
void NDFPropertyF32_vec2::to_ndfbin(NDF *, std::ostream &stream) {
  NDF_F32_vec2 ndf_f32_vec2;
  ndf_f32_vec2.x = x;
  ndf_f32_vec2.y = y;
  stream.write(reinterpret_cast<char *>(&ndf_f32_vec2), sizeof(NDF_F32_vec2));
}

void NDFPropertyPair::from_ndfbin(NDF *root, std::istream &stream) {
  uint32_t ndf_type = 0xFFFFFFFF;
  stream.read(reinterpret_cast<char *>(&ndf_type), sizeof(ndf_type));
  first = NDFProperty::get_property_from_ndfbin(ndf_type, stream);
  first->from_ndfbin(root, stream);
  first->property_name = "First";
  stream.read(reinterpret_cast<char *>(&ndf_type), sizeof(ndf_type));
  second = NDFProperty::get_property_from_ndfbin(ndf_type, stream);
  second->from_ndfbin(root, stream);
  second->property_name = "Second";
}

void NDFPropertyPair::to_ndfbin(NDF *root, std::ostream &stream) {
  uint32_t ndf_type = first->property_type;
  stream.write(reinterpret_cast<char *>(&ndf_type), sizeof(ndf_type));
  first->to_ndfbin(root, stream);
  ndf_type = second->property_type;
  stream.write(reinterpret_cast<char *>(&ndf_type), sizeof(ndf_type));
  second->to_ndfbin(root, stream);
}

#pragma pack(push, 1)
struct NDF_Hash {
  uint8_t hash[16];
};
#pragma pack(pop)

void NDFPropertyHash::from_ndfbin(NDF *, std::istream &stream) {
  NDF_Hash ndf_hash;
  stream.read(reinterpret_cast<char *>(&ndf_hash), sizeof(NDF_Hash));
  hash = "";
  for (auto const &byte : ndf_hash.hash) {
    hash += std::format("{:02X}", byte);
  }
}
void NDFPropertyHash::to_ndfbin(NDF *, std::ostream &stream) {
  NDF_Hash ndf_hash;
  for (uint32_t i = 0; i < 8; i++) {
    std::from_chars(hash.c_str() + i * 2, hash.c_str() + i * 2 + 2,
                    ndf_hash.hash[i], 16);
  }
  stream.write(reinterpret_cast<char *>(&ndf_hash), sizeof(NDF_Hash));
}
