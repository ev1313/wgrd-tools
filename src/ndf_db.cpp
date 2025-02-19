#include "ndf_db.hpp"
#include "sqlite_helpers.hpp"

#include <cstddef>
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
constexpr auto sql_copy_value =
    "INSERT INTO ndf_{0} (value) SELECT value FROM ndf_{0} WHERE id=?;";
// modified trigger for all non list/map/pair properties
constexpr auto sql_trigger_property =
    "CREATE TRIGGER IF NOT EXISTS ndf_property_update_{0} AFTER UPDATE ON "
    "ndf_{0} BEGIN "
    "UPDATE ndf_property SET modifications=modifications+1 WHERE value=old.id; "
    "END;";

#define ndf_property_simple(NAME, DATATYPE)                                    \
  create_table(#NAME, std::format(sql_create_table_value, #NAME, #DATATYPE));  \
  create_table(#NAME, std::format(sql_trigger_property, #NAME));               \
  stmt_insert_ndf_##NAME.init(db, std::format(sql_insert_value, #NAME));       \
  stmt_get_##NAME##_value.init(db, std::format(sql_get_value, #NAME));         \
  stmt_set_##NAME##_value.init(db, std::format(sql_set_value, #NAME));         \
  stmt_get_distinct_##NAME##_value.init(                                       \
      db, std::format(sql_get_distinct_value, #NAME));                         \
  stmt_copy_##NAME##_value.init(db, std::format(sql_copy_value, #NAME));

constexpr auto sql_create_table_vec2_value =
    "CREATE TABLE IF NOT EXISTS ndf_{0} (id INTEGER PRIMARY KEY AUTOINCREMENT, "
    "value_x {1}, value_y {1});";
constexpr auto sql_insert_vec2_value =
    "INSERT INTO ndf_{0} (value_x, value_y) VALUES (?,?);";
constexpr auto sql_get_vec2_value =
    "SELECT value_x, value_y FROM ndf_{0} WHERE id=?;";
constexpr auto sql_set_vec2_value =
    "UPDATE ndf_{0} SET value_x=?, value_y=? WHERE id=?";
constexpr auto sql_get_distinct_vec2_value =
    "SELECT DISTINCT ndf_{0}.value_x, ndf_{0}.value_y FROM ndf_{0} INNER JOIN "
    "ndf_property ON "
    "ndf_property.value=ndf_{0}.id WHERE ndf_property.object_id=? ORDER BY "
    "ndf_{0}.value_x;";
constexpr auto sql_copy_vec2_value =
    "INSERT INTO ndf_{0} (value_x, value_y) SELECT value_x, value_y "
    "FROM ndf_{0} WHERE id=?;";

#define ndf_property_vec2(NAME, DATATYPE)                                      \
  create_table(#NAME,                                                          \
               std::format(sql_create_table_vec2_value, #NAME, #DATATYPE));    \
  create_table(#NAME, std::format(sql_trigger_property, #NAME));               \
  stmt_insert_ndf_##NAME.init(db, std::format(sql_insert_vec2_value, #NAME));  \
  stmt_get_##NAME##_value.init(db, std::format(sql_get_vec2_value, #NAME));    \
  stmt_set_##NAME##_value.init(db, std::format(sql_set_vec2_value, #NAME));    \
  stmt_get_distinct_##NAME##_value.init(                                       \
      db, std::format(sql_get_distinct_vec2_value, #NAME));                    \
  stmt_copy_##NAME##_value.init(db, std::format(sql_copy_vec2_value, #NAME));

constexpr auto sql_create_table_vec3_value =
    "CREATE TABLE IF NOT EXISTS ndf_{0} (id INTEGER PRIMARY KEY AUTOINCREMENT, "
    "value_x {1}, value_y {1}, value_z {1});";
constexpr auto sql_insert_vec3_value =
    "INSERT INTO ndf_{0} (value_x, value_y, value_z) VALUES (?,?,?);";
constexpr auto sql_get_vec3_value =
    "SELECT value_x, value_y, value_z FROM ndf_{0} WHERE id=?;";
constexpr auto sql_set_vec3_value =
    "UPDATE ndf_{0} SET value_x=?, value_y=?, value_z=? WHERE id=?";
constexpr auto sql_get_distinct_vec3_value =
    "SELECT DISTINCT ndf_{0}.value_x, ndf_{0}.value_y, ndf_{0}.value_z FROM "
    "ndf_{0} INNER JOIN "
    "ndf_property ON "
    "ndf_property.value=ndf_{0}.id WHERE ndf_property.object_id=? ORDER BY "
    "ndf_{0}.value_x;";
constexpr auto sql_copy_vec3_value =
    "INSERT INTO ndf_{0} (value_x, value_y, value_z) SELECT value_x, "
    "value_y, value_z "
    "FROM ndf_{0} WHERE id=?;";

#define ndf_property_vec3(NAME, DATATYPE)                                      \
  create_table(#NAME,                                                          \
               std::format(sql_create_table_vec3_value, #NAME, #DATATYPE));    \
  create_table(#NAME, std::format(sql_trigger_property, #NAME));               \
  stmt_insert_ndf_##NAME.init(db, std::format(sql_insert_vec3_value, #NAME));  \
  stmt_get_##NAME##_value.init(db, std::format(sql_get_vec3_value, #NAME));    \
  stmt_set_##NAME##_value.init(db, std::format(sql_set_vec3_value, #NAME));    \
  stmt_get_distinct_##NAME##_value.init(                                       \
      db, std::format(sql_get_distinct_vec3_value, #NAME));                    \
  stmt_copy_##NAME##_value.init(db, std::format(sql_copy_vec3_value, #NAME));

constexpr auto sql_create_table_vec4_value =
    "CREATE TABLE IF NOT EXISTS ndf_{0} (id INTEGER PRIMARY KEY AUTOINCREMENT, "
    "value_x {1}, value_y {1}, value_z {1}, value_w {1});";
constexpr auto sql_insert_vec4_value =
    "INSERT INTO ndf_{0} (value_x, value_y, value_z, value_w) VALUES "
    "(?,?,?,?);";
constexpr auto sql_get_vec4_value =
    "SELECT value_x, value_y, value_z, value_w FROM ndf_{0} WHERE id=?;";
constexpr auto sql_set_vec4_value =
    "UPDATE ndf_{0} SET value_x=?, value_y=?, value_z=?, value_w=? WHERE id=?";
constexpr auto sql_get_distinct_vec4_value =
    "SELECT DISTINCT ndf_{0}.value_x, ndf_{0}.value_y, ndf_{0}.value_z, "
    "ndf_{0}.value_w FROM "
    "ndf_{0} INNER JOIN "
    "ndf_property ON "
    "ndf_property.value=ndf_{0}.id WHERE ndf_property.object_id=? ORDER BY "
    "ndf_{0}.value_x;";
constexpr auto sql_copy_vec4_value = "INSERT INTO ndf_{0} (value_x, value_y, "
                                     "value_z, value_w) SELECT value_x, "
                                     "value_y, value_z, value_w "
                                     "FROM ndf_{0} WHERE id=?;";

#define ndf_property_vec4(NAME, DATATYPE)                                      \
  create_table(#NAME,                                                          \
               std::format(sql_create_table_vec4_value, #NAME, #DATATYPE));    \
  create_table(#NAME, std::format(sql_trigger_property, #NAME));               \
  stmt_insert_ndf_##NAME.init(db, std::format(sql_insert_vec4_value, #NAME));  \
  stmt_get_##NAME##_value.init(db, std::format(sql_get_vec4_value, #NAME));    \
  stmt_set_##NAME##_value.init(db, std::format(sql_set_vec4_value, #NAME));    \
  stmt_get_distinct_##NAME##_value.init(                                       \
      db, std::format(sql_get_distinct_vec4_value, #NAME));                    \
  stmt_copy_##NAME##_value.init(db, std::format(sql_copy_vec4_value, #NAME));

constexpr auto sql_create_table_color_value =
    "CREATE TABLE IF NOT EXISTS ndf_{0} (id INTEGER PRIMARY KEY AUTOINCREMENT, "
    "value_r {1}, value_g {1}, value_b {1}, value_a {1});";
constexpr auto sql_insert_color_value =
    "INSERT INTO ndf_{0} (value_r, value_g, value_b, value_a) VALUES "
    "(?,?,?,?);";
constexpr auto sql_get_color_value =
    "SELECT value_r, value_g, value_b, value_a FROM ndf_{0} WHERE id=?;";
constexpr auto sql_set_color_value =
    "UPDATE ndf_{0} SET value_r=?, value_g=?, value_b=?, value_a=? WHERE id=?";
constexpr auto sql_get_distinct_color_value =
    "SELECT DISTINCT ndf_{0}.value_r, ndf_{0}.value_g, ndf_{0}.value_b, "
    "ndf_{0}.value_a FROM "
    "ndf_{0} INNER JOIN "
    "ndf_property ON "
    "ndf_property.value=ndf_{0}.id WHERE ndf_property.object_id=? ORDER BY "
    "ndf_{0}.value_r;";
constexpr auto sql_copy_color_value = "INSERT INTO ndf_{0} (value_r, value_g, "
                                      "value_b, value_a) SELECT value_r, "
                                      "value_g, value_b, value_a "
                                      "FROM ndf_{0} WHERE id=?;";

#define ndf_property_color(NAME, DATATYPE)                                     \
  create_table(#NAME,                                                          \
               std::format(sql_create_table_color_value, #NAME, #DATATYPE));   \
  create_table(#NAME, std::format(sql_trigger_property, #NAME));               \
  stmt_insert_ndf_##NAME.init(db, std::format(sql_insert_color_value, #NAME)); \
  stmt_get_##NAME##_value.init(db, std::format(sql_get_color_value, #NAME));   \
  stmt_set_##NAME##_value.init(db, std::format(sql_set_color_value, #NAME));   \
  stmt_get_distinct_##NAME##_value.init(                                       \
      db, std::format(sql_get_distinct_color_value, #NAME));                   \
  stmt_copy_##NAME##_value.init(db, std::format(sql_copy_color_value, #NAME));

constexpr auto sql_create_table_reference_value =
    "CREATE TABLE IF NOT EXISTS ndf_{0} ("
    "id INTEGER PRIMARY KEY AUTOINCREMENT, "
    "referenced_object INTEGER REFERENCES ndf_object(id) ON UPDATE CASCADE ON "
    "DELETE SET NULL,"
    "optional_value TEXT);";

constexpr auto sql_get_reference_value =
    "SELECT referenced_object, "
    "optional_value FROM ndf_{0} WHERE id=?;";

constexpr auto sql_insert_reference_value =
    "INSERT INTO ndf_{0} (referenced_object, optional_value) VALUES (?,?);";

constexpr auto sql_set_reference_value =
    "UPDATE ndf_{0} SET referenced_object=?, optional_value=? WHERE id=?;";

constexpr auto sql_get_distinct_reference_value =
    "SELECT DISTINCT referenced_object, "
    "optional_value FROM ndf_{0} WHERE id=?;";

constexpr auto sql_copy_reference_value =
    "INSERT INTO ndf_{0} (referenced_object, optional_value) SELECT "
    "referenced_object, optional_value FROM ndf_{0} WHERE id=?;";

constexpr auto sql_update_references =
    "UPDATE ndf_{0} "
    "SET referenced_object=ndf_object.id "
    "FROM ndf_object "
    "WHERE referenced_object IS NULL "
    "AND optional_value=ndf_object.object_name "
    "AND ndf_object.ndf_id=?;";
constexpr auto sql_get_referencing =
    "SELECT DISTINCT prop.object_id FROM ndf_{0} AS ref INNER JOIN "
    "ndf_property AS prop ON prop.value=ref.id WHERE ref.referenced_object=?;";

#define ndf_property_reference(NAME)                                           \
  create_table(#NAME, std::format(sql_create_table_reference_value, #NAME));   \
  stmt_insert_ndf_##NAME.init(db,                                              \
                              std::format(sql_insert_reference_value, #NAME)); \
  stmt_get_##NAME##_value.init(db,                                             \
                               std::format(sql_get_reference_value, #NAME));   \
  stmt_set_##NAME##_value.init(db,                                             \
                               std::format(sql_set_reference_value, #NAME));   \
  stmt_get_distinct_##NAME##_value.init(                                       \
      db, std::format(sql_get_distinct_reference_value, #NAME));               \
  stmt_copy_##NAME##_value.init(db,                                            \
                                std::format(sql_copy_reference_value, #NAME)); \
  stmt_update_##NAME##_value.init(db,                                          \
                                  std::format(sql_update_references, #NAME));  \
  stmt_get_referencing_##NAME##_value.init(                                    \
      db, std::format(sql_get_referencing, #NAME));

bool NDF_DB::init_statements() {
  // sqlite3_exec(db, "PRAGMA synchronous = FULL", NULL, NULL, NULL);
  // sqlite3_exec(db, "PRAGMA journal_mode = WAL", NULL, NULL, NULL);
  sqlite3_exec(db, "PRAGMA optimize = 0x10002", NULL, NULL, NULL);
  sqlite3_exec(db, "PRAGMA cache_size = -10240", NULL, NULL, NULL);
  sqlite3_exec(db, "PRAGMA page_size = 32768", NULL, NULL, NULL);
  // NDF File
  create_table("ndf_file",
               R"( CREATE TABLE IF NOT EXISTS ndf_file(
                                            id INTEGER PRIMARY KEY AUTOINCREMENT,
                                            vfs_path TEXT,
                                            dat_path TEXT,
                                            fs_path TEXT,
                                            game_version TEXT,
                                            modifications INTEGER DEFAULT 1,
                                            is_current BOOLEAN
                                            ); )");
  stmt_insert_ndf_file.init(
      db,
      R"( INSERT INTO ndf_file (vfs_path, dat_path, fs_path, game_version, is_current) VALUES (?,?,?,?,?); )");
  stmt_delete_ndf_file.init(db, R"( DELETE FROM ndf_file WHERE id=?; )");
  // NDF Object
  create_table("ndf_object",
               R"( CREATE TABLE IF NOT EXISTS ndf_object(
                                          id INTEGER PRIMARY KEY AUTOINCREMENT,
                                          ndf_id INTEGER NOT NULL REFERENCES ndf_file(id) ON UPDATE CASCADE ON DELETE CASCADE,
                                          object_name TEXT,
                                          class_name TEXT,
                                          export_path TEXT,
                                          is_top_object BOOLEAN,
                                          modifications INTEGER DEFAULT 1,
                                          UNIQUE (ndf_id, object_name) ON CONFLICT FAIL
                                          ); )");
  stmt_insert_ndf_object.init(
      db,
      R"( INSERT INTO ndf_object (ndf_id, object_name, class_name, export_path, is_top_object) VALUES (?,?,?,?,?); )");
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
  stmt_get_object_ids_and_names_filtered.init(
      db,
      R"( SELECT id, object_name FROM ndf_object WHERE ndf_id=? AND object_name LIKE CONCAT('%', ?, '%') AND class_name LIKE CONCAT('%', ?, '%'); )");
  stmt_get_object_class_names.init(
      db, R"( SELECT DISTINCT class_name FROM ndf_object WHERE ndf_id=?; )");
  stmt_get_object_export_path.init(
      db, R"( SELECT export_path FROM ndf_object WHERE id=?; )");
  stmt_get_object_top_object.init(
      db, R"( SELECT is_top_object FROM ndf_object WHERE id=?; )");
  stmt_get_object.init(
      db,
      R"( SELECT ndf_id, object_name, class_name, export_path, is_top_object FROM ndf_object WHERE id=?; )");
  stmt_set_object_ndf_id.init(
      db, R"( UPDATE ndf_object SET ndf_id=? WHERE id=?; )");
  stmt_set_object_name.init(
      db, R"( UPDATE ndf_object SET object_name=? WHERE id=?; )");
  stmt_set_object_export_path.init(
      db, R"( UPDATE ndf_object SET export_path=? WHERE id=?; )");
  stmt_set_object_top_object.init(
      db, R"( UPDATE ndf_object SET is_top_object=? WHERE id=?; )");
  stmt_delete_ndf_object.init(db, R"( DELETE FROM ndf_object WHERE id=?; )");
  // NDF Property
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
                                            value INTEGER,
                                            modifications INTEGER DEFAULT 1
                                            ); )");
  stmt_insert_ndf_property.init(
      db,
      R"( INSERT INTO ndf_property (object_id, property_name, property_index, parent, position, type, is_import_reference, value) VALUES (?,?,?,?,?,?,?,?); )");
  stmt_get_object_properties.init(
      db,
      R"( SELECT id FROM ndf_property WHERE object_id=? AND parent IS NULL; )");
  stmt_get_property_names.init(
      db,
      R"( SELECT property_name FROM ndf_property WHERE object_id=? AND property_index<>-1; )");

  stmt_get_property.init(
      db,
      R"( SELECT object_id, property_name, property_index, parent, position, type, is_import_reference, value FROM ndf_property WHERE id=?; )");

  // used by list, map and pair
  stmt_get_list_items.init(
      db, R"( SELECT id FROM ndf_property WHERE parent=? ORDER BY position; )");
  // class db
  create_table("ndf_class", R"(
      CREATE TABLE IF NOT EXISTS ndf_class(
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        class_name TEXT
      );
  )");

  create_table("ndf_property_trigger",
               "CREATE TRIGGER IF NOT EXISTS ndf_property_trigger AFTER UPDATE "
               "ON ndf_property BEGIN UPDATE ndf_object SET "
               "modifications=modifications+1 WHERE id=new.object_id; END;");

  create_table("ndf_object_trigger",
               "CREATE TRIGGER IF NOT EXISTS ndf_object_trigger AFTER UPDATE "
               "ON ndf_object BEGIN UPDATE ndf_file SET "
               "modifications=modifications+1 WHERE id=new.ndf_id; END;");

  stmt_insert_class.init(db,
                         R"( INSERT INTO ndf_class (class_name) VALUES (?); )");
  create_table("ndf_class_property", R"(
      CREATE TABLE IF NOT EXISTS ndf_class_property(
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        class_id INTEGER REFERENCES ndf_class(id) ON UPDATE CASCADE ON DELETE CASCADE,
        property_name TEXT,
        property_index INTEGER
      );
  )");
  stmt_insert_class_property.init(
      db,
      R"( INSERT INTO ndf_class_property (class_id, property_name, property_index) VALUES (?, ?, ?); )");
  stmt_get_class_properties.init(
      db,
      R"( SELECT property_name, property_index FROM ndf_class_property INNER JOIN ndf_class ON ndf_class.id=ndf_class_property.class_id WHERE class_name=?; )");

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

  ndf_property_vec2(F32_vec2, REAL);
  ndf_property_vec2(S32_vec2, INTEGER);
  ndf_property_vec3(F32_vec3, REAL);
  ndf_property_vec3(S32_vec3, INTEGER);
  ndf_property_vec4(F32_vec4, REAL);
  ndf_property_vec4(S32_vec4, INTEGER);
  ndf_property_color(color, INTEGER);

  ndf_property_reference(object_reference);
  ndf_property_reference(import_reference);

  // check if the stash file exists in the database
  auto stash_ndf_id_opt = get_file(":stash:", ":stash:");
  if (!stash_ndf_id_opt) {
    // file does not exist
    stash_ndf_id_opt =
        insert_file(":stash:", ":stash:", ":stash:", ":stash:", false);
    if (!stash_ndf_id_opt) {
      spdlog::error("Could not insert stash file into database");
      return false;
    }
  }
  stash_ndf_id = stash_ndf_id_opt.value();

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

std::optional<size_t>
NDF_DB::insert_file(std::string vfs_path, std::string dat_path,
                    std::string fs_path, std::string version, bool is_current) {
  return stmt_insert_ndf_file.insert(vfs_path, dat_path, fs_path, version,
                                     is_current);
}

bool NDF_DB::delete_file(size_t ndf_id) {
  return stmt_delete_ndf_file.execute(ndf_id);
}

std::optional<size_t> NDF_DB::insert_object(NDFObject &object) {
  auto object_id = stmt_insert_ndf_object.insert(
      object.db_ndf_id, object.name, object.class_name, object.export_path,
      object.is_top_object);
  if (!object_id.has_value()) {
    return std::nullopt;
  }

  for (auto &prop : object.properties) {
    prop->db_object_id = object_id.value();
    insert_property(*prop);
  }
  return object_id;
}

bool NDF_DB::insert_objects(std::vector<NDFObject> &objects) {
  {
    SQLTransaction trans(db);
    for (auto &obj : objects) {
      if (!insert_object(obj)) {
        trans.rollback();
        return false;
      }
    }
  }
  return true;
}

bool NDF_DB::insert_property(NDFProperty &property) {
  // insert the value
  auto ret = property.to_ndf_db(this);
  if (!ret) {
    spdlog::error("Could not insert property into database");
    return false;
  }
  // lists/maps/pairs create their own property entry
  if (property.is_list() || property.is_map() || property.is_pair()) {
    return true;
  }
  if (!property.db_value_id) {
    spdlog::error("Property value not inserted into database?");
    return false;
  }
  // insert the property
  auto prop_ret = property.add_db_property(this);
  if (!prop_ret) {
    spdlog::error("Could not insert property into database");
    return false;
  }
  return true;
}

std::optional<size_t> NDF_DB::get_file(std::string vfs_path,
                                       std::string fs_path) {
  return stmt_get_file_from_paths.query_single<int>(vfs_path, fs_path);
}

std::optional<NDFObject> NDF_DB::get_object(size_t object_idx) {
  auto obj_data_opt = stmt_get_object.query_single<
      std::tuple<int, std::string, std::string, std::string, bool>>(object_idx);
  if (!obj_data_opt) {
    return std::nullopt;
  }
  NDFObject ret;
  auto [ndf_id, object_name, class_name, export_path, is_top_object] =
      obj_data_opt.value();
  ret.name = object_name;
  ret.class_name = class_name;
  ret.export_path = export_path;
  ret.is_top_object = is_top_object;
  ret.db_id = object_idx;
  ret.db_ndf_id = ndf_id;

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

std::optional<std::vector<std::string>>
NDF_DB::get_object_names(size_t ndf_id) {
  return stmt_get_object_names.query<std::string>(ndf_id);
}

std::optional<std::vector<std::tuple<size_t, std::string>>>
NDF_DB::get_object_ids_and_names_filtered(size_t ndf_id,
                                          std::string object_filter,
                                          std::string class_filter) {
  return stmt_get_object_ids_and_names_filtered
      .query<std::tuple<size_t, std::string>>(ndf_id, object_filter,
                                              class_filter);
}

std::optional<std::unique_ptr<NDFProperty>>
NDF_DB::get_property(size_t property_id) {
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

bool NDF_DB::change_object_name(size_t object_id, std::string new_name) {
  // update to the new name
  if (!stmt_set_object_name.execute(new_name, object_id)) {
    return false;
  }
  return true;
}

bool NDF_DB::change_export_path(size_t object_id, std::string new_path) {
  // update to the new export_path
  if (!stmt_set_object_export_path.execute(new_path, object_id)) {
    return false;
  }
  return true;
}

bool NDF_DB::change_is_top_object(size_t object_id, bool is_top_object) {
  // update to the new export_path
  if (!stmt_set_object_top_object.execute(is_top_object, object_id)) {
    return false;
  }
  return true;
}

bool NDF_DB::fix_references(size_t ndf_id) {
  // update all object references
  if (!stmt_update_object_reference_value.execute(ndf_id)) {
    return false;
  }
  // update all import references
  if (!stmt_update_import_reference_value.execute()) {
    return false;
  }
  return true;
}

std::optional<size_t> NDF_DB::insert_only_object(size_t ndf_idx,
                                                 const NDFObject &object) {
  auto object_id =
      stmt_insert_ndf_object.insert(ndf_idx, object.name, object.class_name,
                                    object.export_path, object.is_top_object);
  if (!object_id.has_value()) {
    return std::nullopt;
  }
  return object_id.value();
}

std::optional<size_t>
NDF_DB::insert_only_property(const NDFProperty &property) {
  auto property_id = property.add_db_property(this);
  if (!property_id.has_value()) {
    return false;
  }
  return property_id.value();
}

std::optional<std::vector<NDFObject>> NDF_DB::get_only_objects(size_t ndf_id) {
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
#ifdef DEBUG
    spdlog::debug("loading object {} {}", db_id, obj.name);
#endif
    ret.push_back(std::move(obj));
  }
  return ret;
}

std::optional<std::vector<std::unique_ptr<NDFProperty>>>
NDF_DB::get_only_properties(size_t object_idx) {
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

std::optional<size_t> NDF_DB::copy_object(size_t obj_id, std::string new_name) {
  auto cpy = get_object(obj_id);
  if (!cpy.has_value()) {
    return std::nullopt;
  }
  cpy.value().name = new_name;
  return insert_object(cpy.value());
}

bool NDF_DB::remove_object(size_t obj_id) {
  return stmt_delete_ndf_object.execute(obj_id);
}

bool NDF_DB::move_object(size_t obj_id, size_t new_ndf_id) {
  if (new_ndf_id == 0) {
    new_ndf_id = stash_ndf_id;
  }
  return stmt_set_object_ndf_id.execute(new_ndf_id, obj_id);
}
