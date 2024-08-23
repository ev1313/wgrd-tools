#pragma once

class NDF_DB {
private:
  const char *create_ndf_file_tbl =
      R"rstr( CREATE TABLE ndf_file(
                                    id INTEGER PRIMARY KEY AUTOINCREMENT,
                                    vfs_path TEXT,
                                    dat_path TEXT,
                                    fs_path TEXT
                                    ); )rstr";
  const char *create_ndf_object_tbl =
      R"rstr( CREATE TABLE ndf_object(
                                      id INTEGER PRIMARY KEY AUTOINCREMENT,
                                      FOREIGN_KEY (ndf_id) REFERENCES ndf_file(id) NOT NULL,
                                      object_name TEXT,
                                      class_name TEXT,
                                      export_path TEXT,
                                      is_top_object BOOLEAN
                                      ); )rstr";
  const char *create_ndf_property_tbl =
      R"rstr( CREATE TABLE ndf_property(
                                        id INTEGER PRIMARY KEY AUTOINCREMENT,
                                        FOREIGN_KEY (object_id) REFERENCES ndf_object(id) NOT NULL,
                                        property_name TEXT,
                                        index INTEGER,
                                        FOREIGN_KEY (parent) REFERENCES ndf_property(id),
                                        position INTEGER,
                                        type INTEGER,
                                        value INTEGER
                                        ); )rstr";

  const char *create_ndf_bool_tbl =
      R"rstr( CREATE TABLE ndf_bool(
                                    id INTEGER PRIMARY KEY AUTOINCREMENT,
                                    value BOOLEAN;
                                    ); )rstr";
  const char *create_ndf_int8_tbl =
      R"rstr( CREATE TABLE ndf_int8(
                                    id INTEGER PRIMARY KEY AUTOINCREMENT,
                                    value INTEGER;
                                    ); )rstr";
  const char *create_ndf_uint8_tbl =
      R"rstr( CREATE TABLE ndf_uint8(
                                    id INTEGER PRIMARY KEY AUTOINCREMENT,
                                    value INTEGER;
                                    ); )rstr";
  const char *create_ndf_int16_tbl =
      R"rstr( CREATE TABLE ndf_int16(
                                     id INTEGER PRIMARY KEY AUTOINCREMENT,
                                     value INTEGER;
                                     ); )rstr";
  const char *create_ndf_uint16_tbl =
      R"rstr( CREATE TABLE ndf_uint16(
                                      id INTEGER PRIMARY KEY AUTOINCREMENT,
                                      value INTEGER;
                                      ); )rstr";
  const char *create_ndf_int32_tbl =
      R"rstr( CREATE TABLE ndf_int32(
                                     id INTEGER PRIMARY KEY AUTOINCREMENT,
                                     value INTEGER;
                                     ); )rstr";
  const char *create_ndf_uint32_tbl =
      R"rstr( CREATE TABLE ndf_uint32(
                                      id INTEGER PRIMARY KEY AUTOINCREMENT,
                                      value INTEGER;
                                      ); )rstr";
  const char *create_ndf_float32_tbl =
      R"rstr( CREATE TABLE ndf_float32(
                                       id INTEGER PRIMARY KEY AUTOINCREMENT,
                                       value REAL;
                                       ); )rstr";
  const char *create_ndf_float64_tbl =
      R"rstr( CREATE TABLE ndf_float64(
                                       id INTEGER PRIMARY KEY AUTOINCREMENT,
                                       value REAL;
                                       ); )rstr";
  const char *create_ndf_string_tbl =
      R"rstr( CREATE TABLE ndf_string(
                                      id INTEGER PRIMARY KEY AUTOINCREMENT,
                                      value TEXT;
                                      ); )rstr";
  const char *create_ndf_widestring_tbl =
      R"rstr( CREATE TABLE ndf_widestring(
                                          id INTEGER PRIMARY KEY AUTOINCREMENT,
                                          value TEXT;
                                          ); )rstr";
  const char *create_ndf_F32_vec2_tbl =
      R"rstr( CREATE TABLE ndf_uint32(
                                      id INTEGER PRIMARY KEY AUTOINCREMENT,
                                      value_x REAL;
                                      value_y REAL;
                                      ); )rstr";
  const char *create_ndf_F32_vec3_tbl =
      R"rstr( CREATE TABLE ndf_uint32(
                                      id INTEGER PRIMARY KEY AUTOINCREMENT,
                                      value_x REAL;
                                      value_y REAL;
                                      value_z REAL;
                                      ); )rstr";
  const char *create_ndf_F32_vec4_tbl =
      R"rstr( CREATE TABLE ndf_uint32(
                                      id INTEGER PRIMARY KEY AUTOINCREMENT,
                                      value_x REAL;
                                      value_y REAL;
                                      value_z REAL;
                                      value_w REAL;
                                      ); )rstr";
  const char *create_ndf_S32_vec2_tbl =
      R"rstr( CREATE TABLE ndf_uint32(
                                      id INTEGER PRIMARY KEY AUTOINCREMENT,
                                      value_x INTEGER;
                                      value_y INTEGER;
                                      ); )rstr";
  const char *create_ndf_S32_vec3_tbl =
      R"rstr( CREATE TABLE ndf_uint32(
                                      id INTEGER PRIMARY KEY AUTOINCREMENT,
                                      value_x INTEGER;
                                      value_y INTEGER;
                                      value_z INTEGER;
                                      ); )rstr";
  const char *create_ndf_S32_vec4_tbl =
      R"rstr( CREATE TABLE ndf_uint32(
                                      id INTEGER PRIMARY KEY AUTOINCREMENT,
                                      value_x INTEGER;
                                      value_y INTEGER;
                                      value_z INTEGER;
                                      value_w INTEGER;
                                      ); )rstr";
  const char *create_ndf_color_tbl =
      R"rstr( CREATE TABLE ndf_color(
                                     id INTEGER PRIMARY KEY AUTOINCREMENT,
                                     value_r INTEGER;
                                     value_g INTEGER;
                                     value_b INTEGER;
                                     value_a INTEGER;
                                     ); )rstr";
  const char *create_ndf_object_reference_tbl =
      R"rstr( CREATE TABLE ndf_object_reference(
                                     id INTEGER PRIMARY KEY AUTOINCREMENT,
                                     value TEXT;
                                     ); )rstr";
  const char *create_ndf_import_reference_tbl =
      R"rstr( CREATE TABLE ndf_import_reference(
                                                id INTEGER PRIMARY KEY AUTOINCREMENT,
                                                value TEXT;
                                                ); )rstr";
  const char *create_ndf_GUID_tbl =
      R"rstr( CREATE TABLE ndf_GUID(
                                    id INTEGER PRIMARY KEY AUTOINCREMENT,
                                    value TEXT;
                                    ); )rstr";
  const char *create_ndf_path_reference_tbl =
      R"rstr( CREATE TABLE ndf_path_reference(
                                              id INTEGER PRIMARY KEY AUTOINCREMENT,
                                              value TEXT;
                                              ); )rstr";
  const char *create_ndf_localisation_hash_tbl =
      R"rstr( CREATE TABLE ndf_localisation_hash(
                                                 id INTEGER PRIMARY KEY AUTOINCREMENT,
                                                 value TEXT;
                                                 ); )rstr";
  const char *create_ndf_hash_tbl =
      R"rstr( CREATE TABLE ndf_hash(
                                    id INTEGER PRIMARY KEY AUTOINCREMENT,
                                    value TEXT;
                                    ); )rstr";
  // inserters

  // accessors
  const char *get_object_names =
      R"rstr( SELECT object_name FROM ndf_object WHERE ndf_id=?; )rstr";
  const char *get_property_names =
      R"rstr( SELECT property_name FROM ndf_property WHERE object_id=? AND index<>-1; )rstr";
  const char *get_bool_value =
      R"rstr( SELECT value FROM ndf_bool WHERE id=?; )rstr";
  const char *get_int8_value =
      R"rstr( SELECT value FROM ndf_int8 WHERE id=?; )rstr";
  const char *get_uint8_value =
      R"rstr( SELECT value FROM ndf_uint8 WHERE id=?; )rstr";
  const char *get_int16_value =
      R"rstr( SELECT value FROM ndf_int16 WHERE id=?; )rstr";
  const char *get_uint16_value =
      R"rstr( SELECT value FROM ndf_uint16 WHERE id=?; )rstr";
  const char *get_int32_value =
      R"rstr( SELECT value FROM ndf_int32 WHERE id=?; )rstr";
  const char *get_uint32_value =
      R"rstr( SELECT value FROM ndf_uint32 WHERE id=?; )rstr";
  const char *get_float32_value =
      R"rstr( SELECT value FROM ndf_float32 WHERE id=?; )rstr";
  const char *get_float64_value =
      R"rstr( SELECT value FROM ndf_float64 WHERE id=?; )rstr";
  const char *get_string_value =
      R"rstr( SELECT value FROM ndf_string WHERE id=?; )rstr";
  const char *get_widestring_value =
      R"rstr( SELECT value FROM ndf_widestring WHERE id=?; )rstr";
  const char *get_F32_vec2_value =
      R"rstr( SELECT value_x, value_y FROM ndf_F32_vec2 WHERE id=?; )rstr";
  const char *get_F32_vec3_value =
      R"rstr( SELECT value_x, value_y, value_z FROM ndf_F32_vec3 WHERE id=?; )rstr";
  const char *get_F32_vec4_value =
      R"rstr( SELECT value_x, value_y, value_z, value_w FROM ndf_F32_vec4 WHERE id=?; )rstr";
  const char *get_S32_vec2_value =
      R"rstr( SELECT value_x, value_y FROM ndf_S32_vec2 WHERE id=?; )rstr";
  const char *get_S32_vec3_value =
      R"rstr( SELECT value_x, value_y, value_z FROM ndf_S32_vec3 WHERE id=?; )rstr";
  const char *get_S32_vec4_value =
      R"rstr( SELECT value_x, value_y, value_z, value_w FROM ndf_S32_vec4 WHERE id=?; )rstr";
  const char *get_color_value =
      R"rstr( SELECT value_r, value_g, value_b, value_a FROM ndf_color WHERE id=?; )rstr";
  const char *get_object_reference_value =
      R"rstr( SELECT value FROM ndf_object_reference WHERE id=?; )rstr";
  const char *get_import_reference_value =
      R"rstr( SELECT value FROM ndf_import_reference WHERE id=?; )rstr";
  const char *get_GUID_value =
      R"rstr( SELECT value FROM ndf_GUID WHERE id=?; )rstr";
  const char *get_path_reference_value =
      R"rstr( SELECT value FROM ndf_path_reference WHERE id=?; )rstr";
  const char *get_localisation_hash_value =
      R"rstr( SELECT value FROM ndf_localisation_hash WHERE id=?; )rstr";
  const char *get_hash_value =
      R"rstr( SELECT value FROM ndf_hash WHERE id=?; )rstr";
  // used by list, map and pair
  const char *get_list_items =
      R"rstr( SELECT id FROM ndf_property WHERE parent=? ORDER BY position; )rstr";

public:
};
