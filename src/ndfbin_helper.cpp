#include "argparse/argparse.hpp"

#include "ndf.hpp"

#include "spdlog/spdlog.h"

#include <filesystem>
namespace fs = std::filesystem;

#include "pugixml.hpp"

int main(int argc, char **argv) {
  argparse::ArgumentParser program("ndfbin");
  program.add_argument("input").help("Input file");
  program.add_argument("output").help("Output folder");
  program.add_argument("-v", "--verbose")
      .default_value(false)
      .implicit_value(true)
      .help("Verbose output");
  program.add_argument("-p", "--pack")
      .default_value(false)
      .implicit_value(true)
      .help("instead of parsing the input file, pack the input xml file into a "
            "ndfbin file");

  std::setlocale(LC_NUMERIC, "en_US.UTF-8");

  try {
    program.parse_args(argc, argv);
  } catch (const std::runtime_error &err) {
    std::cout << err.what() << std::endl;
    std::cout << program;
    exit(1);
  }

  if (!program.get<bool>("-v")) {
    spdlog::set_level(spdlog::level::warn);
  } else {
    spdlog::set_level(spdlog::level::debug);
  }
    
  if(!fs::exists(program.get<std::string>("input"))) {
    spdlog::error("Input file does not exist");
    exit(1);
  }

  fs::create_directories(program.get<std::string>("output"));

  if (!program.get<bool>("-p")) {
    NDF ndf;
    ndf.load_from_ndfbin(program.get<std::string>("input"));
    fs::path out_filename = program.get("input");
    out_filename = out_filename.filename();
    out_filename.replace_extension(".xml");
    ndf.save_as_ndf_xml(fs::path(program.get<std::string>("output")) / out_filename);
  } else {
    NDF ndf;
    ndf.load_from_ndf_xml(program.get<std::string>("input"));
    fs::path out_filename = program.get("input");
    out_filename = out_filename.filename();
    out_filename.replace_extension(".ndfbin");
    ndf.save_as_ndfbin(fs::path(program.get<std::string>("output")) / out_filename);
  }
}
