#include <catch2/catch_all.hpp>

#include "catch2/catch_test_macros.hpp"
#include "generator.hpp"

#include <filesystem>
#include <pybind11/embed.h>
namespace py = pybind11;

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

TEST_CASE_PERSISTENT_FIXTURE(PyFixture, "test ndf -> DB") {}

TEST_CASE_PERSISTENT_FIXTURE(PyFixture, "test DB -> ndf") {}
