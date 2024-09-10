#include <catch2/catch_all.hpp>

#include "catch2/catch_test_macros.hpp"
#include "generator.hpp"
#include "ndf_properties.hpp"
#include "spdlog/spdlog.h"

#include <filesystem>
#include <memory>
#include <pybind11/embed.h>
namespace py = pybind11;

#include <chrono>
#include <numeric>

#include "ndf_db.hpp"

struct PyFixture {
private:
  fs::path directory = fs::temp_directory_path() / "testfiles";

public:
  PyFixture() {
    spdlog::set_level(spdlog::level::debug);
    pybind11::initialize_interpreter();
    ndf_generator::create_test_files(directory);
  }
  ~PyFixture() {
    // FIXME
    // fs::remove_all(directory);
  }
};

bool check_property_equality(NDFProperty *prop1, NDFProperty *prop2) {
  if (prop1->property_name != prop2->property_name) {
    spdlog::info("name {} differs from {}", prop1->property_name,
                 prop2->property_name);
    return false;
  }
  if (prop1->property_idx != prop2->property_idx) {
    spdlog::info("idx {} differs from {}", prop1->property_idx,
                 prop2->property_idx);
    return false;
  }
  if (prop1->property_type != prop2->property_type) {
    spdlog::info("type {} differs from {}", prop1->property_type,
                 prop2->property_type);
    return false;
  }
  if (prop1->property_type == NDFPropertyType::UInt8) {
    NDFPropertyUInt8 *p1 = (NDFPropertyUInt8 *)(prop1);
    NDFPropertyUInt8 *p2 = (NDFPropertyUInt8 *)(prop2);
    if (p1->value != p2->value) {
      return false;
    }
  }
  if (prop1->property_type == NDFPropertyType::UInt16) {
    NDFPropertyUInt16 *p1 = (NDFPropertyUInt16 *)(prop1);
    NDFPropertyUInt16 *p2 = (NDFPropertyUInt16 *)(prop2);
    if (p1->value != p2->value) {
      return false;
    }
  }
  if (prop1->property_type == NDFPropertyType::UInt32) {
    NDFPropertyUInt32 *p1 = (NDFPropertyUInt32 *)(prop1);
    NDFPropertyUInt32 *p2 = (NDFPropertyUInt32 *)(prop2);
    if (p1->value != p2->value) {
      return false;
    }
  }
  if (prop1->property_type == NDFPropertyType::Int16) {
    NDFPropertyInt16 *p1 = (NDFPropertyInt16 *)(prop1);
    NDFPropertyInt16 *p2 = (NDFPropertyInt16 *)(prop2);
    if (p1->value != p2->value) {
      return false;
    }
  }
  if (prop1->property_type == NDFPropertyType::Int32) {
    NDFPropertyInt32 *p1 = (NDFPropertyInt32 *)(prop1);
    NDFPropertyInt32 *p2 = (NDFPropertyInt32 *)(prop2);
    if (p1->value != p2->value) {
      return false;
    }
  }
  if (prop1->property_type == NDFPropertyType::String) {
    NDFPropertyString *p1 = (NDFPropertyString *)(prop1);
    NDFPropertyString *p2 = (NDFPropertyString *)(prop2);
    if (p1->value != p2->value) {
      return false;
    }
  }
  if (prop1->property_type == NDFPropertyType::WideString) {
    NDFPropertyWideString *p1 = (NDFPropertyWideString *)(prop1);
    NDFPropertyWideString *p2 = (NDFPropertyWideString *)(prop2);
    if (p1->value != p2->value) {
      return false;
    }
  }
  if (prop1->is_import_reference()) {
    NDFPropertyImportReference *p1 = (NDFPropertyImportReference *)(prop1);
    NDFPropertyImportReference *p2 = (NDFPropertyImportReference *)(prop2);
    if (p1->import_name != p2->import_name) {
      return false;
    }
  }
  if (prop1->is_object_reference()) {
    NDFPropertyObjectReference *p1 = (NDFPropertyObjectReference *)(prop1);
    NDFPropertyObjectReference *p2 = (NDFPropertyObjectReference *)(prop2);
    if (p1->object_name != p2->object_name) {
      return false;
    }
  }
  if (prop1->is_list()) {
    NDFPropertyList *list1 = (NDFPropertyList *)(prop1);
    NDFPropertyList *list2 = (NDFPropertyList *)(prop2);
    REQUIRE(list1->values.size() == list2->values.size());
    for (size_t x = 0; x < list1->values.size(); x++) {
      if (!check_property_equality(list1->values[x].get(),
                                   list2->values[x].get())) {
        return false;
      }
    }
  }
  return true;
}

bool check_object_equality(NDFObject *obj1, NDFObject *obj2) {
  if (obj1->name != obj2->name) {
    spdlog::info("object names differ {} should be {}", obj2->name, obj1->name);
    return false;
  }
  if (obj1->class_name != obj2->class_name) {
    spdlog::info("object class names differ {} should be {}", obj2->class_name,
                 obj1->class_name);
    return false;
  }
  if (obj1->export_path != obj2->export_path) {
    spdlog::info("object export paths differ {} should be {}",
                 obj2->export_path, obj1->export_path);
    return false;
  }
  if (obj1->is_top_object != obj2->is_top_object) {
    spdlog::info("object top object flag differ {} should be {}",
                 obj2->is_top_object, obj1->is_top_object);
    return false;
  }
  if (obj1->properties.size() != obj2->properties.size()) {
    spdlog::info("object property counts differ {} should be {}",
                 obj2->properties.size(), obj1->properties.size());
    return false;
  }
  for (size_t x = 0; x < obj1->properties.size(); x++) {
    if (!check_property_equality(obj1->properties[x].get(),
                                 obj2->properties[x].get())) {
      return false;
    }
  }
  return true;
}

TEST_CASE_PERSISTENT_FIXTURE(PyFixture, "ndf_db tests", "[ndf_db]") {
  SECTION("test ndf_file table contains stash file") {
    NDF_DB db;
    db.init();
    auto stash_ndf_id_opt = db.get_file(":stash:", ":stash:");
    REQUIRE(stash_ndf_id_opt.has_value());
  }
  SECTION("test adding and reading an object") {
    NDF_DB db;
    db.init("/tmp/test.db");
    // create test file
    auto ndf_file_id_opt =
        db.insert_file("$/test/file.ndfbin", "/tmp/foo", "/tmp/bar", "test");
    REQUIRE(ndf_file_id_opt.has_value());
    auto ndf_file_id = ndf_file_id_opt.value();

    // create test object into the test file
    auto obj1 = ndf_generator::gen_random_object();
    // add properties to test object
    ndf_generator::add_random_uint8(obj1);
    ndf_generator::add_random_uint16(obj1);
    ndf_generator::add_random_uint32(obj1);
    ndf_generator::add_random_list(obj1);
    ndf_generator::add_object_reference(obj1, "test_object");
    ndf_generator::add_import_reference(obj1, "$/foo/bar");
    obj1.db_ndf_id = ndf_file_id;

    // insert into DB
    spdlog::info("add obj1");
    auto obj_id_opt = db.insert_object(obj1);
    REQUIRE(obj_id_opt.has_value());
    auto obj_id = obj_id_opt.value();

    spdlog::info("check obj1");
    // now check the DB contains the correct values
    auto db_obj = db.get_object(obj_id);
    spdlog::debug("loaded object {}", obj_id);
    REQUIRE(db_obj.has_value());
    REQUIRE(check_object_equality(&obj1, &db_obj.value()));
  }

  SECTION("changing object names") {
    NDF_DB db;
    db.init();
    // create test file
    auto ndf_file_id_opt =
        db.insert_file("$/test/file.ndfbin", "/tmp/foo", "/tmp/bar", "test");
    REQUIRE(ndf_file_id_opt.has_value());
    auto ndf_file_id = ndf_file_id_opt.value();

    spdlog::info("creating obj");
    // create test objects into the test file
    NDFObject obj;
    obj.name = "ndf_object";
    obj.class_name = "TTestClass";
    obj.is_top_object = true;
    obj.export_path = "$/test/object";
    obj.db_ndf_id = ndf_file_id;
    // add properties to test object
    ndf_generator::add_random_uint8(obj);
    ndf_generator::add_random_uint16(obj);
    ndf_generator::add_random_uint32(obj);
    ndf_generator::add_random_list(obj);
    ndf_generator::add_object_reference(obj, "test_object");
    ndf_generator::add_import_reference(obj, "$/test/object1");
    auto obj_id_opt = db.insert_object(obj);
    REQUIRE(obj_id_opt.has_value());
    auto obj_id = obj_id_opt.value();
    spdlog::info("creating obj1");
    NDFObject obj1;
    obj1.name = "test_object";
    obj1.class_name = "TTestClass";
    obj1.is_top_object = true;
    obj1.export_path = "$/test/object1";
    obj1.db_ndf_id = ndf_file_id;
    // add properties to test object
    ndf_generator::add_random_uint8(obj1);
    ndf_generator::add_random_uint16(obj1);
    ndf_generator::add_random_uint32(obj1);
    ndf_generator::add_random_list(obj1);
    ndf_generator::add_object_reference(obj1, "test_object_2");
    auto obj1_id_opt = db.insert_object(obj1);
    REQUIRE(obj1_id_opt.has_value());
    auto obj1_id = obj1_id_opt.value();

    spdlog::info("creating obj2");
    NDFObject obj2;
    obj2.name = "test_object_2";
    obj2.class_name = "TInt";
    obj2.is_top_object = false;
    obj2.export_path = "$/test/object2";
    obj2.db_ndf_id = ndf_file_id;
    ndf_generator::add_random_uint8(obj2);
    auto obj2_id_opt = db.insert_object(obj2);
    REQUIRE(obj2_id_opt.has_value());
    auto obj2_id = obj2_id_opt.value();
    db.fix_references(ndf_file_id);

    // now change object name of obj2 and check the name changed in the DB + the
    // references
    REQUIRE(db.change_object_name(obj2_id, "new_test_object"));
    REQUIRE(db.change_export_path(obj1_id, "$/test/new_path"));
    // check name is changed
    auto db_obj2 = db.get_object(obj2_id);
    REQUIRE(db_obj2.has_value());
    REQUIRE(db_obj2.value().name == "new_test_object");
    // check reference is changed
    auto db_obj1 = db.get_object(obj1_id);
    REQUIRE(db_obj1.has_value());
    REQUIRE(db_obj1.value().properties.size() == 5);
    REQUIRE(db_obj1.value().properties[4]->is_object_reference());
    NDFPropertyObjectReference *obj_ref =
        (NDFPropertyObjectReference *)db_obj1.value().properties[4].get();
    REQUIRE(obj_ref->object_name == "new_test_object");
    // check the other object reference is not changed
    auto db_obj = db.get_object(obj_id);
    REQUIRE(db_obj.has_value());
    REQUIRE(db_obj.value().properties.size() == 6);
    REQUIRE(db_obj.value().properties[4]->is_object_reference());
    NDFPropertyObjectReference *obj_unchanged_ref =
        (NDFPropertyObjectReference *)db_obj.value().properties[4].get();
    REQUIRE(obj_unchanged_ref->object_name == "test_object");
    // check changed import path
    REQUIRE(db_obj1.value().export_path == "$/test/new_path");
    REQUIRE(db_obj.value().properties[5]->is_import_reference());
    NDFPropertyImportReference *import_ref =
        (NDFPropertyImportReference *)db_obj.value().properties[5].get();
    REQUIRE(import_ref->import_name == "$/test/new_path");
  }
  SECTION("insert many objects") {
    NDF_DB db;
    fs::remove_all("/tmp/foo.db");
    db.init("/tmp/foo.db");
    // db.init("");

    // create test file
    auto ndf_file_id_opt =
        db.insert_file("$/test/file.ndfbin", "/tmp/foo", "/tmp/bar", "test");
    REQUIRE(ndf_file_id_opt.has_value());
    auto ndf_file_id = ndf_file_id_opt.value();

    {
      auto start = std::chrono::high_resolution_clock::now();
      for (int x = 0; x < 160000; x++) {
        NDFObject obj;
        obj.name = "ndf_object_" + std::to_string(x);
        obj.class_name = "TTestClass";
        obj.export_path = "$/test/object" + std::to_string(x);
        obj.is_top_object = x % 99;
        db.insert_only_object(ndf_file_id, obj);
      }
      auto end = std::chrono::high_resolution_clock::now();
      std::chrono::duration<double> elapsed_seconds = end - start;
      spdlog::info("elapsed time inserting objects: {}",
                   elapsed_seconds.count());
    }
    {
      auto start = std::chrono::high_resolution_clock::now();
      auto object_names_opt = db.get_object_names(ndf_file_id);
      REQUIRE(object_names_opt);
      auto end = std::chrono::high_resolution_clock::now();
      std::chrono::duration<double> elapsed_seconds = end - start;
      spdlog::info("elapsed time getting all object names: {}",
                   elapsed_seconds.count());
    }
    {
      auto start = std::chrono::high_resolution_clock::now();
      auto objects_opt = db.get_only_objects(ndf_file_id);
      REQUIRE(objects_opt);
      auto end = std::chrono::high_resolution_clock::now();
      std::chrono::duration<double> elapsed_seconds = end - start;
      spdlog::info("elapsed time getting all objects: {}",
                   elapsed_seconds.count());
    }
  }

  SECTION("load generated test files and check they are parsed and builded "
          "correctly") {
    NDF_DB db;
    db.init();
    fs::path output_files_path =
        fs::temp_directory_path() / "testfiles" / "compare_ndfbin";
    fs::create_directories(output_files_path);

    fs::path files_path = fs::temp_directory_path() / "testfiles" / "ndfbin";

    NDF ndf_from_bin;
    auto ndf_from_bin_id_opt = db.insert_file("$/test/test.ndfbin", files_path,
                                              output_files_path, "test_bin");
    REQUIRE(ndf_from_bin_id_opt.has_value());
    int ndf_from_bin_id = ndf_from_bin_id_opt.value();
    ndf_from_bin.load_from_ndfbin(files_path / "test.ndfbin");
    spdlog::info("property count {}",
                 ndf_from_bin.object_map.begin()->second.properties.size());
    REQUIRE(ndf_from_bin.object_map.begin()->second.properties.size() > 0);
    ndf_from_bin.insert_into_db(&db, ndf_from_bin_id);

    NDF ndf_from_xml;
    auto ndf_from_xml_id_opt = db.insert_file(
        "$/test/test.ndfbin.xml", files_path, output_files_path, "test_xml");
    REQUIRE(ndf_from_xml_id_opt.has_value());
    int ndf_from_xml_id = ndf_from_xml_id_opt.value();
    ndf_from_xml.load_from_ndf_xml(files_path / "test.ndfbin.xml");
    ndf_from_xml.insert_into_db(&db, ndf_from_xml_id);
    REQUIRE(ndf_from_bin.object_map.size() == ndf_from_xml.object_map.size());

    NDF ndf_from_db_1;
    ndf_from_db_1.load_from_db(&db, ndf_from_bin_id);
    REQUIRE(ndf_from_db_1.object_map.size() == ndf_from_bin.object_map.size());
    auto object_it = ndf_from_bin.object_map.begin();
    while (object_it != ndf_from_bin.object_map.end()) {
      REQUIRE(ndf_from_bin.object_map.find(object_it->first) !=
              ndf_from_bin.object_map.end());
      REQUIRE(check_object_equality(
          &object_it.value(), &ndf_from_bin.object_map[object_it->first]));
      object_it++;
    }
    ndf_from_db_1.save_as_ndf_xml(output_files_path / "test.ndfbin.xml");
    ndf_from_db_1.save_as_ndfbin(output_files_path / "test.ndfbin");
  }
  NDF_DB db;
  db.init();
  SECTION("get object from database") { REQUIRE(false); }
  SECTION("get property from database") { REQUIRE(false); }
  SECTION("get properties of object from database") { REQUIRE(false); }
  SECTION("get class properties from database") { REQUIRE(false); }
}
