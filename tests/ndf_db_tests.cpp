#include <catch2/catch_all.hpp>

#include "catch2/catch_test_macros.hpp"
#include "generator.hpp"
#include "ndf_properties.hpp"

#include <filesystem>
#include <memory>
#include <pybind11/embed.h>
namespace py = pybind11;

#include "ndf_db.hpp"

struct PyFixture {
private:
  fs::path directory = fs::temp_directory_path() / "testfiles";

public:
  PyFixture() {
    pybind11::initialize_interpreter();
    ndf_generator::create_test_files(directory);
  }
  ~PyFixture() {
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

TEST_CASE_PERSISTENT_FIXTURE(PyFixture, "test ndf properties -> DB") {
  NDF_DB db;
  db.init();
  // create test file
  auto ndf_file_id_opt =
      db.insert_file("$/test/file.ndfbin", "/tmp/foo", "/tmp/bar");
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

  // insert into DB
  auto obj_id_opt = db.insert_object(ndf_file_id, obj1);
  REQUIRE(obj_id_opt.has_value());
  auto obj_id = obj_id_opt.value();

  // now check the DB contains the correct values
  auto db_obj = db.get_object(obj_id);
  REQUIRE(db_obj.has_value());
  REQUIRE(check_object_equality(&obj1, &db_obj.value()));
}

TEST_CASE_PERSISTENT_FIXTURE(PyFixture, "test changing object name") {
  NDF_DB db;
  db.init();
  // create test file
  auto ndf_file_id_opt =
      db.insert_file("$/test/file.ndfbin", "/tmp/foo", "/tmp/bar");
  REQUIRE(ndf_file_id_opt.has_value());
  auto ndf_file_id = ndf_file_id_opt.value();

  // create test object into the test file
  NDFObject obj1;
  obj1.name = "test_object";
  obj1.class_name = "TTestClass";
  obj1.is_top_object = true;
  obj1.export_path = "$/test/object1";
  // add properties to test object
  ndf_generator::add_random_uint8(obj1);
  ndf_generator::add_random_uint16(obj1);
  ndf_generator::add_random_uint32(obj1);
  ndf_generator::add_random_list(obj1);
  ndf_generator::add_object_reference(obj1, "test_object_2");
  auto obj1_id_opt = db.insert_object(ndf_file_id, obj1);
  REQUIRE(obj1_id_opt.has_value());
  auto obj1_id = obj1_id_opt.value();

  NDFObject obj2;
  obj2.name = "test_object_2";
  obj2.class_name = "TInt";
  obj2.is_top_object = false;
  obj2.export_path = "$/test/object2";
  ndf_generator::add_random_uint8(obj2);
  auto obj2_id_opt = db.insert_object(ndf_file_id, obj2);
  REQUIRE(obj2_id_opt.has_value());
  auto obj2_id = obj2_id_opt.value();

  // now change object name of obj2 and check the name changed in the DB + the
  // references
  REQUIRE(db.change_object_name(obj2_id, "new_test_object"));
}
