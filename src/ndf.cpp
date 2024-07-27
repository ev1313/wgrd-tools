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
    default: {
      throw std::runtime_error(std::format("Unknown NDFType: {}", ndf_type));
    }
  }
}

std::unique_ptr<NDFProperty> NDFProperty::get_property_from_ndfbin_xml(uint32_t ndf_type, const pugi::xml_node& ndf_node) {
  if(ndf_type == 0x9) {
      uint32_t reference_type = ndf_node.child("Reference").child("Reference").attribute("typeId").as_uint();
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

void NDFPropertyBool::from_ndfbin_xml(const NDF*, const pugi::xml_node& node) {
  value = node.child("Boolean").child("Boolean").attribute("value").as_uint() == 1;
  spdlog::debug("Boolean: {}", value);
}

void NDFPropertyBool::to_ndfbin_xml(NDF*, pugi::xml_node& node) {
  node.append_child("Boolean").append_child("Boolean").append_attribute("value").set_value(value);
}

void NDFPropertyInt8::from_ndfbin_xml(const NDF*, const pugi::xml_node& node) {
  value = node.child("Int8").child("Int8").attribute("value").as_int();
  spdlog::debug("Int8: {}", value);
}

void NDFPropertyInt8::to_ndfbin_xml(NDF*, pugi::xml_node& node) {
  node.append_child("Int8").append_child("Int8").append_attribute("value").set_value(value);
}

void NDFPropertyInt32::from_ndfbin_xml(const NDF*, const pugi::xml_node& node) {
  value = node.child("Int32").child("Int32").attribute("value").as_int();
  spdlog::debug("Int32: {}", value);
}

void NDFPropertyInt32::to_ndfbin_xml(NDF*, pugi::xml_node& node) {
  node.append_child("Int32").append_child("Int32").append_attribute("value").set_value(value);
}

void NDFPropertyUInt32::from_ndfbin_xml(const NDF*, const pugi::xml_node& node) {
  value = node.child("UInt32").child("UInt32").attribute("value").as_uint();
  spdlog::debug("UInt32: {}", value);
}

void NDFPropertyUInt32::to_ndfbin_xml(NDF*, pugi::xml_node& node) {
  node.append_child("UInt32").append_child("UInt32").append_attribute("value").set_value(value);
}

void NDFPropertyFloat32::from_ndfbin_xml(const NDF*, const pugi::xml_node& node) {
  value = node.child("Float32").child("Float32").attribute("value").as_float();
  spdlog::debug("Float: {}", value);
}

void NDFPropertyFloat32::to_ndfbin_xml(NDF*, pugi::xml_node& node) {
  node.append_child("Float32").append_child("Float32").append_attribute("value").set_value(value);
}

void NDFPropertyFloat64::from_ndfbin_xml(const NDF*, const pugi::xml_node& node) {
  value = node.child("Float64").child("Float64").attribute("value").as_double();
  spdlog::debug("Double: {}", value);
}

void NDFPropertyFloat64::to_ndfbin_xml(NDF*, pugi::xml_node& node) {
  node.append_child("Float64").append_child("Float64").append_attribute("value").set_value(value);
}

void NDFPropertyString::from_ndfbin_xml(const NDF* root, const pugi::xml_node& node) {
  uint32_t string_idx = node.child("StringReference").child("StringReference").attribute("stringIndex").as_uint();
  value = root->string_table[string_idx];
  spdlog::debug("String: {}", value);
}

void NDFPropertyString::to_ndfbin_xml(NDF* root, pugi::xml_node& node) {
  uint32_t string_idx = root->get_or_add_string(value);
  node.append_child("StringReference").append_child("StringReference").append_attribute("stringIndex").set_value(string_idx);
}

void NDFPropertyWideString::from_ndfbin_xml(const NDF*, const pugi::xml_node& node) {
  value = node.child("WideString").child("WideString").attribute("str").as_string();
  spdlog::debug("Wide String: {}", value);
}

void NDFPropertyWideString::to_ndfbin_xml(NDF*, pugi::xml_node& node) {
  node.append_child("WideString").append_child("WideString").append_attribute("str").set_value(value.c_str());
}

void NDFPropertyF32_vec3::from_ndfbin_xml(const NDF*, const pugi::xml_node& node) {
  x = node.child("F32_vec3").child("F32_vec3").attribute("x").as_float();
  y = node.child("F32_vec3").child("F32_vec3").attribute("y").as_float();
  z = node.child("F32_vec3").child("F32_vec3").attribute("z").as_float();
  spdlog::debug("Vector3: ({}, {}, {})", x, y, z);
}

void NDFPropertyF32_vec3::to_ndfbin_xml(NDF*, pugi::xml_node& node) {
  auto vec3_node = node.append_child("F32_vec3").append_child("F32_vec3");
  vec3_node.append_attribute("x").set_value(x);
  vec3_node.append_attribute("y").set_value(y);
  vec3_node.append_attribute("z").set_value(z);
}

void NDFPropertyF32_vec4::from_ndfbin_xml(const NDF*, const pugi::xml_node& node) {
  x = node.child("F32_vec4").child("F32_vec4").attribute("x").as_float();
  y = node.child("F32_vec4").child("F32_vec4").attribute("y").as_float();
  z = node.child("F32_vec4").child("F32_vec4").attribute("z").as_float();
  w = node.child("F32_vec4").child("F32_vec4").attribute("w").as_float();
  spdlog::debug("Vector4: ({}, {}, {}, {})", x, y, z, w);
}

void NDFPropertyF32_vec4::to_ndfbin_xml(NDF*, pugi::xml_node& node) {
  auto vec4_node = node.append_child("F32_vec4").append_child("F32_vec4");
  vec4_node.append_attribute("x").set_value(x);
  vec4_node.append_attribute("y").set_value(y);
  vec4_node.append_attribute("z").set_value(z);
  vec4_node.append_attribute("w").set_value(w);
}

void NDFPropertyColor::from_ndfbin_xml(const NDF*, const pugi::xml_node& node) {
  r = node.child("Color").child("Color").attribute("r").as_uint();
  g = node.child("Color").child("Color").attribute("g").as_uint();
  b = node.child("Color").child("Color").attribute("b").as_uint();
  a = node.child("Color").child("Color").attribute("a").as_uint();
  spdlog::debug("Color: ({}, {}, {}, {})", r, g, b, a);
}

void NDFPropertyColor::to_ndfbin_xml(NDF*, pugi::xml_node& node) {
  auto color_node = node.append_child("Color").append_child("Color");
  color_node.append_attribute("r").set_value(r);
  color_node.append_attribute("g").set_value(g);
  color_node.append_attribute("b").set_value(b);
  color_node.append_attribute("a").set_value(a);
}

void NDFPropertyS32_vec3::from_ndfbin_xml(const NDF*, const pugi::xml_node& node) {
  x = node.child("S32_vec3").child("S32_vec3").attribute("x").as_int();
  y = node.child("S32_vec3").child("S32_vec3").attribute("y").as_int();
  z = node.child("S32_vec3").child("S32_vec3").attribute("z").as_int();
  spdlog::debug("Vector3: ({}, {}, {})", x, y, z);
}

void NDFPropertyS32_vec3::to_ndfbin_xml(NDF*, pugi::xml_node& node) {
  auto vec3_node = node.append_child("S32_vec3").append_child("S32_vec3");
  vec3_node.append_attribute("x").set_value(x);
  vec3_node.append_attribute("y").set_value(y);
  vec3_node.append_attribute("z").set_value(z);
}

void NDFPropertyObjectReference::from_ndfbin_xml(const NDF*, const pugi::xml_node& node) {
  uint32_t object_idx = node.child("Reference").child("Reference").child("ObjectReference").child("ObjectReference").attribute("objectIndex").as_uint();
  object_name = std::format("Object_{}", object_idx);
  spdlog::debug("Object Reference: {}", object_name);
}

void NDFPropertyObjectReference::to_ndfbin_xml(NDF* root, pugi::xml_node& node) {
  auto object_node = node.append_child("Reference").append_child("Reference").append_child("ObjectReference").append_child("ObjectReference");
  uint32_t obj_idx = root->get_object(object_name);
  object_node.append_attribute("objectIndex").set_value(obj_idx);
}

void NDFPropertyImportReference::from_ndfbin_xml(const NDF* root, const pugi::xml_node& node) {
  uint32_t import_idx = node.child("Reference").child("Reference").child("ImprReference").child("ImprReference").attribute("imprIndex").as_uint();
  spdlog::debug("Import Index: {}", import_idx);
  import_name = root->import_name_table.at(import_idx);
  spdlog::debug("Import Reference: {}", import_name);
}

void NDFPropertyImportReference::to_ndfbin_xml(NDF* root, pugi::xml_node& node) {
  uint32_t import_idx = root->get_or_add_impr(import_name);
  auto import_node = node.append_child("Reference").append_child("Reference").append_child("ImprReference").append_child("ImprReference");
  import_node.append_attribute("imprIndex").set_value(import_idx);
}

void NDFPropertyList::from_ndfbin_xml(const NDF* root, const pugi::xml_node& node) {
  for(auto const& value_node : node.child("List").child("List").children()) {
    uint32_t sub_type = value_node.attribute("typeId").as_uint();
    auto property = get_property_from_ndfbin_xml(sub_type, value_node);
    property->from_ndfbin_xml(root, value_node);
    property->property_name = "ListItem";
    values.push_back(std::move(property));
  }
}

void NDFPropertyList::to_ndfbin_xml(NDF* root, pugi::xml_node& node) {
  auto list_node = node.append_child("List").append_child("List");
  for(auto const& value : values) {
    auto value_node = list_node.append_child("items");
    value_node.append_attribute("typeId").set_value(value->property_type);
    value->to_ndfbin_xml(root, value_node);
  }
}

void NDFPropertyMap::from_ndfbin_xml(const NDF* root, const pugi::xml_node& node) {
  for(auto const& map_node : node.child("Map").child("Map").child("mapitems").children()) {
    auto const &key_node = map_node.child("key");
    uint32_t key_type = key_node.attribute("typeId").as_uint();
    auto key_property = get_property_from_ndfbin_xml(key_type, key_node);
    key_property->property_name = "Key";
    key_property->from_ndfbin_xml(root, key_node);

    auto const &value_node = map_node.child("value");
    uint32_t value_type = value_node.attribute("typeId").as_uint();
    auto value_property = get_property_from_ndfbin_xml(value_type, value_node);
    value_property->property_name = "Value";
    value_property->from_ndfbin_xml(root, value_node);

    values.push_back(std::make_pair(std::move(key_property), std::move(value_property)));
  }
}

void NDFPropertyMap::to_ndfbin_xml(NDF* root, pugi::xml_node& node) {
  auto map_node = node.append_child("Map").append_child("Map").append_child("mapitems");
  for(auto const& [key, value] : values) {
    auto key_node = map_node.append_child("key");
    key_node.append_attribute("typeId").set_value(key->property_type);
    key->to_ndfbin_xml(root, key_node);
    auto value_node = map_node.append_child("value");
    value_node.append_attribute("typeId").set_value(value->property_type);
    value->to_ndfbin_xml(root, value_node);
  }
}

void NDFPropertyS16::from_ndfbin_xml(const NDF*, const pugi::xml_node& node) {
  value = node.child("S16").child("S16").attribute("value").as_int();
  spdlog::debug("S16: {}", value);
}

void NDFPropertyS16::to_ndfbin_xml(NDF*, pugi::xml_node& node) {
  node.append_child("S16").append_child("S16").append_attribute("value").set_value(value);
}

void NDFPropertyU16::from_ndfbin_xml(const NDF*, const pugi::xml_node& node) {
  value = node.child("U16").child("U16").attribute("value").as_uint();
  spdlog::debug("U16: {}", value);
}

void NDFPropertyU16::to_ndfbin_xml(NDF*, pugi::xml_node& node) {
  node.append_child("U16").append_child("U16").append_attribute("value").set_value(value);
}

void NDFPropertyGUID::from_ndfbin_xml(const NDF*, const pugi::xml_node& node) {
  guid = node.child("GUID").child("GUID").attribute("data").as_string();
  spdlog::debug("GUID: {}", guid);
}

void NDFPropertyGUID::to_ndfbin_xml(NDF*, pugi::xml_node& node) {
  node.append_child("GUID").append_child("GUID").append_attribute("data").set_value(guid.c_str());
}

void NDFPropertyPathReference::from_ndfbin_xml(const NDF* root, const pugi::xml_node& node) {
  uint32_t string_idx = node.child("PathReference").child("PathReference").attribute("stringIndex").as_uint();
  path = root->string_table[string_idx];
  spdlog::debug("Path Reference: {}", path);
}

void NDFPropertyPathReference::to_ndfbin_xml(NDF*, pugi::xml_node& node) {
  node.append_child("PathReference").append_child("PathReference").append_attribute("stringIndex").set_value(path.c_str());
}

void NDFPropertyLocalisationHash::from_ndfbin_xml(const NDF*, const pugi::xml_node& node) {
  hash = node.child("LocalisationHash").child("LocalisationHash").attribute("data").as_string();
  spdlog::debug("Localisation Hash: {}", hash);
}

void NDFPropertyLocalisationHash::to_ndfbin_xml(NDF*, pugi::xml_node& node) {
  node.append_child("LocalisationHash").append_child("LocalisationHash").append_attribute("data").set_value(hash.c_str());
}

void NDFPropertyS32_vec2::from_ndfbin_xml(const NDF*, const pugi::xml_node& node) {
  x = node.child("S32_vec2").child("S32_vec2").attribute("x").as_int();
  y = node.child("S32_vec2").child("S32_vec2").attribute("y").as_int();
  spdlog::debug("Vector2: ({}, {})", x, y);
}

void NDFPropertyS32_vec2::to_ndfbin_xml(NDF*, pugi::xml_node& node) {
  auto vec2_node = node.append_child("S32_vec2").append_child("S32_vec2");
  vec2_node.append_attribute("x").set_value(x);
  vec2_node.append_attribute("y").set_value(y);
}

void NDFPropertyF32_vec2::from_ndfbin_xml(const NDF*, const pugi::xml_node& node) {
  x = node.child("F32_vec2").child("F32_vec2").attribute("x").as_float();
  y = node.child("F32_vec2").child("F32_vec2").attribute("y").as_float();
  spdlog::debug("Vector2: ({}, {})", x, y);
}

void NDFPropertyF32_vec2::to_ndfbin_xml(NDF*, pugi::xml_node& node) {
  auto vec2_node = node.append_child("F32_vec2").append_child("F32_vec2");
  vec2_node.append_attribute("x").set_value(x);
  vec2_node.append_attribute("y").set_value(y);
}

void NDFPropertyPair::from_ndfbin_xml(const NDF* root, const pugi::xml_node& node) {
  auto const &first_node = node.child("Pair").child("Pair").child("first");
  uint32_t first_type = first_node.attribute("typeId").as_uint();
  first = get_property_from_ndfbin_xml(first_type, first_node);
  first->from_ndfbin_xml(root, first_node);
  first->property_name = "First";

  auto const &second_node = node.child("Pair").child("Pair").child("second");
  uint32_t second_type = second_node.attribute("typeId").as_uint();
  second = get_property_from_ndfbin_xml(second_type, second_node);
  second->from_ndfbin_xml(root, second_node);
  second->property_name = "Second";
}

void NDFPropertyPair::to_ndfbin_xml(NDF* root, pugi::xml_node& node) {
  auto pair_node = node.append_child("Pair").append_child("Pair");
  auto first_node = pair_node.append_child("first");
  first_node.append_attribute("typeId").set_value(first->property_type);
  first->to_ndfbin_xml(root, first_node);
  auto second_node = pair_node.append_child("second");
  second_node.append_attribute("typeId").set_value(second->property_type);
  second->to_ndfbin_xml(root, second_node);
}

void NDF::load_from_ndfbin_xml(fs::path path) {
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

    uint32_t class_idx = object_node.attribute("classIndex").as_uint();
    auto class_iterator = root.child("CLAS").children().begin();
    std::advance(class_iterator, class_idx);
    object.class_name = class_iterator->attribute("str").as_string();

    for(auto const prop_node : object_node.children()) {
      uint32_t property_idx = prop_node.attribute("propertyIndex").as_uint();

      if(property_idx == 2880154539) {
        continue;
      }

      spdlog::debug("Property Index: {}", property_idx);
      auto property_iterator = root.child("PROP").children().begin();
      std::advance(property_iterator, property_idx);

      auto ndf_node = prop_node.child("NDFType");
      uint32_t ndf_type = ndf_node.attribute("typeId").as_uint();

      std::unique_ptr<NDFProperty> property = NDFProperty::get_property_from_ndfbin_xml(ndf_type, ndf_node);
      property->from_ndfbin_xml(this, ndf_node);

      property->property_name = property_iterator->attribute("str").as_string();

      object.properties.push_back(std::move(property));
    }

    objects.push_back(std::move(object));
  }

  for(auto const &topo_node : root.child("TOPO").children()) {
    uint32_t object_idx = topo_node.attribute("objectIndex").as_uint();
    auto object_iterator = objects.begin();
    std::advance(object_iterator, object_idx);
    object_iterator->is_top_object = true;
  }

  std::vector<std::string> current_export_path;
  iterate_exprs(root, root.child("EXPR").child("Export"), current_export_path);
}

void NDF::save_imprs(const std::map<std::vector<uint32_t>, uint32_t>& gen_table, pugi::xml_node& root_node, const std::string& first_name) {
  std::vector<uint32_t> previous_import_path = {};
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
    std::vector<uint32_t> current_path;
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

void NDF::save_as_ndfbin_xml(fs::path path) {
  gen_object_table.clear();
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
  gen_property_items.clear();
  gen_property_set.clear();

  pugi::xml_document doc;
  auto root = doc.append_child("root").append_child("NdfBin").append_child("toc0header");

  auto obje = root.append_child("OBJE");

  for(const auto &[obj_idx, obj] : objects | std::views::enumerate) {
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
  uint32_t property_idx = 0;

  auto strg = root.append_child("STRG");
  auto tran = root.append_child("TRAN");
  auto impr = root.append_child("IMPR");
  auto expr = root.append_child("EXPR");

  uint32_t object_idx = 0;
  for(const auto &obj : objects) {
    auto obje_node = obje.append_child("Object");

    uint32_t class_idx;
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
      uint32_t property_idx = gen_property_table[class_idx].properties[property->property_name];
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
      get_or_add_expr(obj.export_path, object_idx);
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
