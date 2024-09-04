#include "ndf.hpp"
#include "ndf_db.hpp"
#include "ndf_properties.hpp"
#include "sqlite_helpers.hpp"
#include <memory>
#include <ranges>

void NDF::save_as_ndf_xml(fs::path path) {
  pugi::xml_document doc;
  auto root = doc.append_child("NDF");

  for (const auto &[name, obj] : object_map) {
    auto object_node = root.append_child(obj.name.c_str());
    object_node.append_attribute("class") = obj.class_name.c_str();
    object_node.append_attribute("export_path") = obj.export_path.c_str();
    object_node.append_attribute("is_top_object") = obj.is_top_object;

    for (const auto &prop : obj.properties) {
      prop->to_ndf_xml(object_node);
    }
  }

  fs::create_directories(path.parent_path());
  doc.save_file(path.c_str());
}

void NDF::insert_into_db(NDF_DB *db, int ndf_id) {
  {
    // FIXME: just init only for all ndf types
    for (int i = 0; i < 0x25; i++) {
      db_property_map[i] = std::vector<NDFProperty *>();
    }
    auto begin = std::chrono::high_resolution_clock::now();
    {
      SQLTransaction trans(db->get_db());
      for (auto &[name, object] : object_map) {
        auto object_id_opt = db->insert_only_object(ndf_id, object);
        if (!object_id_opt) {
          spdlog::error("couldn't insert object!");
          trans.rollback();
          continue;
        }
        auto object_id = object_id_opt.value();
        spdlog::debug("inserted object {} into {}", object_id, ndf_id);
        for (auto &property : object.properties) {
          property->db_object_id = object_id;
          db_property_map[property->property_type].push_back(property.get());
        }
      }
    }
    auto end = std::chrono::high_resolution_clock::now();
    spdlog::info(
        "inserted objects {} in {} ms", object_map.size(),
        std::chrono::duration_cast<std::chrono::milliseconds>(end - begin)
            .count());
  }
  // insert values
  {
    spdlog::info("inserting NDF values to db");
    int i = 0;
    auto begin = std::chrono::high_resolution_clock::now();
    {
      SQLTransaction trans(db->get_db());
      for (auto [ndf_type, properties] : db_property_map) {
        spdlog::debug("inserting NDF type {}", ndf_type);
        for (auto prop : properties) {
          i += 1;
          prop->to_ndf_db(db);
        }
      }
    }
    auto end = std::chrono::high_resolution_clock::now();
    spdlog::info(
        "inserted properties {} in {} ms", i,
        std::chrono::duration_cast<std::chrono::milliseconds>(end - begin)
            .count());
  }
  // insert missing properties for types other than list/map/pair
  spdlog::debug("inserting missing NDF properties to db");
  {
    int i = 0;
    auto begin = std::chrono::high_resolution_clock::now();
    {
      SQLTransaction trans(db->get_db());
      for (auto [ndf_type, properties] : db_property_map) {
        if (ndf_type == NDFPropertyType::List) {
          continue;
        }
        if (ndf_type == NDFPropertyType::Map) {
          continue;
        }
        if (ndf_type == NDFPropertyType::Pair) {
          continue;
        }
        for (auto prop : properties) {
          i += 1;
          db->insert_only_property(*prop);
        }
      }
    }
    auto end = std::chrono::high_resolution_clock::now();
    spdlog::info(
        "inserted missing {} properties in {} ms", i,
        std::chrono::duration_cast<std::chrono::milliseconds>(end - begin)
            .count());
  }
  {
    auto begin = std::chrono::high_resolution_clock::now();
    {
      if (!db->fix_references(ndf_id)) {
        spdlog::error("unable to fix references!");
      }
    }
    auto end = std::chrono::high_resolution_clock::now();
    spdlog::info(
        "fixed references in {} ms",
        std::chrono::duration_cast<std::chrono::milliseconds>(end - begin)
            .count());
  }
  spdlog::debug("finished NDF db");
}

void NDF::load_from_ndf_xml(fs::path path) {
  pugi::xml_document doc;
  pugi::xml_parse_result result = doc.load_file(path.c_str());
  spdlog::info("Load result: {}", result.description());
  assert(result.status == pugi::status_ok);

  spdlog::info("parsing NDF objects");
  for (const auto &obj : doc.child("NDF").children()) {
    NDFObject object;
    object.name = obj.name();
    object.class_name = obj.attribute("class").as_string();
    object.export_path = obj.attribute("export_path").as_string();
    object.is_top_object = obj.attribute("is_top_object").as_bool();

    for (const auto &prop : obj.children()) {
      uint32_t ndf_type = prop.attribute("typeId").as_uint();
      std::unique_ptr<NDFProperty> property =
          NDFProperty::get_property_from_ndf_xml(ndf_type, prop);

      // property->db_object_id = object_id;
      //  add into the db_map
      db_property_map[ndf_type].push_back(property.get());

      property->from_ndf_xml(prop);
      object.add_property(std::move(property));
    }

    add_object(std::move(object));
  }
  fill_gen_object();
}

void NDF::load_imprs(std::istream &stream,
                     std::vector<std::string> current_import_path) {
  uint32_t tran_index;
  stream.read(reinterpret_cast<char *>(&tran_index), sizeof(uint32_t));
  uint32_t index;
  stream.read(reinterpret_cast<char *>(&index), sizeof(uint32_t));
  uint32_t count;
  stream.read(reinterpret_cast<char *>(&count), sizeof(uint32_t));

  uint32_t begin_offset = (uint32_t)stream.tellg();
  if (count > 0) {
    std::vector<uint32_t> offsets;
    offsets.resize(count);
    stream.read(reinterpret_cast<char *>(offsets.data()),
                sizeof(uint32_t) * count);

    for (uint32_t offset : offsets) {
      spdlog::debug("assertion @0x{:02X} is 0x{:02X} should be 0x{:02X}",
                    begin_offset, ((uint32_t)stream.tellg() - begin_offset),
                    offset);
      assert(offset == ((uint32_t)stream.tellg() - begin_offset));
      current_import_path.push_back(tran_table[tran_index]);
      load_imprs(stream, current_import_path);
      current_import_path.pop_back();
    }
  }

  if (index == 4294967295) {
    return;
  }

  auto foo = current_import_path | std::views::join_with('/');
  std::string tmp(foo.begin(), foo.end());
  import_name_table[index] = tmp + std::string("/") + tran_table[tran_index];
  spdlog::debug("Import: {}", import_name_table[index]);
}

void NDF::load_exprs(std::istream &stream,
                     std::vector<std::string> current_export_path) {
  uint32_t tran_index;
  stream.read(reinterpret_cast<char *>(&tran_index), sizeof(uint32_t));
  uint32_t index;
  stream.read(reinterpret_cast<char *>(&index), sizeof(uint32_t));
  uint32_t count;
  stream.read(reinterpret_cast<char *>(&count), sizeof(uint32_t));

  uint32_t begin_offset = (uint32_t)stream.tellg();
  if (count > 0) {
    std::vector<uint32_t> offsets;
    offsets.resize(count);
    stream.read(reinterpret_cast<char *>(offsets.data()),
                sizeof(uint32_t) * count);

    for (uint32_t offset : offsets) {
      assert(offset == ((uint32_t)stream.tellg() - begin_offset));
      current_export_path.push_back(tran_table[tran_index]);
      load_exprs(stream, current_export_path);
      current_export_path.pop_back();
    }
  }

  if (index == 4294967295) {
    return;
  }

  auto &obj = get_object(gen_object_items[index]);

  auto test = current_export_path | std::views::join_with('/');
  std::string tmp(test.begin(), test.end());
  obj.export_path = tmp + std::string("/") + tran_table[tran_index];
  spdlog::debug("Export: {}", obj.export_path);
}

std::optional<NDFObject> NDF::get_object(NDF_DB *db, int id) {
  return db->get_object(id);
}
std::optional<std::unique_ptr<NDFProperty>> NDF::get_property(NDF_DB *db,
                                                              int id) {
  return db->get_property(id);
}

void NDF::load_from_db(NDF_DB *db, int ndf_id) {
  // load all objects (empty)
  {
    auto begin = std::chrono::high_resolution_clock::now();
    {
      auto objects_opt = db->get_only_objects(ndf_id);
      if (!objects_opt.has_value()) {
        spdlog::error("failed to get objects from db");
        return;
      }
      auto &objects = objects_opt.value();
      for (auto &object : objects) {
        object_map.emplace(object.name, std::move(object));
      }
    }
    auto end = std::chrono::high_resolution_clock::now();
    spdlog::debug(
        "got objects from db in {} ms",
        std::chrono::duration_cast<std::chrono::milliseconds>(end - begin)
            .count());
  }
  {
    auto begin = std::chrono::high_resolution_clock::now();
    {
      auto object_it = object_map.begin();
      while (object_it != object_map.end()) {
        std::optional<std::vector<std::unique_ptr<NDFProperty>>>
            properties_opt = db->get_only_properties(object_it->second.db_id);
        if (properties_opt.has_value()) {
          spdlog::debug("loaded {} properties", properties_opt.value().size());
          object_it.value().properties = std::move(properties_opt.value());
        } else {
          spdlog::error("failed to get properties from db");
          return;
        }
        object_it++;
      }
    }
    auto end = std::chrono::high_resolution_clock::now();
    spdlog::debug(
        "got properties from db in {} ms",
        std::chrono::duration_cast<std::chrono::milliseconds>(end - begin)
            .count());
  }
  // load all properties
  spdlog::debug("finished NDF db");
}
