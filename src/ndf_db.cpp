#include "ndf_db.hpp"
#include "sqlite_helpers.hpp"

#include <spdlog/spdlog.h>

bool NDF_DB::init_statements() {
  stmt_create_ndf_file_tbl.init(db,
                                R"rstr( CREATE TABLE ndf_file(
                                                          id INTEGER PRIMARY KEY AUTOINCREMENT,
                                                          vfs_path TEXT,
                                                          dat_path TEXT,
                                                          fs_path TEXT
                                                          ); )rstr");

  stmt_create_ndf_object_tbl.init(db,
                                  R"rstr( CREATE TABLE ndf_object(
                                          id INTEGER PRIMARY KEY AUTOINCREMENT,
                                          FOREIGN_KEY (ndf_id) REFERENCES ndf_file(id) NOT NULL,
                                          object_name TEXT,
                                          class_name TEXT,
                                          export_path TEXT,
                                          is_top_object BOOLEAN
                                          ); )rstr");
  stmt_create_ndf_property_tbl.init(db,
                                    R"rstr( CREATE TABLE ndf_property(
                                            id INTEGER PRIMARY KEY AUTOINCREMENT,
                                            FOREIGN_KEY (object_id) REFERENCES ndf_object(id) NOT NULL,
                                            property_name TEXT,
                                            index INTEGER,
                                            FOREIGN_KEY (parent) REFERENCES ndf_property(id),
                                            position INTEGER,
                                            type INTEGER,
                                            value INTEGER
                                            ); )rstr");

  stmt_create_ndf_bool_tbl.init(db,
                                R"rstr( CREATE TABLE ndf_bool(
                                                          id INTEGER PRIMARY KEY AUTOINCREMENT,
                                                          value BOOLEAN;
                                                          ); )rstr");
  stmt_create_ndf_int8_tbl.init(db,
                                R"rstr( CREATE TABLE ndf_int8(
                                                          id INTEGER PRIMARY KEY AUTOINCREMENT,
                                                          value INTEGER;
                                                          ); )rstr");
  stmt_create_ndf_uint8_tbl.init(db,
                                 R"rstr( CREATE TABLE ndf_uint8(
                                                           id INTEGER PRIMARY KEY AUTOINCREMENT,
                                                           value INTEGER;
                                                           ); )rstr");
  stmt_create_ndf_int16_tbl.init(db,
                                 R"rstr( CREATE TABLE ndf_int16(
                                                           id INTEGER PRIMARY KEY AUTOINCREMENT,
                                                           value INTEGER;
                                                           ); )rstr");
  stmt_create_ndf_uint16_tbl.init(db,
                                  R"rstr( CREATE TABLE ndf_uint16(
                                   id INTEGER PRIMARY KEY AUTOINCREMENT,
                                   value INTEGER;
                                   ); )rstr");
  stmt_create_ndf_int32_tbl.init(db,
                                 R"rstr( CREATE TABLE ndf_int32(
                                                           id INTEGER PRIMARY KEY AUTOINCREMENT,
                                                           value INTEGER;
                                                           ); )rstr");
  stmt_create_ndf_uint32_tbl.init(db,
                                  R"rstr( CREATE TABLE ndf_uint32(
                                          id INTEGER PRIMARY KEY AUTOINCREMENT,
                                          value INTEGER;
                                          ); )rstr");
  stmt_create_ndf_float32_tbl.init(db,
                                   R"rstr( CREATE TABLE ndf_float32(
                                           id INTEGER PRIMARY KEY AUTOINCREMENT,
                                           value REAL;
                                           ); )rstr");
  stmt_create_ndf_float64_tbl.init(db,
                                   R"rstr( CREATE TABLE ndf_float64(
                                           id INTEGER PRIMARY KEY AUTOINCREMENT,
                                           value REAL;
                                           ); )rstr");
  stmt_create_ndf_string_tbl.init(db, R"rstr( CREATE TABLE ndf_string(
                                          id INTEGER PRIMARY KEY AUTOINCREMENT,
                                          value TEXT;
                                          ); )rstr");
  stmt_create_ndf_widestring_tbl.init(db,
                                      R"rstr( CREATE TABLE ndf_widestring(
                                              id INTEGER PRIMARY KEY AUTOINCREMENT,
                                              value TEXT;
                                              ); )rstr");
  stmt_create_ndf_F32_vec2_tbl.init(db,
                                    R"rstr( CREATE TABLE ndf_uint32(
                                          id INTEGER PRIMARY KEY AUTOINCREMENT,
                                          value_x REAL;
                                          value_y REAL;
                                          ); )rstr");
  stmt_create_ndf_F32_vec3_tbl.init(db,
                                    R"rstr( CREATE TABLE ndf_uint32(
                                          id INTEGER PRIMARY KEY AUTOINCREMENT,
                                          value_x REAL;
                                          value_y REAL;
                                          value_z REAL;
                                          ); )rstr");
  stmt_create_ndf_F32_vec4_tbl.init(db,
                                    R"rstr( CREATE TABLE ndf_uint32(
                                          id INTEGER PRIMARY KEY AUTOINCREMENT,
                                          value_x REAL;
                                          value_y REAL;
                                          value_z REAL;
                                          value_w REAL;
                                          ); )rstr");
  stmt_create_ndf_S32_vec2_tbl.init(db,
                                    R"rstr( CREATE TABLE ndf_uint32(
                                          id INTEGER PRIMARY KEY AUTOINCREMENT,
                                          value_x INTEGER;
                                          value_y INTEGER;
                                          ); )rstr");
  stmt_create_ndf_S32_vec3_tbl.init(db,
                                    R"rstr( CREATE TABLE ndf_uint32(
                                          id INTEGER PRIMARY KEY AUTOINCREMENT,
                                          value_x INTEGER;
                                          value_y INTEGER;
                                          value_z INTEGER;
                                          ); )rstr");
  stmt_create_ndf_S32_vec4_tbl.init(db,
                                    R"rstr( CREATE TABLE ndf_uint32(
                                          id INTEGER PRIMARY KEY AUTOINCREMENT,
                                          value_x INTEGER;
                                          value_y INTEGER;
                                          value_z INTEGER;
                                          value_w INTEGER;
                                          ); )rstr");
  stmt_create_ndf_color_tbl.init(db,
                                 R"rstr( CREATE TABLE ndf_color(
                                                           id INTEGER PRIMARY KEY AUTOINCREMENT,
                                                           value_r INTEGER;
                                                           value_g INTEGER;
                                                           value_b INTEGER;
                                                           value_a INTEGER;
                                                           ); )rstr");
  stmt_create_ndf_object_reference_tbl.init(
      db,
      R"rstr( CREATE TABLE ndf_object_reference(
                                         id INTEGER PRIMARY KEY AUTOINCREMENT,
                                         value TEXT;
                                         ); )rstr");
  stmt_create_ndf_import_reference_tbl.init(
      db,
      R"rstr( CREATE TABLE ndf_import_reference(
                                                    id INTEGER PRIMARY KEY AUTOINCREMENT,
                                                    value TEXT;
                                                    ); )rstr");
  stmt_create_ndf_GUID_tbl.init(db,
                                R"rstr( CREATE TABLE ndf_GUID(
                                                          id INTEGER PRIMARY KEY AUTOINCREMENT,
                                                          value TEXT;
                                                          ); )rstr");
  stmt_create_ndf_path_reference_tbl.init(
      db,
      R"rstr( CREATE TABLE ndf_path_reference(
                                                  id INTEGER PRIMARY KEY AUTOINCREMENT,
                                                  value TEXT;
                                                  ); )rstr");
  stmt_create_ndf_localisation_hash_tbl.init(
      db,
      R"rstr( CREATE TABLE ndf_localisation_hash(
                                                     id INTEGER PRIMARY KEY AUTOINCREMENT,
                                                     value TEXT;
                                                     ); )rstr");
  stmt_create_ndf_hash_tbl.init(db,
                                R"rstr( CREATE TABLE ndf_hash(
                                                          id INTEGER PRIMARY KEY AUTOINCREMENT,
                                                          value TEXT;
                                                          ); )rstr");
  // inserters
  stmt_insert_ndf_file.init(
      db,
      R"rstr( INSERT INTO ndf_file (vfs_path, dat_path, fs_path) VALUES (?,?,?); )rstr");
  stmt_insert_ndf_object.init(
      db,
      R"rstr( INSERT INTO ndf_object (ndf_id, object_name, class_name, export_path, is_top_object) VALUES (?,?,?,?,?); )rstr");
  stmt_insert_ndf_property.init(
      db,
      R"rstr( INSERT INTO ndf_property (object_id, property_name, index, parent, position, type, value) VALUES (?,?,?,?,?,?,?); )rstr");
  stmt_insert_ndf_bool.init(
      db, R"rstr( INSERT INTO ndf_bool (value) VALUES (?); )rstr");
  stmt_insert_ndf_int8.init(
      db, R"rstr( INSERT INTO ndf_int8 (value) VALUES (?); )rstr");
  stmt_insert_ndf_uint8.init(
      db, R"rstr( INSERT INTO ndf_uint8 (value) VALUES (?); )rstr");
  stmt_insert_ndf_int16.init(
      db, R"rstr( INSERT INTO ndf_int16 (value) VALUES (?); )rstr");
  stmt_insert_ndf_uint16.init(
      db, R"rstr( INSERT INTO ndf_uint16 (value) VALUES (?); )rstr");
  stmt_insert_ndf_int32.init(
      db, R"rstr( INSERT INTO ndf_int32 (value) VALUES (?); )rstr");
  stmt_insert_ndf_uint32.init(
      db, R"rstr( INSERT INTO ndf_uint32 (value) VALUES (?); )rstr");
  stmt_insert_ndf_float32.init(
      db, R"rstr( INSERT INTO ndf_float32 (value) VALUES (?); )rstr");
  stmt_insert_ndf_float64.init(
      db, R"rstr( INSERT INTO ndf_float64 (value) VALUES (?); )rstr");
  stmt_insert_ndf_string.init(
      db, R"rstr( INSERT INTO ndf_string (value) VALUES (?); )rstr");
  stmt_insert_ndf_widestring.init(
      db, R"rstr( INSERT INTO ndf_widestring (value) VALUES (?); )rstr");
  stmt_insert_ndf_F32_vec2.init(
      db,
      R"rstr( INSERT INTO ndf_float32_vec2 (value_x, value_y) VALUES (?,?); )rstr");
  stmt_insert_ndf_F32_vec3.init(
      db,
      R"rstr( INSERT INTO ndf_float32_vec3 (value_x, value_y, value_z) VALUES (?,?,?); )rstr");
  stmt_insert_ndf_F32_vec4.init(
      db,
      R"rstr( INSERT INTO ndf_float32_vec4 (value_x, value_y, value_z, value_w) VALUES (?,?,?,?); )rstr");
  stmt_insert_ndf_S32_vec2.init(
      db,
      R"rstr( INSERT INTO ndf_int32_vec2 (value_x, value_y) VALUES (?,?); )rstr");
  stmt_insert_ndf_S32_vec3.init(
      db,
      R"rstr( INSERT INTO ndf_int32_vec3 (value_x, value_y, value_z) VALUES (?,?,?); )rstr");
  stmt_insert_ndf_S32_vec4.init(
      db,
      R"rstr( INSERT INTO ndf_int32_vec4 (value_x, value_y, value_z, value_w) VALUES (?,?,?,?); )rstr");
  stmt_insert_ndf_color.init(
      db,
      R"rstr( INSERT INTO ndf_color (value_r, value_g, value_b, value_a) VALUES (?,?,?,?); )rstr");
  stmt_insert_ndf_object_reference.init(
      db,
      R"rstr( INSERT INTO ndf_object_reference (reference_id) VALUES (?); )rstr");
  stmt_insert_ndf_import_reference.init(
      db,
      R"rstr( INSERT INTO ndf_import_reference (reference_id) VALUES (?); )rstr");
  stmt_insert_ndf_GUID.init(
      db, R"rstr( INSERT INTO ndf_GUID (value) VALUES (?); )rstr");
  stmt_insert_ndf_localisation_hash.init(
      db, R"rstr( INSERT INTO ndf_localisation_hash (value) VALUES (?); )rstr");
  stmt_insert_ndf_hash.init(
      db, R"rstr( INSERT INTO ndf_hash (value) VALUES (?); )rstr");

  // accessors
  stmt_get_object_names.init(
      db, R"rstr( SELECT object_name FROM ndf_object WHERE ndf_id=?; )rstr");
  stmt_get_property_names.init(
      db,
      R"rstr( SELECT property_name FROM ndf_property WHERE object_id=? AND index<>-1; )rstr");
  stmt_get_bool_value.init(
      db, R"rstr( SELECT value FROM ndf_bool WHERE id=?; )rstr");
  stmt_get_int8_value.init(
      db, R"rstr( SELECT value FROM ndf_int8 WHERE id=?; )rstr");
  stmt_get_uint8_value.init(
      db, R"rstr( SELECT value FROM ndf_uint8 WHERE id=?; )rstr");
  stmt_get_int16_value.init(
      db, R"rstr( SELECT value FROM ndf_int16 WHERE id=?; )rstr");
  stmt_get_uint16_value.init(
      db, R"rstr( SELECT value FROM ndf_uint16 WHERE id=?; )rstr");
  stmt_get_int32_value.init(
      db, R"rstr( SELECT value FROM ndf_int32 WHERE id=?; )rstr");
  stmt_get_uint32_value.init(
      db, R"rstr( SELECT value FROM ndf_uint32 WHERE id=?; )rstr");
  stmt_get_float32_value.init(
      db, R"rstr( SELECT value FROM ndf_float32 WHERE id=?; )rstr");
  stmt_get_float64_value.init(
      db, R"rstr( SELECT value FROM ndf_float64 WHERE id=?; )rstr");
  stmt_get_string_value.init(
      db, R"rstr( SELECT value FROM ndf_string WHERE id=?; )rstr");
  stmt_get_widestring_value.init(
      db, R"rstr( SELECT value FROM ndf_widestring WHERE id=?; )rstr");
  stmt_get_F32_vec2_value.init(
      db, R"rstr( SELECT value_x, value_y FROM ndf_F32_vec2 WHERE id=?; )rstr");
  stmt_get_F32_vec3_value.init(
      db,
      R"rstr( SELECT value_x, value_y, value_z FROM ndf_F32_vec3 WHERE id=?; )rstr");
  stmt_get_F32_vec4_value.init(
      db,
      R"rstr( SELECT value_x, value_y, value_z, value_w FROM ndf_F32_vec4 WHERE id=?; )rstr");
  stmt_get_S32_vec2_value.init(
      db, R"rstr( SELECT value_x, value_y FROM ndf_S32_vec2 WHERE id=?; )rstr");
  stmt_get_S32_vec3_value.init(
      db,
      R"rstr( SELECT value_x, value_y, value_z FROM ndf_S32_vec3 WHERE id=?; )rstr");
  stmt_get_S32_vec4_value.init(
      db,
      R"rstr( SELECT value_x, value_y, value_z, value_w FROM ndf_S32_vec4 WHERE id=?; )rstr");
  stmt_get_color_value.init(
      db,
      R"rstr( SELECT value_r, value_g, value_b, value_a FROM ndf_color WHERE id=?; )rstr");
  stmt_get_object_reference_value.init(
      db, R"rstr( SELECT value FROM ndf_object_reference WHERE id=?; )rstr");
  stmt_get_import_reference_value.init(
      db, R"rstr( SELECT value FROM ndf_import_reference WHERE id=?; )rstr");
  stmt_get_GUID_value.init(
      db, R"rstr( SELECT value FROM ndf_GUID WHERE id=?; )rstr");
  stmt_get_path_reference_value.init(
      db, R"rstr( SELECT value FROM ndf_path_reference WHERE id=?; )rstr");
  stmt_get_localisation_hash_value.init(
      db, R"rstr( SELECT value FROM ndf_localisation_hash WHERE id=?; )rstr");
  stmt_get_hash_value.init(
      db, R"rstr( SELECT value FROM ndf_hash WHERE id=?; )rstr");
  // used by list, map and pair
  stmt_get_list_items.init(
      db,
      R"rstr( SELECT id FROM ndf_property WHERE parent=? ORDER BY position; )rstr");

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
  int rc = sqlite3_open(path.c_str(), &db);

  if (rc != SQLITE_OK) {
    spdlog::error("Could not open SQLite DB: {}", sqlite3_errmsg(db));
    return false;
  }
  return init_statements();
}

NDF_DB::~NDF_DB() {
  if (db) {
    sqlite3_close(db);
  }
}
}
