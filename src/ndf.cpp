#include "ndf.hpp"

std::unique_ptr<NDFProperty> NDFProperty::get_property_from_ndftype(uint32_t ndf_type) {
  switch(ndf_type) {
    case 0x0: {
      return std::make_unique<NDFPropertyBool>();
    }
    case 0x1: {
      return std::make_unique<NDFPropertyInt8>();
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
      return std::make_unique<NDFPropertyS16>();
    }
    case 0x19: {
      return std::make_unique<NDFPropertyU16>();
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

std::unique_ptr<NDFProperty> NDFProperty::get_property_from_ndf_xml(uint32_t ndf_type, const pugi::xml_node& ndf_node) {
  if(ndf_type == 0x9) {
    std::string reference_type = ndf_node.attribute("referenceType").as_string();
    if(reference_type == "object") {
      return std::make_unique<NDFPropertyObjectReference>();
    } else if(reference_type == "import") {
      return std::make_unique<NDFPropertyImportReference>();
    } else {
      throw std::runtime_error(std::format("Unknown ReferenceType: {}", reference_type));
    }
  }
  return get_property_from_ndftype(ndf_type);
}

std::unique_ptr<NDFProperty> NDFProperty::get_property_from_ndfbin(uint32_t ndf_type, std::istream& stream) {
  spdlog::debug("NDFType: {} @0x{:02X}", ndf_type, (uint32_t)stream.tellg());
  if(ndf_type == 0x9) {
    uint32_t reference_type;
    stream.read(reinterpret_cast<char*>(&reference_type), sizeof(uint32_t));
    if(reference_type == ReferenceType::Object) {
      return std::make_unique<NDFPropertyObjectReference>();
    } else if(reference_type == ReferenceType::Import) {
      return std::make_unique<NDFPropertyImportReference>();
    } else {
      throw std::runtime_error(std::format("Unknown ReferenceType: {}", reference_type));
    }
  }
  return get_property_from_ndftype(ndf_type);
}

void NDF::save_as_ndf_xml(fs::path path) {
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

void NDF::load_from_ndf_xml(fs::path path) {
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
    object_map.insert({object.name, objects.size() - 1});
  }
}

void NDF::load_imprs(std::istream &stream, std::vector<std::string> current_import_path) {
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

  auto foo = current_import_path | std::views::join_with('/');
  std::string tmp(foo.begin(), foo.end());
  import_name_table[index] = tmp + std::string("/") + tran_table[tran_index];
  spdlog::debug("Import: {}", import_name_table[index]);
}

void NDF::load_exprs(std::istream &stream, std::vector<std::string> current_export_path) {
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

  auto test = current_export_path | std::views::join_with('/');
  std::string tmp(test.begin(), test.end());
  objects[index].export_path = tmp + std::string("/") + tran_table[tran_index];
  spdlog::debug("Export: {}", objects[index].export_path);
}
