#include "basic.hpp"

#include "argparse/argparse.hpp"
#include "edat.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>

using namespace etcetera;
using namespace std::string_literals;
namespace fs = std::filesystem;

int main(int argc, char **argv) {
  argparse::ArgumentParser program("EDat");
  program.add_argument("input").required().help("Input file");
  program.add_argument("-o", "--output").help("Output folder");
  program.add_argument("-r", "--rebuild").default_value(false).implicit_value(true).help(
      "Rebuild the input file (fix alignment and checksums for all files)");
  program.add_argument("-i", "--in-place").default_value(false).implicit_value(true).help(
      "Rebuild the input file in place (fix alignment and checksums for all files), ignored if -r is not set.");
  program.add_argument("-v", "--verbose")
      .default_value(false)
      .implicit_value(true)
      .help("Verbose output");
  program.add_argument("--dont-read-files")
      .default_value(false)
      .implicit_value(true)
      .help("Don't read the files, just parse the dictionary, necessary for some ndfbin edats.");
  program.add_argument("-p", "--pack").default_value(false).implicit_value(true).help(
      "instead of parsing the input file, pack the input xml file into an edat file, ignored if -r is set.");

  try {
    program.parse_args(argc, argv);
  } catch (const std::runtime_error &err) {
    std::cout << err.what() << std::endl;
    std::cout << program;
    exit(1);
  }

#ifdef _WIN32
  std::setlocale(LC_NUMERIC, "en-US");
#else
  std::setlocale(LC_NUMERIC, "en_US.UTF-8");
#endif

  if (!program.get<bool>("-v")) {
    spdlog::set_level(spdlog::level::warn);
  } else {
    spdlog::set_level(spdlog::level::debug);
  }

  fs::path input_path = program.get("input");

  if(program.get<bool>("-r")) {
    spdlog::debug("input path {}", input_path.c_str());
    std::ifstream input;
    input.open(input_path, std::ios::binary);
    auto edat = EDat::create();
    if(!program.get<bool>("-i")) {
      edat->outpath = program.get("--output");
    } else {
      edat->outpath = fs::temp_directory_path();
    }
    edat->parse(input);
    auto output_path = input_path;
    if(!program.get<bool>("-i")) {
      output_path = program.get("--output") / input_path.filename();
    }
    spdlog::debug("Output path: {}", output_path.c_str());
    std::fstream ofs(output_path, std::fstream::in | std::fstream::out | std::fstream::binary | std::fstream::trunc);
    edat->build(ofs);
    return 0;
  }

  auto edat = EDat::create();
  edat->outpath = program.get("--output");
  edat->read_files = !program.get<bool>("--dont-read-files");

  if(!program.get<bool>("-p")) {
    std::ifstream input;
    input.open(program.get("input"), std::ios::binary);
    edat->parse(input);

    pugi::xml_document doc;
    auto root = doc.append_child("root");
    edat->build_xml(root, "EDat");
    std::string outpath = program.get("--output") / input_path.filename().replace_extension(".xml");
    doc.save_file(outpath.c_str());
  } else {
    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_file(program.get("input").c_str());
    spdlog::debug("Load result: {}", result.description());
    edat->parse_xml(doc.child("root").child("EDat"), "EDat", true);

    std::fstream ofs(program.get("--output") / input_path.filename().replace_extension(".dat"), std::fstream::in | std::fstream::out | std::fstream::binary | std::fstream::trunc);
    edat->build(ofs);
  }
}
