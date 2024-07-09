#include "ndfbin.hpp"

#include "argparse/argparse.hpp"

#include <fstream>

using namespace etcetera;

int main(int argc, char **argv) {
  argparse::ArgumentParser program("ndfbin");
  program.add_argument("input").help("Input file");
  program.add_argument("output").help("Output folder");
  program.add_argument("-v", "--verbose")
      .default_value(false)
      .implicit_value(true)
      .help("Verbose output");
  program.add_argument("-p", "--pack").default_value(false).implicit_value(true).help("instead of parsing the input file, pack the input xml file into a ndfbin file");


  try {
    program.parse_args(argc, argv);
  } catch (const std::runtime_error &err) {
    std::cout << err.what() << std::endl;
    std::cout << program;
    exit(1);
  }

  if(!program.get<bool>("-v")) {
    spdlog::set_level(spdlog::level::warn);
  } else {
    spdlog::set_level(spdlog::level::debug);
  }

  auto NdfBin = get_ndfbin();

  if(!program.get<bool>("-p")) {
    std::ifstream input;
    input.open(program.get("input"), std::ios::binary);
    NdfBin->parse(input);

    spdlog::debug("parsed");
    NdfBin->dbg_print_tree("");

    pugi::xml_document doc;
    auto root = doc.append_child("root");
    NdfBin->build_xml(root, "NdfBin");
    std::string outpath = program.get("output") + "/ndfbin.xml";
    doc.save_file(outpath.c_str());
  } else {
    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_file(program.get("input").c_str());
    spdlog::debug("Load result: {}", result.description());
    NdfBin->parse_xml(doc.child("root").child("NdfBin"), "NdfBin", true);

    std::fstream ofs(program.get("output") + "/ndfbin.bin", std::fstream::in | std::fstream::out | std::fstream::binary | std::fstream::trunc);
    NdfBin->build(ofs);
  }
}
