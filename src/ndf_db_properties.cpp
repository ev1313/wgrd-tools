#include "ndf_properties.hpp"

#include "ndf_db.hpp"

#include "sqlite_helpers.hpp"

std::unique_ptr<NDFProperty>
NDFProperty::get_property_from_ndf_db(uint32_t ndf_type,
                                      bool is_import_reference) {
  if (ndf_type == 0x9) {
    if (!is_import_reference) {
      return std::make_unique<NDFPropertyObjectReference>();
    } else {
      return std::make_unique<NDFPropertyImportReference>();
    }
  }
  return get_property_from_ndftype(ndf_type);
}

std::unique_ptr<NDFProperty>
NDFProperty::get_db_property_type(NDF_DB *db, int prop_id, int pos) {
  auto prop_opt = db->stmt_get_property.query_single<
      std::tuple<int, std::string, int, int, int, uint32_t, bool, int>>(
      prop_id);
  if (!prop_opt) {
    spdlog::error("Could not get property, id {}", prop_id);
    return nullptr;
  }
  auto [_, _, _, _, key_position, key_type, is_import_reference, _] =
      prop_opt.value();
  if (pos != -1 && pos != key_position) {
    spdlog::error("Position mismatch property required {} but is {}", pos,
                  key_position);
    return nullptr;
  }
  return get_property_from_ndf_db(key_type, is_import_reference);
}

// also sets the propery name and propery index
int NDFProperty::get_db_property_value(NDF_DB *db, int property_id) {
  auto prop_opt = db->stmt_get_property.query_single<
      std::tuple<int, std::string, int, int, int, uint32_t, bool, int>>(
      property_id);
  if (!prop_opt) {
    return false;
  }
  auto [_, prop_name, prop_idx, _, _, _, _, value_id] = prop_opt.value();
  property_name = prop_name;
  property_idx = prop_idx;
  return value_id;
}

std::optional<int>
NDFProperty::add_db_property(NDF_DB *db, int object_id, int parent,
                             int position, int value_id,
                             bool is_import_reference) const {
  std::optional<int> prop_id;
  if (parent == -1) {
    if (value_id == -1) {
      prop_id = db->stmt_insert_ndf_property.insert(
          object_id, property_name, property_idx, SQLNULL{}, SQLNULL{},
          property_type, is_import_reference, SQLNULL{});
    } else {

      prop_id = db->stmt_insert_ndf_property.insert(
          object_id, property_name, property_idx, SQLNULL{}, SQLNULL{},
          property_type, is_import_reference, value_id);
    }
  } else {
    if (value_id == -1) {
      prop_id = db->stmt_insert_ndf_property.insert(
          object_id, property_name, property_idx, parent, position,
          property_type, is_import_reference, SQLNULL{});
    } else {
      prop_id = db->stmt_insert_ndf_property.insert(
          object_id, property_name, property_idx, parent, position,
          property_type, is_import_reference, value_id);
    }
  }
  return prop_id;
}

bool NDFPropertyBool::from_ndf_db(NDF_DB *db, int property_id) {
  auto value_id = get_db_property_value(db, property_id);
  // get property type for assert only
  auto value_opt = db->stmt_get_bool_value.query_single<int>(value_id);
  if (!value_opt) {
    return false;
  }
  value = value_opt.value();
  return true;
}

bool NDFPropertyBool::to_ndf_db(NDF_DB *db, int object_id, int parent,
                                int position) const {
  // insert the value in the bool table
  auto value_id = db->stmt_insert_ndf_bool.insert(this->value);
  if (!value_id) {
    return -1;
  }
  return add_db_property(db, object_id, parent, position, value_id.value())
      .has_value();
}

bool NDFPropertyUInt8::from_ndf_db(NDF_DB *db, int property_id) {
  auto value_id = get_db_property_value(db, property_id);
  // get property type for assert only
  auto value_opt = db->stmt_get_uint8_value.query_single<int>(value_id);
  if (!value_opt) {
    return false;
  }
  value = value_opt.value();
  return true;
}

bool NDFPropertyUInt8::to_ndf_db(NDF_DB *db, int object_id, int parent,
                                 int position) const {
  // insert the value in the bool table
  auto value_id = db->stmt_insert_ndf_uint8.insert(this->value);
  if (!value_id) {
    return -1;
  }
  return add_db_property(db, object_id, parent, position, value_id.value())
      .has_value();
}

bool NDFPropertyUInt16::from_ndf_db(NDF_DB *db, int property_id) {
  auto value_id = get_db_property_value(db, property_id);
  // get property type for assert only
  auto value_opt = db->stmt_get_uint16_value.query_single<int>(value_id);
  if (!value_opt) {
    return false;
  }
  value = value_opt.value();
  return true;
}

bool NDFPropertyUInt16::to_ndf_db(NDF_DB *db, int object_id, int parent,
                                  int position) const {
  // insert the value in the bool table
  auto value_id = db->stmt_insert_ndf_uint16.insert(this->value);
  if (!value_id) {
    return -1;
  }

  return add_db_property(db, object_id, parent, position, value_id.value())
      .has_value();
}

bool NDFPropertyInt16::from_ndf_db(NDF_DB *db, int property_id) {
  auto value_id = get_db_property_value(db, property_id);
  // get property type for assert only
  auto value_opt = db->stmt_get_int16_value.query_single<int>(value_id);
  if (!value_opt) {
    return false;
  }
  value = value_opt.value();
  return true;
}

bool NDFPropertyInt16::to_ndf_db(NDF_DB *db, int object_id, int parent,
                                 int position) const {
  // insert the value in the bool table
  auto value_id = db->stmt_insert_ndf_int16.insert(this->value);
  if (!value_id) {
    return -1;
  }
  return add_db_property(db, object_id, parent, position, value_id.value())
      .has_value();
}

bool NDFPropertyUInt32::from_ndf_db(NDF_DB *db, int property_id) {
  auto value_id = get_db_property_value(db, property_id);
  // get property type for assert only
  auto value_opt = db->stmt_get_uint32_value.query_single<int>(value_id);
  if (!value_opt) {
    return false;
  }
  value = value_opt.value();
  return true;
}

bool NDFPropertyUInt32::to_ndf_db(NDF_DB *db, int object_id, int parent,
                                  int position) const {
  // insert the value in the bool table
  auto value_id = db->stmt_insert_ndf_uint32.insert(this->value);
  if (!value_id) {
    return -1;
  }
  return add_db_property(db, object_id, parent, position, value_id.value())
      .has_value();
}

bool NDFPropertyInt32::from_ndf_db(NDF_DB *db, int property_id) {
  auto value_id = get_db_property_value(db, property_id);
  // get property type for assert only
  auto value_opt = db->stmt_get_int32_value.query_single<int>(value_id);
  if (!value_opt) {
    return false;
  }
  value = value_opt.value();
  return true;
}

bool NDFPropertyInt32::to_ndf_db(NDF_DB *db, int object_id, int parent,
                                 int position) const {
  // insert the value in the bool table
  auto value_id = db->stmt_insert_ndf_int32.insert(this->value);
  if (!value_id) {
    return -1;
  }
  return add_db_property(db, object_id, parent, position, value_id.value())
      .has_value();
}

bool NDFPropertyFloat32::from_ndf_db(NDF_DB *db, int property_id) {
  auto value_id = get_db_property_value(db, property_id);
  // get property type for assert only
  auto value_opt = db->stmt_get_float32_value.query_single<float>(value_id);
  if (!value_opt) {
    return false;
  }
  value = value_opt.value();
  return true;
}

bool NDFPropertyFloat32::to_ndf_db(NDF_DB *db, int object_id, int parent,
                                   int position) const {
  // insert the value in the bool table
  auto value_id = db->stmt_insert_ndf_float32.insert(this->value);
  if (!value_id) {
    return -1;
  }
  return add_db_property(db, object_id, parent, position, value_id.value())
      .has_value();
}

bool NDFPropertyFloat64::from_ndf_db(NDF_DB *db, int property_id) {
  auto value_id = get_db_property_value(db, property_id);
  // get property type for assert only
  auto value_opt = db->stmt_get_float64_value.query_single<double>(value_id);
  if (!value_opt) {
    return false;
  }
  value = value_opt.value();
  return true;
}

bool NDFPropertyFloat64::to_ndf_db(NDF_DB *db, int object_id, int parent,
                                   int position) const {
  // insert the value in the bool table
  auto value_id = db->stmt_insert_ndf_float64.insert(this->value);
  if (!value_id) {
    return -1;
  }
  return add_db_property(db, object_id, parent, position, value_id.value())
      .has_value();
}

bool NDFPropertyString::from_ndf_db(NDF_DB *db, int property_id) {
  auto value_id = get_db_property_value(db, property_id);
  // get property type for assert only
  auto value_opt =
      db->stmt_get_string_value.query_single<std::string>(value_id);
  if (!value_opt) {
    return false;
  }
  value = value_opt.value();
  return true;
}

bool NDFPropertyString::to_ndf_db(NDF_DB *db, int object_id, int parent,
                                  int position) const {
  // insert the value in the bool table
  auto value_id = db->stmt_insert_ndf_string.insert(this->value);
  if (!value_id) {
    return -1;
  }
  return add_db_property(db, object_id, parent, position, value_id.value())
      .has_value();
}

bool NDFPropertyWideString::from_ndf_db(NDF_DB *db, int property_id) {
  auto value_id = get_db_property_value(db, property_id);
  // get property type for assert only
  auto value_opt =
      db->stmt_get_widestring_value.query_single<std::string>(value_id);
  if (!value_opt) {
    return false;
  }
  value = value_opt.value();
  return true;
}

bool NDFPropertyWideString::to_ndf_db(NDF_DB *db, int object_id, int parent,
                                      int position) const {
  // insert the value in the bool table
  auto value_id = db->stmt_insert_ndf_widestring.insert(this->value);
  if (!value_id) {
    return -1;
  }
  return add_db_property(db, object_id, parent, position, value_id.value())
      .has_value();
}

bool NDFPropertyF32_vec2::from_ndf_db(NDF_DB *db, int property_id) {
  auto value_id = get_db_property_value(db, property_id);
  // get property type for assert only
  auto value_opt =
      db->stmt_get_F32_vec2_value.query_single<std::tuple<float, float>>(
          value_id);
  if (!value_opt) {
    return false;
  }
  x = std::get<0>(value_opt.value());
  y = std::get<1>(value_opt.value());
  return true;
}

bool NDFPropertyF32_vec2::to_ndf_db(NDF_DB *db, int object_id, int parent,
                                    int position) const {
  // insert the value in the bool table
  auto value_id = db->stmt_insert_ndf_F32_vec2.insert(this->x, this->y);
  if (!value_id) {
    return -1;
  }
  return add_db_property(db, object_id, parent, position, value_id.value())
      .has_value();
}

bool NDFPropertyF32_vec3::from_ndf_db(NDF_DB *db, int property_id) {
  auto value_id = get_db_property_value(db, property_id);
  // get property type for assert only
  auto value_opt =
      db->stmt_get_F32_vec3_value.query_single<std::tuple<float, float, float>>(
          value_id);
  if (!value_opt) {
    return false;
  }
  x = std::get<0>(value_opt.value());
  y = std::get<1>(value_opt.value());
  z = std::get<2>(value_opt.value());
  return true;
}

bool NDFPropertyF32_vec3::to_ndf_db(NDF_DB *db, int object_id, int parent,
                                    int position) const {
  // insert the value in the bool table
  auto value_id =
      db->stmt_insert_ndf_F32_vec3.insert(this->x, this->y, this->z);
  if (!value_id) {
    return -1;
  }
  return add_db_property(db, object_id, parent, position, value_id.value())
      .has_value();
}

bool NDFPropertyF32_vec4::from_ndf_db(NDF_DB *db, int property_id) {
  auto value_id = get_db_property_value(db, property_id);
  // get property type for assert only
  auto value_opt =
      db->stmt_get_F32_vec4_value
          .query_single<std::tuple<float, float, float, float>>(value_id);
  if (!value_opt) {
    return false;
  }
  x = std::get<0>(value_opt.value());
  y = std::get<1>(value_opt.value());
  z = std::get<2>(value_opt.value());
  w = std::get<3>(value_opt.value());
  return true;
}

bool NDFPropertyF32_vec4::to_ndf_db(NDF_DB *db, int object_id, int parent,
                                    int position) const {
  // insert the value in the bool table
  auto value_id =
      db->stmt_insert_ndf_F32_vec4.insert(this->x, this->y, this->z, this->w);
  if (!value_id) {
    return -1;
  }
  return add_db_property(db, object_id, parent, position, value_id.value())
      .has_value();
}

bool NDFPropertyColor::from_ndf_db(NDF_DB *db, int property_id) {
  auto value_id = get_db_property_value(db, property_id);
  // get property type for assert only
  auto value_opt =
      db->stmt_get_color_value.query_single<std::tuple<int, int, int, int>>(
          value_id);
  if (!value_opt) {
    return false;
  }
  r = std::get<0>(value_opt.value());
  g = std::get<1>(value_opt.value());
  b = std::get<2>(value_opt.value());
  a = std::get<3>(value_opt.value());
  return true;
}

bool NDFPropertyColor::to_ndf_db(NDF_DB *db, int object_id, int parent,
                                 int position) const {
  // insert the value in the bool table
  auto value_id =
      db->stmt_insert_ndf_color.insert(this->r, this->g, this->b, this->a);
  if (!value_id) {
    return -1;
  }
  return add_db_property(db, object_id, parent, position, value_id.value())
      .has_value();
}

bool NDFPropertyS32_vec2::from_ndf_db(NDF_DB *db, int property_id) {
  auto value_id = get_db_property_value(db, property_id);
  // get property type for assert only
  auto value_opt =
      db->stmt_get_S32_vec2_value.query_single<std::tuple<int, int>>(value_id);
  if (!value_opt) {
    return false;
  }
  x = std::get<0>(value_opt.value());
  y = std::get<1>(value_opt.value());
  return true;
}

bool NDFPropertyS32_vec2::to_ndf_db(NDF_DB *db, int object_id, int parent,
                                    int position) const {
  // insert the value in the bool table
  auto value_id = db->stmt_insert_ndf_S32_vec2.insert(this->x, this->y);
  if (!value_id) {
    return -1;
  }
  return add_db_property(db, object_id, parent, position, value_id.value())
      .has_value();
}

bool NDFPropertyS32_vec3::from_ndf_db(NDF_DB *db, int property_id) {
  auto value_id = get_db_property_value(db, property_id);
  // get property type for assert only
  auto value_opt =
      db->stmt_get_S32_vec3_value.query_single<std::tuple<int, int, int>>(
          value_id);
  if (!value_opt) {
    return false;
  }
  x = std::get<0>(value_opt.value());
  y = std::get<1>(value_opt.value());
  z = std::get<2>(value_opt.value());
  return true;
}

bool NDFPropertyS32_vec3::to_ndf_db(NDF_DB *db, int object_id, int parent,
                                    int position) const {
  // insert the value in the bool table
  auto value_id =
      db->stmt_insert_ndf_S32_vec3.insert(this->x, this->y, this->z);
  if (!value_id) {
    return -1;
  }
  return add_db_property(db, object_id, parent, position, value_id.value())
      .has_value();
}

bool NDFPropertyImportReference::from_ndf_db(NDF_DB *db, int property_id) {
  auto value_id = get_db_property_value(db, property_id);
  // get property type for assert only
  auto value_opt =
      db->stmt_get_import_reference_value.query_single<std::string>(value_id);
  if (!value_opt) {
    return false;
  }
  import_name = value_opt.value();
  return true;
}

bool NDFPropertyImportReference::to_ndf_db(NDF_DB *db, int object_id,
                                           int parent, int position) const {
  // insert the value in the bool table
  auto value_id = db->stmt_insert_ndf_import_reference.insert(import_name);
  if (!value_id) {
    return -1;
  }
  // note the extra true flag for is_import_reference
  return add_db_property(db, object_id, parent, position, value_id.value(),
                         true)
      .has_value();
}

bool NDFPropertyObjectReference::from_ndf_db(NDF_DB *db, int property_id) {
  auto value_id = get_db_property_value(db, property_id);
  // get property type for assert only
  auto value_opt =
      db->stmt_get_object_reference_value.query_single<std::string>(value_id);
  if (!value_opt) {
    return false;
  }
  object_name = value_opt.value();
  return true;
}

bool NDFPropertyObjectReference::to_ndf_db(NDF_DB *db, int object_id,
                                           int parent, int position) const {
  // insert the value in the bool table
  auto value_id = db->stmt_insert_ndf_object_reference.insert(object_name);
  if (!value_id) {
    return -1;
  }
  return add_db_property(db, object_id, parent, position, value_id.value())
      .has_value();
}

bool NDFPropertyGUID::from_ndf_db(NDF_DB *db, int property_id) {
  auto value_id = get_db_property_value(db, property_id);
  // get property type for assert only
  auto value_opt = db->stmt_get_GUID_value.query_single<std::string>(value_id);
  if (!value_opt) {
    return false;
  }
  guid = value_opt.value();
  return true;
}

bool NDFPropertyGUID::to_ndf_db(NDF_DB *db, int object_id, int parent,
                                int position) const {
  // insert the value in the bool table
  auto value_id = db->stmt_insert_ndf_GUID.insert(guid);
  if (!value_id) {
    return -1;
  }
  return add_db_property(db, object_id, parent, position, value_id.value())
      .has_value();
}

bool NDFPropertyPathReference::from_ndf_db(NDF_DB *db, int property_id) {
  auto value_id = get_db_property_value(db, property_id);
  // get property type for assert only
  auto value_opt =
      db->stmt_get_path_reference_value.query_single<std::string>(value_id);
  if (!value_opt) {
    return false;
  }
  path = value_opt.value();
  return true;
}

bool NDFPropertyPathReference::to_ndf_db(NDF_DB *db, int object_id, int parent,
                                         int position) const {
  // insert the value in the bool table
  auto value_id = db->stmt_insert_ndf_path_reference.insert(path);
  if (!value_id) {
    return -1;
  }
  return add_db_property(db, object_id, parent, position, value_id.value())
      .has_value();
}

bool NDFPropertyLocalisationHash::from_ndf_db(NDF_DB *db, int property_id) {
  auto value_id = get_db_property_value(db, property_id);
  // get property type for assert only
  auto value_opt =
      db->stmt_get_localisation_hash_value.query_single<std::string>(value_id);
  if (!value_opt) {
    return false;
  }
  hash = value_opt.value();
  return true;
}

bool NDFPropertyLocalisationHash::to_ndf_db(NDF_DB *db, int object_id,
                                            int parent, int position) const {
  // insert the value in the bool table
  auto value_id = db->stmt_insert_ndf_localisation_hash.insert(hash);
  if (!value_id) {
    return -1;
  }
  return add_db_property(db, object_id, parent, position, value_id.value())
      .has_value();
}

bool NDFPropertyHash::from_ndf_db(NDF_DB *db, int property_id) {
  auto value_id = get_db_property_value(db, property_id);
  // get property type for assert only
  auto value_opt = db->stmt_get_hash_value.query_single<std::string>(value_id);
  if (!value_opt) {
    return false;
  }
  hash = value_opt.value();
  return true;
}

bool NDFPropertyHash::to_ndf_db(NDF_DB *db, int object_id, int parent,
                                int position) const {
  // insert the value in the bool table
  auto value_id = db->stmt_insert_ndf_hash.insert(hash);
  if (!value_id) {
    return -1;
  }
  return add_db_property(db, object_id, parent, position, value_id.value())
      .has_value();
}

bool NDFPropertyList::from_ndf_db(NDF_DB *db, int property_id) {
  // first we need to get the id of the list property
  auto value_id = get_db_property_value(db, property_id);
  // now we get all the properties in us
  auto value_opt = db->stmt_get_list_items.query<int>(value_id);
  if (!value_opt) {
    return false;
  }
  for (auto [pos, prop_id] : value_opt.value() | std::views::enumerate) {
    // now get the corresponding properties and initialize them
    auto prop = get_db_property_type(db, prop_id, pos);
    prop->from_ndf_db(db, prop_id);
    values.push_back(std::move(prop));
  }
  return true;
}

bool NDFPropertyList::to_ndf_db(NDF_DB *db, int object_id, int parent,
                                int position) const {
  int pos = 0;
  // first we insert the property for us in the table, note we do not have a
  auto prop_id_opt = add_db_property(db, object_id, parent, position, -1);
  if (!prop_id_opt) {
    spdlog::error("Couldn't add property {}", property_name);
    return false;
  }
  int prop_id = prop_id_opt.value();
  for (auto &prop : values) {
    // insert the property into the db
    bool ret = prop->to_ndf_db(db, object_id, prop_id, pos);
    if (!ret) {
      spdlog::error("Could not insert list item into db for property {}", pos);
      return false;
    }
    pos += 1;
  }
  return true;
}

bool NDFPropertyMap::from_ndf_db(NDF_DB *db, int property_id) {
  // first we need to get the id of the map property
  auto value_id = get_db_property_value(db, property_id);
  // now we get all the properties in us
  auto value_opt = db->stmt_get_list_items.query<int>(value_id);
  if (!value_opt) {
    return false;
  }
  int pos = 0;
  auto &value = value_opt.value();
  auto prop_it = value.begin();
  while (prop_it != value.end()) {
    auto key_prop_id = *prop_it;
    // handle key property
    auto key_prop = get_db_property_type(db, key_prop_id, pos++);
    key_prop->from_ndf_db(db, key_prop_id);
    pos += 1;
    prop_it++;
    if (prop_it == value.end()) {
      spdlog::error("No value property after key property! @{}", pos);
      return false;
    }
    // handle value property
    auto value_prop_id = *prop_it;
    // now get the corresponding properties and initialize them
    auto value_prop = get_db_property_type(db, value_prop_id, pos++);
    value_prop->from_ndf_db(db, value_prop_id);
    pos += 1;
    prop_it++;

    values.push_back({std::move(key_prop), std::move(value_prop)});
  }
  return true;
}

bool NDFPropertyMap::to_ndf_db(NDF_DB *db, int object_id, int parent,
                               int position) const {
  int pos = 0;
  // first we insert the property for us in the table, note we do not have a
  auto prop_id_opt = add_db_property(db, object_id, parent, position, -1);
  if (!prop_id_opt) {
    spdlog::error("Couldn't add property {}", property_name);
    return false;
  }
  int prop_id = prop_id_opt.value();
  for (auto &[key, value] : values) {
    // insert the property into the db
    bool ret = key->to_ndf_db(db, object_id, prop_id, pos);
    if (!ret) {
      spdlog::error("Could not insert map key into db @{}", pos);
      return false;
    }
    pos += 1;
    ret = value->to_ndf_db(db, object_id, prop_id, pos);
    if (!ret) {
      spdlog::error("Could not insert map value into db @{}", pos);
      return false;
    }
    pos += 1;
  }
  return true;
}

bool NDFPropertyPair::from_ndf_db(NDF_DB *db, int property_id) {
  // first we need to get the id of the map property
  auto value_id = get_db_property_value(db, property_id);
  // now we get all the properties in us
  auto value_opt = db->stmt_get_list_items.query<int>(value_id);
  if (!value_opt) {
    return false;
  }
  int pos = 0;
  auto &value = value_opt.value();
  auto prop_it = value.begin();
  auto key_prop_id = *prop_it;
  // handle key property
  auto key_prop = get_db_property_type(db, key_prop_id, pos++);
  if (!key_prop) {
    spdlog::error("Couldn't get key property type!");
    return false;
  }
  key_prop->from_ndf_db(db, key_prop_id);
  first = std::move(key_prop);
  pos += 1;
  prop_it++;
  if (prop_it == value.end()) {
    spdlog::error("No second property after first property!");
    return false;
  }
  // handle value property
  auto value_prop_id = *prop_it;
  // now get the corresponding properties and initialize them
  auto value_prop = get_db_property_type(db, value_prop_id, pos++);
  if (!value_prop) {
    spdlog::error("Couldn't get value property type!");
    return false;
  }
  value_prop->from_ndf_db(db, value_prop_id);
  prop_it++;
  second = std::move(value_prop);
  return true;
}

bool NDFPropertyPair::to_ndf_db(NDF_DB *db, int object_id, int parent,
                                int position) const {
  int pos = 0;
  // first we insert the property for us in the table, note we do not have a
  auto prop_id_opt = add_db_property(db, object_id, parent, position, -1);
  if (!prop_id_opt) {
    spdlog::error("Couldn't add property {}", property_name);
    return false;
  }
  int prop_id = prop_id_opt.value();
  // insert the property into the db
  bool ret = first->to_ndf_db(db, object_id, prop_id, pos);
  if (!ret) {
    spdlog::error("Could not insert pair first into db @{}", pos);
    return false;
  }
  pos += 1;
  ret = second->to_ndf_db(db, object_id, prop_id, pos);
  if (!ret) {
    spdlog::error("Could not insert pair second into db @{}", pos);
    return false;
  }
  pos += 1;
  return true;
}
