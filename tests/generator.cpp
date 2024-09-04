// test data generator for modding suite,
// creates dat files with some random ndfbin data
// and their corresponding xml files
#include "generator.hpp"
#include "ndf_properties.hpp"

#include <argparse/argparse.hpp>

#include <experimental/random>
#include <memory>
#include <stdint.h>

#include <pybind11/embed.h>
namespace py = pybind11;

#include "pugixml.hpp"

NDFObject ndf_generator::gen_random_object(int id) {
  NDFObject obj;
  obj.name = std::format("test_object_{}", id);
  obj.class_name = "TTestClass";
  obj.is_top_object = true;
  obj.export_path = std::format("$/test/object{}", id);
  return obj;
}

void ndf_generator::add_random_object(NDF &ndf) {
  auto obj = gen_random_object(ndf.object_map.size() + 1);

  ndf.add_object(std::move(obj));
}

void ndf_generator::add_random_objects(NDF &ndf, size_t size) {
  for (size_t i = 0; i < size; i++) {
    auto obj = gen_random_object(ndf.object_map.size() + 1);
    add_random_properties(obj, 10);
    ndf.add_object(std::move(obj));
  }
}

std::unique_ptr<NDFProperty> ndf_generator::gen_random_string(int idx) {
  auto prop = std::make_unique<NDFPropertyString>();
  prop->property_idx = idx;
  prop->property_type = NDFPropertyType::String;
  prop->property_name = std::format("TestString_{}", prop->property_idx);
  uint8_t i = std::experimental::randint(0, 255);
  prop->value = std::format("TestString_{}", i);
  return prop;
}

void ndf_generator::add_random_string(NDFObject &obj) {
  auto prop = gen_random_string(obj.properties.size());
  obj.properties.push_back(std::move(prop));
}

std::unique_ptr<NDFProperty> ndf_generator::gen_random_uint8(int idx) {
  auto prop = std::make_unique<NDFPropertyUInt8>();
  prop->property_idx = idx;
  prop->property_type = NDFPropertyType::UInt8;
  prop->property_name = std::format("TestUInt8_{}", prop->property_idx);
  prop->value = std::experimental::randint(0, 255);
  return prop;
}

void ndf_generator::add_random_uint8(NDFObject &obj) {
  auto prop = gen_random_uint8(obj.properties.size());
  obj.properties.push_back(std::move(prop));
}

std::unique_ptr<NDFProperty> ndf_generator::gen_random_uint16(int idx) {
  auto prop = std::make_unique<NDFPropertyUInt16>();
  prop->property_idx = idx;
  prop->property_type = NDFPropertyType::UInt16;
  prop->property_name = std::format("TestUInt16_{}", prop->property_idx);
  prop->value = std::experimental::randint((uint16_t)0, (uint16_t)UINT16_MAX);
  return prop;
}

void ndf_generator::add_random_uint16(NDFObject &obj) {
  auto prop = gen_random_uint16(obj.properties.size());
  obj.properties.push_back(std::move(prop));
}

std::unique_ptr<NDFProperty> ndf_generator::gen_random_uint32(int idx) {
  auto prop = std::make_unique<NDFPropertyUInt32>();
  prop->property_idx = idx;
  prop->property_type = NDFPropertyType::UInt32;
  prop->property_name = std::format("TestUInt32_{}", prop->property_idx);
  prop->value = std::experimental::randint((uint32_t)0, (uint32_t)UINT32_MAX);
  return prop;
}

void ndf_generator::add_random_uint32(NDFObject &obj) {
  auto prop = gen_random_uint32(obj.properties.size());
  obj.properties.push_back(std::move(prop));
}

std::unique_ptr<NDFProperty> ndf_generator::gen_random_int32(int idx) {
  auto prop = std::make_unique<NDFPropertyInt32>();
  prop->property_idx = idx;
  prop->property_type = NDFPropertyType::Int32;
  prop->property_name = std::format("TestInt32_{}", prop->property_idx);
  prop->value = std::experimental::randint((int)INT32_MIN, (int)INT32_MAX);
  return prop;
}

void ndf_generator::add_random_int32(NDFObject &obj) {
  auto prop = gen_random_uint32(obj.properties.size());
  obj.properties.push_back(std::move(prop));
}

std::unique_ptr<NDFProperty> ndf_generator::gen_random_list(int idx) {
  auto prop = std::make_unique<NDFPropertyList>();
  prop->property_idx = idx;
  prop->property_type = NDFPropertyType::List;
  prop->property_name = std::format("TestList_{}", prop->property_idx);

  for (int i = 0; i < 10; i++) {
    auto item = gen_random_uint32(-1);
    item->property_name = "ListItem";
    prop->values.push_back(std::move(item));
  }
  return prop;
}

void ndf_generator::add_random_list(NDFObject &obj) {
  auto prop = gen_random_list(obj.properties.size());
  obj.properties.push_back(std::move(prop));
}

std::unique_ptr<NDFProperty> ndf_generator::gen_random_map(int idx) {
  auto prop = std::make_unique<NDFPropertyMap>();
  prop->property_idx = idx;
  prop->property_type = NDFPropertyType::Map;
  prop->property_name = std::format("TestMap_{}", prop->property_idx);

  for (int i = 0; i < 10; i++) {
    auto key = gen_random_uint32(-1);
    key->property_name = "Key";
    auto value = gen_random_int32(-1);
    value->property_name = "Value";
    prop->values.push_back({std::move(key), std::move(value)});
  }
  return prop;
}

void ndf_generator::add_random_map(NDFObject &obj) {
  auto prop = gen_random_map(obj.properties.size());
  obj.properties.push_back(std::move(prop));
}

std::unique_ptr<NDFProperty>
ndf_generator::gen_object_reference(int idx, std::string ref) {
  auto prop = std::make_unique<NDFPropertyObjectReference>();
  prop->property_idx = idx;
  prop->property_type = NDFPropertyType::ObjectReference;
  prop->property_name = std::format("ObjRef_{}", prop->property_idx);

  prop->object_name = ref;
  return prop;
}

void ndf_generator::add_object_reference(NDFObject &obj, std::string ref) {
  auto prop = gen_object_reference(obj.properties.size(), ref);
  obj.properties.push_back(std::move(prop));
}

std::unique_ptr<NDFProperty>
ndf_generator::gen_import_reference(int idx, std::string ref) {
  auto prop = std::make_unique<NDFPropertyImportReference>();
  prop->property_idx = idx;
  prop->property_type = NDFPropertyType::ImportReference;
  prop->property_name = std::format("ObjRef_{}", prop->property_idx);

  prop->import_name = ref;
  return prop;
}

void ndf_generator::add_import_reference(NDFObject &obj, std::string ref) {
  auto prop = gen_import_reference(obj.properties.size(), ref);
  obj.properties.push_back(std::move(prop));
}

std::unique_ptr<NDFProperty> ndf_generator::gen_random_property(int idx) {
  std::unique_ptr<NDFProperty> prop;
  switch (std::experimental::randint(0, 6)) {
  case 0:
    prop = gen_random_string(idx);
    break;
  case 1:
    prop = gen_random_uint8(idx);
    break;
  case 2:
    prop = gen_random_uint16(idx);
    break;
  case 3:
    prop = gen_random_uint32(idx);
    break;
  case 4:
    prop = gen_random_int32(idx);
    break;
  case 5:
    prop = gen_random_list(idx);
    break;
  case 6:
    prop = gen_random_map(idx);
    break;
  }
  return prop;
}

void ndf_generator::add_random_property(NDFObject &obj) {
  auto prop = gen_random_property(obj.properties.size());
  obj.properties.push_back(std::move(prop));
}

std::vector<std::unique_ptr<NDFProperty>>
ndf_generator::gen_random_properties(int idx, int count) {
  std::vector<std::unique_ptr<NDFProperty>> prop;
  switch (std::experimental::randint(0, 6)) {
  case 0:
    for (int i = 0; i < count; i++) {
      prop.push_back(gen_random_string(idx + i));
    }
    break;
  case 1:
    for (int i = 0; i < count; i++) {
      prop.push_back(gen_random_uint8(idx + i));
    }
    break;
  case 2:
    for (int i = 0; i < count; i++) {
      prop.push_back(gen_random_uint16(idx + i));
    }
    break;
  case 3:
    for (int i = 0; i < count; i++) {
      prop.push_back(gen_random_uint32(idx + i));
    }
    break;
  case 4:
    for (int i = 0; i < count; i++) {
      prop.push_back(gen_random_int32(idx + i));
    }
    break;
  case 5:
    for (int i = 0; i < count; i++) {
      prop.push_back(gen_random_list(idx + i));
    }
    break;
  case 6:
    for (int i = 0; i < count; i++) {
      prop.push_back(gen_random_map(idx + i));
    }
    break;
  }
  return prop;
}

void ndf_generator::add_random_properties(NDFObject &obj, int count) {
  auto props = gen_random_properties(obj.properties.size(), count);
  for (auto &prop : props) {
    obj.properties.push_back(std::move(prop));
  }
}

// call with vfs_path -> path to the file
void ndf_generator::create_edat(
    fs::path path, std::unordered_map<std::string, std::string> files) {
  fs::path out_path = path.parent_path() / "out";
  // first we create the xml file for the edat
  pugi::xml_document doc;
  auto root = doc.append_child("EDat");
  root.append_attribute("sectorSize") = 8192;
  root.append_attribute("_wgrd_cons_parsers_version") = "0.2.11";
  for (auto &[vfs_path, _] : files) {
    auto file = root.append_child("File");
    std::string p = ("out" / fs::path(vfs_path));
    std::replace(p.begin(), p.end(), '/', '\\');
    file.append_attribute("path") = p.c_str();
  }
  fs::path xml_path = path;
  xml_path = xml_path.replace_extension("edat.xml");
  doc.save_file(xml_path.string().c_str());

  // now we create the out folder and copy the files in there
  fs::create_directories(out_path);
  for (auto &[vfs_path, fs_path] : files) {
    spdlog::info("copying file {} to {}", fs_path,
                 (out_path / vfs_path).string());
    fs::copy(fs_path, out_path / vfs_path,
             fs::copy_options::overwrite_existing);
  }

  // now we create the edat file
  py::object edat =
      py::module::import("wgrd_cons_parsers.edat").attr("EdatMain")();

  // as the EdatMain uses the . instead of [] for accessing, we need the
  // dingsda Container
  py::object container =
      py::module::import("dingsda.lib.containers").attr("Container");
  py::dict args = py::dict();
  args["no_alignment"] = true;
  args["disable_checksums"] = true;
  edat.attr("args") = container(args);
  py::object data = edat.attr("get_data")(xml_path.string());
  edat.attr("pack")(xml_path.string(), out_path.string(), data);
}

void ndf_generator::create_test_files(fs::path output_folder) {
  py::str py_exec = (py::module::import("sys").attr("executable"));
  spdlog::info(std::string(py_exec));
  py::str py_path = (py::module::import("sys").attr("path"));
  spdlog::info(std::string(py_path));

  std::srand(std::time(nullptr));

  fs::create_directories(output_folder / "ndfbin");

  // let's first create some random ndfbin data
  NDF test_ndf;
  {
    add_random_object(test_ndf);
    auto &obj = test_ndf.object_map.begin().value();
    add_random_uint16(obj);
    add_random_uint32(obj);
    add_random_uint8(obj);
    add_random_uint8(obj);
    add_random_list(obj);
    add_random_map(obj);
  }
  // now save it in the output folder
  test_ndf.save_as_ndf_xml(output_folder / "ndfbin" / "test.ndfbin.xml");
  test_ndf.save_as_ndfbin(output_folder / "ndfbin" / "test.ndfbin");

  // now generate an edat file containing the ndfbin file
  create_edat(output_folder / "test.dat",
              {{"test.ndfbin", output_folder / "ndfbin" / "test.ndfbin"}});
}

/*
int main(int argc, char *argv[]) {
  argparse::ArgumentParser program;
  program.add_argument("-o").help(gettext("Path to output folder"));

  try {
    program.parse_args(argc, argv);
  } catch (const std::runtime_error &err) {
    std::cout << err.what() << std::endl;
    std::cout << program;
    exit(0);
  }
  py::scoped_interpreter guard{};

  fs::path output_folder;
  if (program.present("-o")) {
    output_folder = program.get<std::string>("-o");
  } else {
    output_folder = fs::temp_directory_path() / "modding_suite_test_data";
  }

  create_test_files(output_folder);
}
*/
