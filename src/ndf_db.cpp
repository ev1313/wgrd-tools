#include "ndf_db.hpp"
#include "sqlite_helpers.hpp"

#include <optional>
#include <spdlog/spdlog.h>

constexpr auto sql_create_table_value =
    "CREATE TABLE IF NOT EXISTS ndf_{0} (id INTEGER PRIMARY KEY AUTOINCREMENT, "
    "value {1});";
constexpr auto sql_insert_value = "INSERT INTO ndf_{0} (value) VALUES (?);";
constexpr auto sql_get_value = "SELECT value FROM ndf_{0} WHERE id=?";
constexpr auto sql_set_value = "UPDATE ndf_{0} SET value=? WHERE id=?";
constexpr auto sql_get_distinct_value =
    "SELECT DISTINCT ndf_{0}.value FROM ndf_{0} INNER JOIN ndf_property ON "
    "ndf_property.value=ndf_{0}.id WHERE ndf_property.object_id=? ORDER BY "
    "ndf_{0}.value;";

#define ndf_property_simple(NAME, DATATYPE)                                    \
  create_table(#NAME, std::format(sql_create_table_value, #NAME, #DATATYPE));  \
  stmt_insert_ndf_##NAME.init(db, std::format(sql_insert_value, #NAME));       \
  stmt_get_##NAME##_value.init(db, std::format(sql_get_value, #NAME));         \
  stmt_set_##NAME##_value.init(db, std::format(sql_set_value, #NAME));         \
  stmt_get_distinct_##NAME##_value.init(                                       \
      db, std::format(sql_get_distinct_value, #NAME));

bool NDF_DB::init_statements() {
  // sqlite3_exec(db, "PRAGMA synchronous = FULL", NULL, NULL, NULL);
  // sqlite3_exec(db, "PRAGMA journal_mode = WAL", NULL, NULL, NULL);
  sqlite3_exec(db, "PRAGMA optimize = 0x10002", NULL, NULL, NULL);
  sqlite3_exec(db, "PRAGMA cache_size = -10240", NULL, NULL, NULL);
  sqlite3_exec(db, "PRAGMA page_size = 32768", NULL, NULL, NULL);
  create_table("ndf_file",
               R"( CREATE TABLE IF NOT EXISTS ndf_file(
                                            id INTEGER PRIMARY KEY AUTOINCREMENT,
                                            vfs_path TEXT,
                                            dat_path TEXT,
                                            fs_path TEXT,
                                            game_version TEXT,
                                            is_current BOOLEAN
                                            ); )");
  create_table("ndf_object",
               R"( CREATE TABLE IF NOT EXISTS ndf_object(
                                          id INTEGER PRIMARY KEY AUTOINCREMENT,
                                          ndf_id INTEGER NOT NULL REFERENCES ndf_file(id) ON UPDATE CASCADE ON DELETE CASCADE,
                                          object_name TEXT,
                                          class_name TEXT,
                                          export_path TEXT,
                                          is_top_object BOOLEAN
                                          ); )");
  create_table("ndf_property",
               R"( CREATE TABLE IF NOT EXISTS ndf_property(
                                            id INTEGER PRIMARY KEY AUTOINCREMENT,
                                            object_id INTEGER NOT NULL REFERENCES ndf_object(id) ON UPDATE CASCADE ON DELETE CASCADE,
                                            property_name TEXT,
                                            property_index INTEGER,
                                            parent INTEGER REFERENCES ndf_property(id) ON UPDATE CASCADE ON DELETE CASCADE,
                                            position INTEGER,
                                            type INTEGER,
                                            is_import_reference BOOLEAN,
                                            value INTEGER
                                            ); )");
  // class db
  create_table("ndf_class", R"(
      CREATE TABLE IF NOT EXISTS ndf_class(
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        class_name TEXT
      );
  )");
  create_table("ndf_class_property", R"(
      CREATE TABLE IF NOT EXISTS ndf_class_property(
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        class_id INTEGER REFERENCES ndf_class(id) ON UPDATE CASCADE ON DELETE CASCADE,
        property_name TEXT,
        property_index INTEGER
      );
  )");
  stmt_insert_class.init(db,
                         R"( INSERT INTO ndf_class (class_name) VALUES (?); )");
  stmt_insert_class_property.init(
      db,
      R"( INSERT INTO ndf_class_property (class_id, property_name, property_index) VALUES (?, ?, ?); )");
  stmt_get_class_properties.init(
      db,
      R"( SELECT property_name, property_index FROM ndf_class_property INNER JOIN ndf_class ON ndf_class.id=ndf_class_property.class_id WHERE class_name=?; )");

  create_table("ndf_F32_vec2",
               R"( CREATE TABLE IF NOT EXISTS ndf_F32_vec2(
                                          id INTEGER PRIMARY KEY AUTOINCREMENT,
                                          value_x REAL,
                                          value_y REAL
                                          ); )");
  create_table("ndf_F32_vec3",
               R"( CREATE TABLE IF NOT EXISTS ndf_F32_vec3(
                                          id INTEGER PRIMARY KEY AUTOINCREMENT,
                                          value_x REAL,
                                          value_y REAL,
                                          value_z REAL
                                          ); )");
  create_table("ndf_F32_vec4",
               R"( CREATE TABLE IF NOT EXISTS ndf_F32_vec4(
                                          id INTEGER PRIMARY KEY AUTOINCREMENT,
                                          value_x REAL,
                                          value_y REAL,
                                          value_z REAL,
                                          value_w REAL
                                          ); )");
  create_table("ndf_S32_vec2",
               R"( CREATE TABLE IF NOT EXISTS ndf_S32_vec2(
                                          id INTEGER PRIMARY KEY AUTOINCREMENT,
                                          value_x INTEGER,
                                          value_y INTEGER
                                          ); )");
  create_table("ndf_S32_vec3",
               R"( CREATE TABLE IF NOT EXISTS ndf_S32_vec3(
                                          id INTEGER PRIMARY KEY AUTOINCREMENT,
                                          value_x INTEGER,
                                          value_y INTEGER,
                                          value_z INTEGER
                                          ); )");
  create_table("ndf_S32_vec4",
               R"( CREATE TABLE IF NOT EXISTS ndf_S32_vec4(
                                          id INTEGER PRIMARY KEY AUTOINCREMENT,
                                          value_x INTEGER,
                                          value_y INTEGER,
                                          value_z INTEGER,
                                          value_w INTEGER
                                          ); )");
  create_table("ndf_color",
               R"( CREATE TABLE IF NOT EXISTS ndf_color(
                                                           id INTEGER PRIMARY KEY AUTOINCREMENT,
                                                           value_r INTEGER,
                                                           value_g INTEGER,
                                                           value_b INTEGER,
                                                           value_a INTEGER
                                                           ); )");
  create_table("ndf_object_reference",
               R"( CREATE TABLE IF NOT EXISTS ndf_object_reference(
                                         id INTEGER PRIMARY KEY AUTOINCREMENT,
                                         referenced_object INTEGER REFERENCES ndf_object(id) ON UPDATE CASCADE ON DELETE SET NULL,
                                         optional_value TEXT
                                         ); )");
  create_table("ndf_import_reference",
               R"( CREATE TABLE IF NOT EXISTS ndf_import_reference(
                                                    id INTEGER PRIMARY KEY AUTOINCREMENT,
                                                    referenced_object INTEGER REFERENCES ndf_object(id) ON UPDATE CASCADE ON DELETE SET NULL,
                                                    optional_value TEXT
                                                    ); )");

  ndf_property_simple(bool, BOOLEAN);
  ndf_property_simple(uint8, INTEGER);
  ndf_property_simple(int8, INTEGER);
  ndf_property_simple(uint16, INTEGER);
  ndf_property_simple(int16, INTEGER);
  ndf_property_simple(uint32, INTEGER);
  ndf_property_simple(int32, INTEGER);
  ndf_property_simple(float32, REAL);
  ndf_property_simple(float64, REAL);
  ndf_property_simple(string, TEXT);
  ndf_property_simple(widestring, TEXT);
  ndf_property_simple(path_reference, TEXT);
  ndf_property_simple(GUID, TEXT);
  ndf_property_simple(localisation_hash, TEXT);
  ndf_property_simple(hash, TEXT);

  // inserters
  stmt_insert_ndf_file.init(
      db,
      R"( INSERT INTO ndf_file (vfs_path, dat_path, fs_path, game_version, is_current) VALUES (?,?,?,?,?); )");
  stmt_insert_ndf_object.init(
      db,
      R"( INSERT INTO ndf_object (ndf_id, object_name, class_name, export_path, is_top_object) VALUES (?,?,?,?,?); )");
  stmt_insert_ndf_property.init(
      db,
      R"( INSERT INTO ndf_property (object_id, property_name, property_index, parent, position, type, is_import_reference, value) VALUES (?,?,?,?,?,?,?,?); )");
  stmt_insert_ndf_F32_vec2.init(
      db, R"( INSERT INTO ndf_F32_vec2 (value_x, value_y) VALUES (?,?); )");
  stmt_insert_ndf_F32_vec3.init(
      db,
      R"( INSERT INTO ndf_f32_vec3 (value_x, value_y, value_z) VALUES (?,?,?); )");
  stmt_insert_ndf_F32_vec4.init(
      db,
      R"( INSERT INTO ndf_F32_vec4 (value_x, value_y, value_z, value_w) VALUES (?,?,?,?); )");
  stmt_insert_ndf_S32_vec2.init(
      db, R"( INSERT INTO ndf_S32_vec2 (value_x, value_y) VALUES (?,?); )");
  stmt_insert_ndf_S32_vec3.init(
      db,
      R"( INSERT INTO ndf_S32_vec3 (value_x, value_y, value_z) VALUES (?,?,?); )");
  stmt_insert_ndf_S32_vec4.init(
      db,
      R"( INSERT INTO ndf_S32_vec4 (value_x, value_y, value_z, value_w) VALUES (?,?,?,?); )");
  stmt_insert_ndf_color.init(
      db,
      R"( INSERT INTO ndf_color (value_r, value_g, value_b, value_a) VALUES (?,?,?,?); )");
  stmt_insert_ndf_object_reference.init(
      db,
      R"( INSERT INTO ndf_object_reference (referenced_object, optional_value) VALUES (?,?); )");
  stmt_insert_ndf_import_reference.init(
      db,
      R"( INSERT INTO ndf_import_reference (referenced_object, optional_value) VALUES (?,?); )");

  // accessors
  stmt_get_file_from_paths.init(
      db, R"( SELECT id FROM ndf_file WHERE vfs_path=? AND fs_path=?; )");
  stmt_get_object_from_name.init(
      db, R"( SELECT id FROM ndf_object WHERE object_name=?; )");
  stmt_get_object_from_export_path.init(
      db, R"( SELECT id FROM ndf_object WHERE export_path=?; )");
  stmt_get_object_ndf_id.init(db,
                              R"( SELECT ndf_id FROM ndf_object WHERE id=?; )");
  stmt_get_object_full_ndf_id.init(
      db,
      R"( SELECT id, object_name, class_name, export_path, is_top_object FROM ndf_object WHERE ndf_id=?; )");
  stmt_get_object_name.init(
      db, R"( SELECT object_name FROM ndf_object WHERE id=?; )");
  stmt_get_object_names.init(
      db, R"( SELECT object_name FROM ndf_object WHERE ndf_id=?; )");
  stmt_get_object_class_names.init(
      db, R"( SELECT DISTINCT class_name FROM ndf_object WHERE ndf_id=?; )");
  stmt_get_object_export_path.init(
      db, R"( SELECT export_path FROM ndf_object WHERE id=?; )");
  stmt_get_object.init(
      db,
      R"( SELECT ndf_id, object_name, class_name, export_path, is_top_object FROM ndf_object WHERE id=?; )");
  stmt_get_object_properties.init(
      db,
      R"( SELECT id FROM ndf_property WHERE object_id=? AND parent IS NULL; )");
  stmt_get_property_names.init(
      db,
      R"( SELECT property_name FROM ndf_property WHERE object_id=? AND property_index<>-1; )");

  stmt_get_property.init(
      db,
      R"( SELECT object_id, property_name, property_index, parent, position, type, is_import_reference, value FROM ndf_property WHERE id=?; )");

  // value accessors
  stmt_get_F32_vec2_value.init(
      db, R"( SELECT value_x, value_y FROM ndf_F32_vec2 WHERE id=?; )");
  stmt_get_F32_vec3_value.init(
      db,
      R"( SELECT value_x, value_y, value_z FROM ndf_F32_vec3 WHERE id=?; )");
  stmt_get_F32_vec4_value.init(
      db,
      R"( SELECT value_x, value_y, value_z, value_w FROM ndf_F32_vec4 WHERE id=?; )");
  stmt_get_S32_vec2_value.init(
      db, R"( SELECT value_x, value_y FROM ndf_S32_vec2 WHERE id=?; )");
  stmt_get_S32_vec3_value.init(
      db,
      R"( SELECT value_x, value_y, value_z FROM ndf_S32_vec3 WHERE id=?; )");
  stmt_get_S32_vec4_value.init(
      db,
      R"( SELECT value_x, value_y, value_z, value_w FROM ndf_S32_vec4 WHERE id=?; )");
  stmt_get_color_value.init(
      db,
      R"( SELECT value_r, value_g, value_b, value_a FROM ndf_color WHERE id=?; )");
  stmt_get_object_reference_value.init(
      db,
      R"( SELECT referenced_object, optional_value FROM ndf_object_reference WHERE id=?; )");
  stmt_get_import_reference_value.init(
      db,
      R"( SELECT referenced_object, optional_value FROM ndf_import_reference WHERE id=?; )");
  // used by list, map and pair
  stmt_get_list_items.init(
      db, R"( SELECT id FROM ndf_property WHERE parent=? ORDER BY position; )");
  stmt_get_objects_referencing.init(
      db,
      R"( SELECT DISTINCT prop.object_id FROM ndf_object_reference AS ref INNER JOIN ndf_property AS prop ON prop.value=ref.id WHERE ref.referenced_object=?; )");
  stmt_get_objects_importing.init(
      db,
      R"( SELECT DISTINCT prop.object_id FROM ndf_import_reference AS ref INNER JOIN ndf_property AS prop ON prop.value=ref.id WHERE ref.referenced_object=?; )");

  // change values
  stmt_set_F32_vec2_value.init(
      db, R"( UPDATE ndf_F32_vec2 SET value_x=?, value_y=? WHERE id=?; )");
  stmt_set_F32_vec3_value.init(
      db,
      R"( UPDATE ndf_F32_vec3 SET value_x=?, value_y=?, value_z=? WHERE id=?; )");
  stmt_set_F32_vec4_value.init(
      db,
      R"( UPDATE ndf_F32_vec4 SET value_x=?, value_y=?, value_z=?, value_w=? WHERE id=?; )");
  stmt_set_S32_vec2_value.init(
      db, R"( UPDATE ndf_S32_vec2 SET value_x=?, value_y=? WHERE id=?; )");
  stmt_set_S32_vec3_value.init(
      db,
      R"( UPDATE ndf_S32_vec3 SET value_x=?, value_y=?, value_z=? WHERE id=?; )");
  stmt_set_S32_vec4_value.init(
      db,
      R"( UPDATE ndf_S32_vec4 SET value_x=?, value_y=?, value_z=?, value_w=? WHERE id=?; )");
  stmt_set_color_value.init(
      db,
      R"( UPDATE ndf_color SET value_r=?, value_g=?, value_b=?, value_a=? WHERE id=?; )");
  stmt_set_object_reference_value.init(
      db,
      R"( UPDATE ndf_object_reference SET referenced_object=?,optional_value=? WHERE id=?; )");
  stmt_set_import_reference_value.init(
      db,
      R"( UPDATE ndf_import_reference SET referenced_object=?,optional_value=? WHERE id=?; )");
  // object updates
  stmt_set_object_name.init(
      db, R"( UPDATE ndf_object SET object_name=? WHERE id=?; )");
  stmt_set_object_export_path.init(
      db, R"( UPDATE ndf_object SET export_path=? WHERE id=?; )");

  stmt_update_object_references.init(db,
                                     R"( UPDATE ndf_object_reference
                                             SET referenced_object=ndf_object.id
                                             FROM ndf_object
                                             WHERE referenced_object IS NULL
                                             AND optional_value=ndf_object.object_name
                                             AND ndf_object.ndf_id=?; )");
  stmt_update_import_references.init(db,
                                     R"( UPDATE ndf_import_reference
                                             SET referenced_object=ndf_object.id
                                             FROM ndf_object
                                             WHERE referenced_object IS NULL
                                             AND optional_value=ndf_object.export_path
                                             AND (ndf_object.ndf_id) IN
                                             (SELECT id FROM ndf_file WHERE is_current=True); )");

  // delete file
  stmt_delete_ndf_file.init(db, R"( DELETE FROM ndf_file WHERE id=?; )");

  return true;
}

bool NDF_DB::init() {
  int rc = sqlite3_open(":memory:", &db);
  if (rc != SQLITE_OK) {
    spdlog::error("Could not open SQLite DB: {}", sqlite3_errmsg(db));
    return false;
  }
  return init_statements();
}

bool NDF_DB::init(fs::path path) {
  std::string path_str = path.string();
  int rc = sqlite3_open(path_str.c_str(), &db);

  if (rc != SQLITE_OK) {
    spdlog::error("Could not open SQLite DB: {}", sqlite3_errmsg(db));
    return false;
  }
  spdlog::info("opened SQLite DB: {}", path.string());
  return init_statements();
}

NDF_DB::~NDF_DB() {
  if (db) {
    sqlite3_close(db);
  }
}

bool NDF_DB::create_table(std::string name, std::string query) {
  SQLStatement<0, 0> stmt_create_ndf_file_tbl;
  stmt_create_ndf_file_tbl.init(db, query.c_str());
  if (!stmt_create_ndf_file_tbl.execute()) {
    return false;
  }
  return true;
}

std::optional<int> NDF_DB::insert_file(std::string vfs_path,
                                       std::string dat_path,
                                       std::string fs_path, std::string version,
                                       bool is_current) {
  return stmt_insert_ndf_file.insert(vfs_path, dat_path, fs_path, version,
                                     is_current);
}

bool NDF_DB::delete_file(int ndf_id) {
  return stmt_delete_ndf_file.execute(ndf_id);
}

std::optional<int> NDF_DB::insert_object(int ndf_idx, const NDFObject &object) {
  auto object_id =
      stmt_insert_ndf_object.insert(ndf_idx, object.name, object.class_name,
                                    object.export_path, object.is_top_object);
  if (!object_id.has_value()) {
    return std::nullopt;
  }

  for (auto &prop : object.properties) {
    insert_property(*prop, *object_id);
  }
  return object_id;
}

bool NDF_DB::insert_objects(int ndf_idx,
                            const std::vector<NDFObject> &objects) {
  {
    SQLTransaction trans(db);
    for (auto &obj : objects) {
      if (!insert_object(ndf_idx, obj)) {
        trans.rollback();
        return false;
      }
    }
  }
  return true;
}

bool NDF_DB::insert_property(const NDFProperty &property, int object_id,
                             int parent, int position) {
  return false;
}

std::optional<int> NDF_DB::get_file(std::string vfs_path, std::string fs_path) {
  return stmt_get_file_from_paths.query_single<int>(vfs_path, fs_path);
}

std::optional<NDFObject> NDF_DB::get_object(int object_idx) {
  auto obj_data_opt = stmt_get_object.query_single<
      std::tuple<int, std::string, std::string, std::string, bool>>(object_idx);
  if (!obj_data_opt) {
    return std::nullopt;
  }
  NDFObject ret;
  auto [_, object_name, class_name, export_path, is_top_object] =
      obj_data_opt.value();
  ret.name = object_name;
  ret.class_name = class_name;
  ret.export_path = export_path;
  ret.is_top_object = is_top_object;

  // get list of all properties in this object (only direct properties, not list
  // items etc.)
  auto prop_ids_opt = stmt_get_object_properties.query<int>(object_idx);
  if (!prop_ids_opt) {
    return std::nullopt;
  }
  for (auto prop_id : prop_ids_opt.value()) {
    auto prop_opt = get_property(prop_id);
    if (prop_opt) {
      ret.properties.push_back(std::move(prop_opt.value()));
    } else {
      return std::nullopt;
    }
  }
  return ret;
}

std::optional<std::unique_ptr<NDFProperty>>
NDF_DB::get_property(int property_id) {
  // get property type
  auto property = NDFProperty::get_db_property_type(this, property_id);
  if (!property) {
    return std::nullopt;
  }
  // load value
  if (!property->from_ndf_db(this, property_id)) {
    return std::nullopt;
  }
  return property;
}

bool NDF_DB::change_object_name(int object_id, std::string new_name) {
  // update to the new name
  if (!stmt_set_object_name.execute(new_name, object_id)) {
    return false;
  }
  return true;
}

bool NDF_DB::change_export_path(int object_id, std::string new_path) {
  // update to the new export_path
  if (!stmt_set_object_export_path.execute(new_path, object_id)) {
    return false;
  }
  return true;
}

bool NDF_DB::fix_references(int ndf_id) {
  // update all object references
  if (!stmt_update_object_references.execute(ndf_id)) {
    return false;
  }
  // update all import references
  if (!stmt_update_import_references.execute()) {
    return false;
  }
  return true;
}

std::optional<int> NDF_DB::insert_only_object(int ndf_idx,
                                              const NDFObject &object) {
  auto object_id =
      stmt_insert_ndf_object.insert(ndf_idx, object.name, object.class_name,
                                    object.export_path, object.is_top_object);
  if (!object_id.has_value()) {
    return std::nullopt;
  }
  return object_id.value();
}

std::optional<int> NDF_DB::insert_only_property(const NDFProperty &property) {
  auto property_id = property.add_db_property(this);
  if (!property_id.has_value()) {
    return false;
  }
  return property_id.value();
}

std::optional<std::vector<NDFObject>> NDF_DB::get_only_objects(int ndf_id) {
  auto objects_opt = stmt_get_object_full_ndf_id.query<
      std::tuple<size_t, std::string, std::string, std::string, bool>>(ndf_id);
  if (!objects_opt) {
    spdlog::error("did not find objects {}", ndf_id);
    return std::nullopt;
  }
  std::vector<NDFObject> ret;
  for (auto object_tup : objects_opt.value()) {
    auto &[db_id, object_name, class_name, export_path, is_top_object] =
        object_tup;
    NDFObject obj;
    obj.name = object_name;
    obj.class_name = class_name;
    obj.export_path = export_path;
    obj.is_top_object = is_top_object;
    obj.db_id = db_id;
    spdlog::debug("loading object {} {}", db_id, obj.name);
    ret.push_back(std::move(obj));
  }
  return ret;
}

std::optional<std::vector<std::unique_ptr<NDFProperty>>>
NDF_DB::get_only_properties(int object_idx) {
  auto prop_ids_opt = stmt_get_object_properties.query<int>(object_idx);
  if (!prop_ids_opt.has_value()) {
    spdlog::error("did not find properties {}", object_idx);
    return std::nullopt;
  }
  std::vector<std::unique_ptr<NDFProperty>> ret;
  for (auto prop_id : prop_ids_opt.value()) {
    auto prop_opt = get_property(prop_id);
    if (prop_opt.has_value()) {
      spdlog::debug("loading property {} {}", object_idx, prop_id);
      ret.push_back(std::move(prop_opt.value()));
    } else {
      spdlog::error("could not load property {} {}", object_idx, prop_id);
      return std::nullopt;
    }
  }
  return ret;
}
