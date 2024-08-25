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

  // insert into DB
  auto obj_id_opt = db.insert_object(ndf_file_id, obj1);
  REQUIRE(obj_id_opt.has_value());
  auto obj_id = obj_id_opt.value();

  // now check the DB contains the correct values
  auto db_obj = db.get_object(obj_id);
  REQUIRE(db_obj.has_value());
  REQUIRE(obj1.name == db_obj->name);
  REQUIRE(obj1.class_name == db_obj->class_name);
  REQUIRE(obj1.export_path == db_obj->export_path);
  REQUIRE(obj1.is_top_object == db_obj->is_top_object);
  REQUIRE(obj1.properties.size() == db_obj->properties.size());
  for (size_t i = 0; i < obj1.properties.size(); i++) {
    REQUIRE(check_property_equality(obj1.properties[i].get(),
                                    db_obj->properties[i].get()));
  }
}

TEST_CASE_PERSISTENT_FIXTURE(PyFixture, "test DB -> ndf") {}
