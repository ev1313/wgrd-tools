#include "ndf.hpp"

#include <set>

#include "utf.hpp"

#include <iostream>
#include <fstream>
#include <memory>

void NDFPropertyString::from_ndfbin(NDF* root, std::istream& stream) {
  NDF_String ndf_string;
  stream.read(reinterpret_cast<char*>(&ndf_string), sizeof(NDF_String));
  value = root->string_table[ndf_string.string_index];
}

void NDFPropertyString::to_ndfbin(NDF* root, std::ostream& stream) {
  NDF_String ndf_string;
  ndf_string.string_index = root->get_or_add_string(value);
  stream.write(reinterpret_cast<char*>(&ndf_string), sizeof(NDF_String));
}

void NDFPropertyWideString::from_ndfbin(NDF*, std::istream& stream) {
  NDF_WideString ndf_wide_string;
  stream.read(reinterpret_cast<char*>(&ndf_wide_string), sizeof(NDF_WideString));
  assert(ndf_wide_string.length % 2 == 0);
  std::u16string str;
  for(uint32_t i = 0; i < ndf_wide_string.length/2; i++) {
    char16_t c;
    stream.read(reinterpret_cast<char*>(&c), sizeof(char16_t));
    str.push_back(c);
  }
  // convert to UTF-8
  value = Utf32To8(Utf16To32(str));
  spdlog::debug("WideString: {}", value);
}

void NDFPropertyWideString::to_ndfbin(NDF*, std::ostream& stream) {
  NDF_WideString ndf_wide_string;
  ndf_wide_string.length = value.size() * 2;
  stream.write(reinterpret_cast<char*>(&ndf_wide_string), sizeof(NDF_WideString));
  // convert to UTF-16
  std::u16string buffer = Utf32To16(Utf8To32(value));
  stream.write((char*)buffer.data(), buffer.size() * sizeof(char16_t));
}

void NDFPropertyObjectReference::from_ndfbin(NDF*, std::istream& stream) {
  NDF_ObjectReference ndf_object_reference;
  stream.read(reinterpret_cast<char*>(&ndf_object_reference), sizeof(NDF_ObjectReference));
  object_name = "Object_" + std::to_string(ndf_object_reference.object_index);
}

void NDFPropertyObjectReference::to_ndfbin(NDF* root, std::ostream& stream) {
  uint32_t reference_type = ReferenceType::Object;
  stream.write(reinterpret_cast<char*>(&reference_type), sizeof(reference_type));
  NDF_ObjectReference ndf_object_reference;
  ndf_object_reference.object_index = root->get_object_index(object_name);
  ndf_object_reference.class_index = root->get_class_of_object(object_name);
  stream.write(reinterpret_cast<char*>(&ndf_object_reference), sizeof(NDF_ObjectReference));
}

void NDFPropertyImportReference::from_ndfbin(NDF* root, std::istream& stream) {
  NDF_ImportReference ndf_import_reference;
  stream.read(reinterpret_cast<char*>(&ndf_import_reference), sizeof(NDF_ImportReference));
  import_name = root->import_name_table[ndf_import_reference.import_index];
}

void NDFPropertyImportReference::to_ndfbin(NDF* root, std::ostream& stream) {
  uint32_t reference_type = ReferenceType::Import;
  stream.write(reinterpret_cast<char*>(&reference_type), sizeof(reference_type));
  NDF_ImportReference ndf_import_reference;
  ndf_import_reference.import_index = root->get_or_add_impr(import_name);
  stream.write(reinterpret_cast<char*>(&ndf_import_reference), sizeof(NDF_ImportReference));
}

void NDFPropertyList::from_ndfbin(NDF* root, std::istream& stream) {
  NDF_List ndf_list;
  stream.read(reinterpret_cast<char*>(&ndf_list), sizeof(NDF_List));
  for (uint32_t i = 0; i < ndf_list.count; i++) {
    uint32_t ndf_type = 0xFFFFFFFF;
    stream.read(reinterpret_cast<char*>(&ndf_type), sizeof(ndf_type));
    auto property = NDFProperty::get_property_from_ndfbin(ndf_type, stream);
    property->from_ndfbin(root, stream);
    property->property_name = "ListItem";
    values.push_back(std::move(property));
  }
}

void NDFPropertyList::to_ndfbin(NDF* root, std::ostream& stream) {
  NDF_List ndf_list;
  ndf_list.count = values.size();
  stream.write(reinterpret_cast<char*>(&ndf_list), sizeof(NDF_List));
  for (auto& property : values) {
    uint32_t ndf_type = property->property_type;
    stream.write(reinterpret_cast<char*>(&ndf_type), sizeof(ndf_type));
    property->to_ndfbin(root, stream);
  }
}

void NDFPropertyMap::from_ndfbin(NDF* root, std::istream& stream) {
  NDF_Map ndf_map;
  stream.read(reinterpret_cast<char*>(&ndf_map), sizeof(NDF_Map));
  for (uint32_t i = 0; i < ndf_map.count; i++) {
    uint32_t ndf_type = 0xFFFFFFFF;
    stream.read(reinterpret_cast<char*>(&ndf_type), sizeof(ndf_type));
    auto key = NDFProperty::get_property_from_ndfbin(ndf_type, stream);
    key->from_ndfbin(root, stream);
    key->property_name = "Key";
    stream.read(reinterpret_cast<char*>(&ndf_type), sizeof(ndf_type));
    auto value = NDFProperty::get_property_from_ndfbin(ndf_type, stream);
    value->from_ndfbin(root, stream);
    value->property_name = "Value";
    values.push_back(std::make_pair(std::move(key), std::move(value)));
  }
}

void NDFPropertyMap::to_ndfbin(NDF* root, std::ostream& stream) {
  NDF_Map ndf_map;
  ndf_map.count = values.size();
  stream.write(reinterpret_cast<char*>(&ndf_map), sizeof(NDF_Map));
  for (auto& [key, value] : values) {
    uint32_t ndf_type = key->property_type;
    stream.write(reinterpret_cast<char*>(&ndf_type), sizeof(ndf_type));
    key->to_ndfbin(root, stream);
    ndf_type = value->property_type;
    stream.write(reinterpret_cast<char*>(&ndf_type), sizeof(ndf_type));
    value->to_ndfbin(root, stream);
  }
}

void NDFPropertyPathReference::from_ndfbin(NDF* root, std::istream& stream) {
  NDF_PathReference ndf_path_reference;
  stream.read(reinterpret_cast<char*>(&ndf_path_reference), sizeof(NDF_PathReference));
  path = root->string_table[ndf_path_reference.path_index];
}

void NDFPropertyPathReference::to_ndfbin(NDF* root, std::ostream& stream) {
  NDF_PathReference ndf_path_reference;
  ndf_path_reference.path_index = root->get_or_add_string(path);
  stream.write(reinterpret_cast<char*>(&ndf_path_reference), sizeof(NDF_PathReference));
}

void NDFPropertyPair::from_ndfbin(NDF* root, std::istream& stream) {
  uint32_t ndf_type = 0xFFFFFFFF;
  stream.read(reinterpret_cast<char*>(&ndf_type), sizeof(ndf_type));
  first = NDFProperty::get_property_from_ndfbin(ndf_type, stream);
  first->from_ndfbin(root, stream);
  first->property_name = "First";
  stream.read(reinterpret_cast<char*>(&ndf_type), sizeof(ndf_type));
  second = NDFProperty::get_property_from_ndfbin(ndf_type, stream);
  second->from_ndfbin(root, stream);
  second->property_name = "Second";
}

void NDFPropertyPair::to_ndfbin(NDF* root, std::ostream& stream) {
  uint32_t ndf_type = first->property_type;
  stream.write(reinterpret_cast<char*>(&ndf_type), sizeof(ndf_type));
  first->to_ndfbin(root, stream);
  ndf_type = second->property_type;
  stream.write(reinterpret_cast<char*>(&ndf_type), sizeof(ndf_type));
  second->to_ndfbin(root, stream);
}

#pragma pack(push, 1)
struct NDFBinHeader {
  char magic[4] = {'E', 'U', 'G', '0'};
  char magic2[4] = {0,0,0,0};
  char magic3[4] = {'C', 'N', 'D', 'F'};
  uint32_t compressed = 0;
  uint32_t toc0offset;
  char unk0[4] = {0,0,0,0};
  uint32_t headerSize = 40;
  char unk2[4] = {0,0,0,0};
  uint32_t size = 0;
  char unk4[4] = {0,0,0,0};
};

struct TOCTableEntry {
  char magic[4];
  char pad0[4] = {0,0,0,0};
  uint32_t offset;
  char pad1[4] = {0,0,0,0};
  uint32_t size;
  char pad2[4] = {0,0,0,0};
};

struct TOCTable {
  char magic[4] = {'T', 'O', 'C', '0'};
  uint32_t count = 9;
  TOCTableEntry OBJE;
  TOCTableEntry TOPO;
  TOCTableEntry CHNK;
  TOCTableEntry CLAS;
  TOCTableEntry PROP;
  TOCTableEntry STRG;
  TOCTableEntry TRAN;
  TOCTableEntry IMPR;
  TOCTableEntry EXPR;
};

struct NDF_Object {
  uint32_t classIndex;
};

struct NDF_Property {
  uint32_t propertyIndex;
};

struct NDF_Type {
  uint32_t typeIndex;
};
#pragma pack(pop)

void NDF::load_from_ndfbin(fs::path path) {
  std::ifstream file(path, std::ios::binary);
  if (!file.is_open()) {
    throw std::runtime_error("Failed to open file");
  }

  NDFBinHeader header;
  file.read(reinterpret_cast<char*>(&header), sizeof(header));

  if (header.magic[0] != 'E' || header.magic[1] != 'U' || header.magic[2] != 'G' || header.magic[3] != '0') {
    throw std::runtime_error("Invalid magic number");
  }

  if (header.magic3[0] != 'C' || header.magic3[1] != 'N' || header.magic3[2] != 'D' || header.magic3[3] != 'F') {
    throw std::runtime_error("Invalid magic number");
  }

  if (header.headerSize != 40) {
    throw std::runtime_error("Invalid header size");
  }

  TOCTable toc;
  file.seekg(header.toc0offset);
  file.read(reinterpret_cast<char*>(&toc), sizeof(toc));

  if (toc.magic[0] != 'T' || toc.magic[1] != 'O' || toc.magic[2] != 'C' || toc.magic[3] != '0') {
    throw std::runtime_error("Invalid TOC magic number");
  }

  if (toc.count != 9) {
    throw std::runtime_error("Invalid TOC count");
  }

  // load class names
  file.seekg(toc.CLAS.offset);
  uint32_t clas_endoffset = toc.CLAS.offset + toc.CLAS.size;
  while ((uint32_t)file.tellg() < clas_endoffset) {
    uint32_t length;
    file.read(reinterpret_cast<char*>(&length), sizeof(length));
    std::string class_name;
    class_name.resize(length);
    file.read(class_name.data(), length * sizeof(char));
    class_table.push_back(class_name);
    spdlog::debug("Class: {}", class_name);
  }

  // load strings
  file.seekg(toc.STRG.offset);
  uint32_t strg_endoffset = toc.STRG.offset + toc.STRG.size;
  while ((uint32_t)file.tellg() < strg_endoffset) {
    uint32_t length;
    file.read(reinterpret_cast<char*>(&length), sizeof(length));
    std::string string;
    string.resize(length);
    file.read(string.data(), length * sizeof(char));
    string_table.push_back(string);
    spdlog::debug("String: {}", string);
  }

  // load tran table
  file.seekg(toc.TRAN.offset);
  uint32_t tran_endoffset = toc.TRAN.offset + toc.TRAN.size;
  while ((uint32_t)file.tellg() < tran_endoffset) {
    uint32_t length;
    file.read(reinterpret_cast<char*>(&length), sizeof(length));
    std::string string;
    string.resize(length);
    file.read(string.data(), length * sizeof(char));
    spdlog::debug("Tran: {}", string);
    tran_table.push_back(string);
  }

  // load properties
  file.seekg(toc.PROP.offset);
  uint32_t prop_endoffset = toc.PROP.offset + toc.PROP.size;
  while ((uint32_t)file.tellg() < prop_endoffset) {
    uint32_t str_len;
    file.read(reinterpret_cast<char*>(&str_len), sizeof(str_len));
    std::string prop_name;
    prop_name.resize(str_len);
    file.read(prop_name.data(), str_len * sizeof(char));
    uint32_t class_idx;
    file.read(reinterpret_cast<char*>(&class_idx), sizeof(class_idx));
    spdlog::debug("Property: {} {}", prop_name, class_idx);
    property_table.emplace_back(prop_name, class_idx);
  }

  // load imports
  file.seekg(toc.IMPR.offset);
  uint32_t impr_endoffset = toc.IMPR.offset + toc.IMPR.size;
  while ((uint32_t)file.tellg() < impr_endoffset) {
    load_imprs(file, {});
  }

  // load objects
  file.seekg(toc.OBJE.offset);
  spdlog::debug("0x{:02X} Object Table", (uint32_t)file.tellg());
  uint32_t obje_endoffset = toc.OBJE.offset + toc.OBJE.size;
  while ((uint32_t)file.tellg() < obje_endoffset) {
    NDF_Object obj;
    file.read(reinterpret_cast<char*>(&obj), sizeof(obj));

    NDFObject object;
    object.name = "Object_" + std::to_string(object_map.size());
    obj.classIndex = obj.classIndex;
    object.class_name = class_table[obj.classIndex];

    spdlog::debug("0x{:02X} Object: {} ({})", (uint32_t)file.tellg(), object.name, object.class_name);

    // load properties
    NDF_Property prop{0};
    while(prop.propertyIndex != 2880154539) {
      file.read(reinterpret_cast<char*>(&prop), sizeof(prop));
      if (prop.propertyIndex == 2880154539) {
        break;
      }
      uint32_t ndf_type = 0xFFFFFFFF;
      file.read(reinterpret_cast<char*>(&ndf_type), sizeof(ndf_type));
      
      auto property = NDFProperty::get_property_from_ndfbin(ndf_type, file);
      property->from_ndfbin(this, file);
      property->property_name = property_table[prop.propertyIndex].first;

      object.properties.push_back(std::move(property));
      object.property_map.insert({property_table[prop.propertyIndex].first, object.properties.size() - 1});
    }
    object_map.insert({object.name, std::move(object)});
  }

  fill_gen_object();

  // load exports
  file.seekg(toc.EXPR.offset);
  uint32_t expr_endoffset = toc.EXPR.offset + toc.EXPR.size;
  while ((uint32_t)file.tellg() < expr_endoffset) {
    load_exprs(file, {});
  }

  // load TOPO
  file.seekg(toc.TOPO.offset);
  uint32_t topo_endoffset = toc.TOPO.offset + toc.TOPO.size;
  while ((uint32_t)file.tellg() < topo_endoffset) {
    uint32_t object_index;
    file.read(reinterpret_cast<char*>(&object_index), sizeof(object_index));
    object_map[gen_object_items[object_index]].is_top_object = true;
  }
}

void NDF::save_ndfbin_imprs(const std::map<std::vector<uint32_t>, uint32_t>& gen_table, std::ostream& stream) {
  // we need for every node the count of the children of this node
  std::map<std::vector<uint32_t>, uint32_t> count_table;
  {
    std::set<std::vector<uint32_t>> visited_subpaths;
    for(const auto&[k, v] : gen_table) {
      {
        std::string key_str = "";
        for(auto const &foo : k) {
          key_str += gen_tran_table[foo] + "/";
        }
        spdlog::debug("count_table: {}", key_str);
      }
      std::vector<uint32_t> current_part = {k[0]};
      for(const auto& i : k | std::views::drop(1) | std::views::take(k.size() - 1)) {
        std::vector<uint32_t> sub_part = current_part;
        current_part.push_back(i);

        if(visited_subpaths.find(current_part) == visited_subpaths.end()) {
          if(count_table.find(sub_part) == count_table.end()) {
            count_table[sub_part] = 1;
          } else {
            count_table[sub_part] += 1;
          }
          visited_subpaths.insert(current_part);
        }
        {
          std::string key_str = "";
          for(auto const &foo : sub_part) {
            key_str += gen_tran_table[foo] + "/";
          }
          spdlog::debug("count_table import: {} -> {}", key_str, count_table[sub_part]);
        }
      }
    }
  }

  std::vector<uint32_t> previous_import_path = {};
  std::vector<std::pair<uint32_t, std::vector<uint32_t>>> previous_offsets = {};
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
        //spdlog::debug("diff_point size: {} {}", previous_import_path.size(), x);
        break;
      }
      if(previous_import_path[x] != k[x]) {
        diff_point = x;
        //spdlog::debug("diff_point diff {} {}", previous_import_path[x], k[x]);
        break;
      }
      // we do not add the item that differs
      current_path.push_back(k[x]);
    }
    // now shorten the previous import_path and import xml nodes to the diff point
    //spdlog::debug("diff_point: {}", diff_point);
    {
      std::string key_str = "";
      for(auto const &foo : previous_import_path) {
        key_str += gen_tran_table[foo] + "/";
      }
      spdlog::debug("before shorten prev Import path: {} -> {}", key_str, v);
    }
    int diff = previous_offsets.size() - diff_point;
    for(int x = 0; x < diff; x++) {
      // fix previously empty offsets
      auto& [offset, offsets] = previous_offsets.back();
      uint32_t prev_loc = stream.tellp();
      stream.seekp(offset);
      stream.write(reinterpret_cast<char*>(offsets.data()), offsets.size() * sizeof(uint32_t));
    
      {
        std::string key_str = "";
        for(auto const &foo : previous_import_path) {
          key_str += gen_tran_table[foo] + "/";
        }
        //spdlog::debug("IMPORT: {} -> {}", key_str, v);
      }
      
      size_t count = count_table[previous_import_path];
      spdlog::debug("COUNT {} {}", count, offsets.size());
      assert(count == offsets.size());

      spdlog::debug("WRITE offsets {} {} @0x{:02X}", previous_offsets.size(), previous_offsets.back().second.size(), (uint32_t)offset);

      stream.seekp(prev_loc);
      previous_import_path.pop_back();
      previous_offsets.pop_back();
    }
    {
      std::string key_str = "";
      for(auto const &foo : previous_import_path) {
        key_str += gen_tran_table[foo] + "/";
      }
      //spdlog::debug("after shorten prev Import path: {} -> {}", key_str, v);
    }
    assert(previous_offsets.size() == previous_import_path.size());

    for(unsigned int x = diff_point; x < k.size(); x++) {
      // if not first node, add offset to the offsets array
      if(previous_offsets.size() != 0) {
        auto& [offset, offsets] = previous_offsets.back();
        offsets.push_back((uint32_t)stream.tellp() - offset);
        spdlog::debug("ADD OFFSET prev_offsets @0x{:02X} {} offsets {} 0x{:02X}", offset, previous_offsets.size(), offsets.size(), offsets.back());
      }

      // we now update the current_path and search for a possible import
      current_path.push_back(k[x]);
      
      // same layer -> add new node
      // write tranIndex, index and count
      uint32_t tranIndex = k[x];
      uint32_t index = 4294967295;
      // search for import
      auto impr_index_it = gen_table.find(current_path); 
      if(impr_index_it != gen_table.end()) {
        index = impr_index_it->second;
      }
      uint32_t count = count_table[current_path];

      stream.write(reinterpret_cast<char*>(&tranIndex), sizeof(tranIndex));
      stream.write(reinterpret_cast<char*>(&index), sizeof(index));
      stream.write(reinterpret_cast<char*>(&count), sizeof(count));

      // create offsets table
      previous_offsets.push_back({stream.tellp(), {}});
      spdlog::debug("ARRAY @0x{:02X} count {}", (uint32_t)stream.tellp(), count);
      // write table with wrong info
      std::vector<uint32_t> offsets;
      offsets.resize(count);
      stream.write(reinterpret_cast<char*>(offsets.data()), offsets.size() * sizeof(uint32_t));

      {
        std::string key_str = "";
        for(auto const &foo : current_path) {
          key_str += gen_tran_table[foo] + "/";
        }
        spdlog::debug("ARRAY IMPORT: {} -> {}", key_str, v);
      }
    }

    previous_import_path = current_path;
  }
  int s = previous_offsets.size();
  for(int x = 0; x < s; x++) {
    spdlog::debug("SIZE {} {}" , x, previous_offsets.size());
    // fix previously empty offsets
    auto& [offset, offsets] = previous_offsets.back();
    uint32_t prev_loc = stream.tellp();
    stream.seekp(offset);
    stream.write(reinterpret_cast<char*>(offsets.data()), offsets.size() * sizeof(uint32_t));

    size_t count = count_table[previous_import_path];
    spdlog::debug("COUNT {} {}", count, offsets.size());
    assert(count == offsets.size());

    spdlog::debug("WRITE offsets {} {} @0x{:02X}", previous_offsets.size(), previous_offsets.back().second.size(), (uint32_t)offset);

    stream.seekp(prev_loc);
    previous_import_path.pop_back();
    previous_offsets.pop_back();
  }
}

void NDF::save_as_ndfbin(fs::path output) {
  gen_object_items.clear();
  gen_object_table.clear();
  gen_string_items.clear();
  gen_string_table.clear();
  gen_clas_items.clear();
  gen_clas_table.clear();
  gen_topo_table.clear();
  gen_tran_items.clear();
  gen_tran_table.clear();
  gen_import_table.clear();
  gen_import_items.clear();
  gen_export_table.clear();
  gen_export_items.clear();
  gen_property_table.clear();

  fill_gen_object();

  NDFBinHeader header;
  TOCTable toc_table;

  std::fstream ofs(output, std::fstream::in | std::fstream::out | std::fstream::binary | std::fstream::trunc);

  ofs.write(reinterpret_cast<char*>(&header), sizeof(header));

  toc_table.OBJE.magic[0] = 'O';
  toc_table.OBJE.magic[1] = 'B';
  toc_table.OBJE.magic[2] = 'J';
  toc_table.OBJE.magic[3] = 'E';

  toc_table.OBJE.offset = ofs.tellp();

  {
    // fill class and property tables
    // iterating object_map here works, because std::map is ordered by key
    for(const auto &[obj_idx, it] : object_map | std::views::enumerate) {
      const auto& obj = it.second;
      auto clas_it = gen_clas_items.find(obj.class_name);
      if(clas_it == gen_clas_items.end()) {
        gen_clas_table.push_back(obj.class_name);
        gen_clas_items.insert({obj.class_name, gen_clas_table.size() - 1});
        uint32_t class_idx = gen_clas_table.size() - 1;
        gen_property_table.emplace_back();
        for(auto& property : obj.properties) {
          gen_property_table[class_idx].properties.insert({property->property_name, 0});
          gen_property_set.insert({property->property_name, class_idx});
        }
      } else {
        for(auto& property : obj.properties) {
          if(!gen_property_set.contains({property->property_name, clas_it->second})) {
            gen_property_set.insert({property->property_name, clas_it->second});
            gen_property_table[clas_it->second].properties.insert({property->property_name, 0});
          }
        }
      }

      if(obj.is_top_object) {
        gen_topo_table.push_back(obj_idx);
      }

      if(obj.export_path.size()) {
        get_or_add_expr(obj.export_path, obj_idx);
      }
    }
    // now generate property indices
    for(auto&& [clas_idx, clas] : gen_property_table | std::views::enumerate) {
      for(auto& [prop_name, prop_idx] : clas.properties) {
        gen_property_items.push_back({prop_name, clas_idx});
        prop_idx = gen_property_items.size() - 1;
      }
    }
  }

  // write OBJE
  // writing the properties also fills the string table
  spdlog::debug("writing objects @0x{:02X}", (uint32_t)ofs.tellp());
  for(const auto &[obj_idx, it] : object_map | std::views::enumerate) {
    const auto& obj = it.second;
    uint32_t class_idx = gen_clas_items[obj.class_name];
    spdlog::debug("writing classidx @0x{:02X} {}", (uint32_t)ofs.tellp(), class_idx);
    ofs.write(reinterpret_cast<char*>(&class_idx), sizeof(class_idx));

    for(auto& property : obj.properties) {
      uint32_t property_idx = gen_property_table[gen_clas_items[obj.class_name]].properties[property->property_name];
      property->property_idx = property_idx;
      spdlog::debug("writing propidx @0x{:02X} {}", (uint32_t)ofs.tellp(), property_idx);
      ofs.write(reinterpret_cast<char*>(&property_idx), sizeof(property_idx));
      uint32_t ndf_type = property->property_type;
      spdlog::debug("writing ndf_type @0x{:02X} {}", (uint32_t)ofs.tellp(), ndf_type);
      ofs.write(reinterpret_cast<char*>(&ndf_type), sizeof(ndf_type));
      property->to_ndfbin(this, ofs);
    }
    // write last property
    uint32_t property_idx = 2880154539;
    ofs.write(reinterpret_cast<char*>(&property_idx), sizeof(property_idx));
  }

  toc_table.OBJE.size = (uint32_t)ofs.tellp() - toc_table.OBJE.offset;

  // write TOPO
  toc_table.TOPO.magic[0] = 'T';
  toc_table.TOPO.magic[1] = 'O';
  toc_table.TOPO.magic[2] = 'P';
  toc_table.TOPO.magic[3] = 'O';

  toc_table.TOPO.offset = ofs.tellp();

  spdlog::debug("writing topo @0x{:02X}", (uint32_t)ofs.tellp());
  for(uint32_t obj_idx : gen_topo_table) {
    ofs.write(reinterpret_cast<char*>(&obj_idx), sizeof(obj_idx));
  }

  toc_table.TOPO.size = (uint32_t)ofs.tellp() - toc_table.TOPO.offset;


  // write CHNK
  toc_table.CHNK.magic[0] = 'C';
  toc_table.CHNK.magic[1] = 'H';
  toc_table.CHNK.magic[2] = 'N';
  toc_table.CHNK.magic[3] = 'K';

  toc_table.CHNK.offset = ofs.tellp();

  spdlog::debug("writing chunk @0x{:02X}", (uint32_t)ofs.tellp());

  uint32_t object_count = object_map.size();
  ofs.write(reinterpret_cast<char*>(&object_count), sizeof(object_count));

  toc_table.CHNK.size = (uint32_t)ofs.tellp() - toc_table.CHNK.offset;

  // write CLAS
  toc_table.CLAS.magic[0] = 'C';
  toc_table.CLAS.magic[1] = 'L';
  toc_table.CLAS.magic[2] = 'A';
  toc_table.CLAS.magic[3] = 'S';

  toc_table.CLAS.offset = ofs.tellp();

  for(const auto& clas : gen_clas_table) {
    uint32_t length = clas.size();
    ofs.write(reinterpret_cast<char*>(&length), sizeof(length));
    ofs.write(clas.data(), length * sizeof(char));
  }

  toc_table.CLAS.size = (uint32_t)ofs.tellp() - toc_table.CLAS.offset;

  // write PROP
  toc_table.PROP.magic[0] = 'P';
  toc_table.PROP.magic[1] = 'R';
  toc_table.PROP.magic[2] = 'O';
  toc_table.PROP.magic[3] = 'P';

  toc_table.PROP.offset = ofs.tellp();

  spdlog::debug("writing properties @0x{:02X}", (uint32_t)ofs.tellp());
  for(const auto& [prop_name, class_idx] : gen_property_items) {
    uint32_t length = prop_name.size();
    ofs.write(reinterpret_cast<char*>(&length), sizeof(length));
    ofs.write(prop_name.data(), length * sizeof(char));
    uint32_t class_index = class_idx;
    ofs.write(reinterpret_cast<char*>(&class_index), sizeof(class_index));
    spdlog::debug("writing prop {} {}", prop_name, class_index);
  }

  toc_table.PROP.size = (uint32_t)ofs.tellp() - toc_table.PROP.offset;

  // write STRG
  toc_table.STRG.magic[0] = 'S';
  toc_table.STRG.magic[1] = 'T';
  toc_table.STRG.magic[2] = 'R';
  toc_table.STRG.magic[3] = 'G';

  toc_table.STRG.offset = ofs.tellp();

  spdlog::debug("writing strings @0x{:02X}", (uint32_t)ofs.tellp());
  for(const auto& str : gen_string_items) {
    uint32_t length = str.size();
    ofs.write(reinterpret_cast<char*>(&length), sizeof(length));
    ofs.write(str.data(), length * sizeof(char));
  }

  toc_table.STRG.size = (uint32_t)ofs.tellp() - toc_table.STRG.offset;

  // write TRAN
  toc_table.TRAN.magic[0] = 'T';
  toc_table.TRAN.magic[1] = 'R';
  toc_table.TRAN.magic[2] = 'A';
  toc_table.TRAN.magic[3] = 'N';

  toc_table.TRAN.offset = ofs.tellp();

  spdlog::debug("writing tran @0x{:02X}", (uint32_t)ofs.tellp());
  for(const auto& tran : gen_tran_table) {
    uint32_t length = tran.size();
    ofs.write(reinterpret_cast<char*>(&length), sizeof(length));
    ofs.write(tran.data(), length * sizeof(char));
  }

  toc_table.TRAN.size = (uint32_t)ofs.tellp() - toc_table.TRAN.offset;

  // write IMPR
  toc_table.IMPR.magic[0] = 'I';
  toc_table.IMPR.magic[1] = 'M';
  toc_table.IMPR.magic[2] = 'P';
  toc_table.IMPR.magic[3] = 'R';

  toc_table.IMPR.offset = ofs.tellp();

  spdlog::debug("writing impr @0x{:02X}", (uint32_t)ofs.tellp());
  save_ndfbin_imprs(gen_import_table, ofs);

  toc_table.IMPR.size = (uint32_t)ofs.tellp() - toc_table.IMPR.offset;

  // write EXPR
  toc_table.EXPR.magic[0] = 'E';
  toc_table.EXPR.magic[1] = 'X';
  toc_table.EXPR.magic[2] = 'P';
  toc_table.EXPR.magic[3] = 'R';

  toc_table.EXPR.offset = ofs.tellp();

  spdlog::debug("writing expr @0x{:02X}", (uint32_t)ofs.tellp());
  save_ndfbin_imprs(gen_export_table, ofs);

  toc_table.EXPR.size = (uint32_t)ofs.tellp() - toc_table.EXPR.offset;

  // write TOC0 Header
  header.toc0offset = (uint32_t)ofs.tellp();
  ofs.write(reinterpret_cast<char*>(&toc_table), sizeof(toc_table));

  // rewrite header
  header.size = (uint32_t)ofs.tellp() - 40;
  ofs.seekp(0);
  ofs.write(reinterpret_cast<char*>(&header), sizeof(header));
}
