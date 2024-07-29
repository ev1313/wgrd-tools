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

