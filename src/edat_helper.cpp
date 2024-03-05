#include "edat.hpp"
#include <argparse/argparse.hpp>

using namespace etcetera;
namespace fs = std::filesystem;

int main(int argc, char **argv) {
  argparse::ArgumentParser program("EDat");
  program.add_argument("-w", "--wgrd_path").help("path to the vanilla game files, e.g. C:/Program Files (x86)/Steam/steamapps/common/Wargame Red Dragon/Data/WARGAME/PC");
  program.add_argument("-m", "--modded_path").help("path to the modded game files");
  program.add_argument("-o", "--output").default_value("out/").help("path to the output directory");
  program.add_argument("-v", "--verbose").default_value(false).implicit_value(true).help("Verbose output");

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

  fs::path modded_path = program.get<std::string>("--modded_path");
  std::map<std::string, std::shared_ptr<EDat>> modded_edats;
  // unpack all original game files
  for (const auto &entry : fs::recursive_directory_iterator(modded_path)) {
    if (entry.is_regular_file()) {
      fs::path part = entry.path().lexically_relative(modded_path);
      std::cout << "loading: " << entry << " " << part << std::endl;

      // unpack only once
      if(!modded_edats.contains(part.filename())) {
        modded_edats[part.filename()] = EDat::create();
      } else {
        continue;
      }

      // parse the original game file
      std::ifstream orig_input;
      orig_input.open(fs::path(program.get("--wgrd_path")) / part, std::ios::binary);
      auto orig_edat = EDat::create();
      orig_edat->parse(orig_input);
    }
  }
  // iterate all modded game files
  for (const auto &entry : fs::recursive_directory_iterator(modded_path)) {
    if (entry.is_regular_file()) {
      fs::path part = entry.path().lexically_relative(modded_path);
      std::cout << "testing: " << entry << " " << part << std::endl;

      // parse the original game file
      std::ifstream orig_input;
      orig_input.open(fs::path(program.get("--wgrd_path")) / part, std::ios::binary);
      auto orig_edat = EDat::create();
      // only read the file headers
      orig_edat->read_files = false;
      orig_edat->parse(orig_input);

      // parse the modded game file
      std::ifstream modded_input;
      modded_input.open(entry.path(), std::ios::binary);
      auto modded_edat = EDat::create();
      modded_edat->outpath = program.get("--output");
      // only read the file headers
      modded_edat->read_files = false;
      modded_edat->parse(modded_input);

      custom_assert(modded_edats.contains(part.filename()));
      std::shared_ptr<EDat> new_edat = modded_edats[part.filename()];
      new_edat->outpath = program.get("--output");

      // compare the original and modded game files and get the changed files
      for (const auto &[path, header] : modded_edat->file_headers) {
        auto orig_header = orig_edat->file_headers[path];
        for(int i = 0; i < 16; i++) {
          // NDF_Win.dat needs ALL files from the original game, else the game doesn't start
          if ((header.checksum[i] != orig_header.checksum[i]) || part.filename() == "NDF_Win.dat") {
            std::cout << "changed file: " << entry << " " << path << std::endl;
            // the headers get rebuilt when building, the offsets are wrong obviously
            new_edat->file_headers[path] = header;
            // extract the changed file
            modded_edat->parse_single_file(modded_input, path, header);
            break;
          }
        }
      }
    }
  }

  for(auto &[path, edat] : modded_edats) {
    std::fstream ofs;
    fs::path outpath = program.get("--output") / fs::path(path);
    spdlog::debug("output path {}", outpath.c_str());
    std::filesystem::create_directories(outpath.parent_path());
    ofs.open(outpath, std::fstream::in | std::fstream::out | std::fstream::binary | std::fstream::trunc);
    edat->build(ofs);
  }

  return 0;
}
