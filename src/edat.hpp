#pragma once

#include "argparse/argparse.hpp"
#include "array.hpp"
#include "basic.hpp"
#include "conditional.hpp"
#include "helpers.hpp"
#include "md5.h"
#include "number.hpp"
#include "pointer.hpp"
#include "special.hpp"
#include "string.hpp"
#include "struct.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>

namespace fs = std::filesystem;

// these are all allowed characters in paths in their correct order
// all paths get sorted by this order
// note all paths are with \ and not /
// however internally only / is used so filepaths "just work"
static const std::vector<char> characters = {'/', '\\', '-', '.', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '_', ' ', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'};
struct dictionarySort {
  bool operator()(const std::string& a, const std::string& b) const {
    for(size_t i = 0; i < std::min(a.size(), b.size()); i++) {
      if(a[i] != b[i]) {
        auto a_i = std::find(characters.begin(), characters.end(), a[i]);
        auto b_i = std::find(characters.begin(), characters.end(), b[i]);
        etcetera::custom_assert(a_i != characters.end());
        etcetera::custom_assert(b_i != characters.end());
        return a_i < b_i;
      }
    }
    // a == b should not happen when parsing an edat
    // this would mean there are two paths equal
    spdlog::debug("EDat::dictionarySort a == b {} {}", a, b);
    etcetera::custom_assert(false);
    return false;
  }
};
class EDat : public etcetera::Base {
private:
  Chocobo1::MD5 dictionary_md5;
  dictionarySort sorter;

  struct Trie {
    std::map<char, Trie> children;
    [[nodiscard]] bool end() const {
      return children.empty();
    }

    void insert(const std::string& path) {
      Trie* current = this;
      for(auto c : path) {
        etcetera::custom_assert(std::find(characters.begin(), characters.end(), c) != characters.end());
        if(current->children.find(c) == current->children.end()) {
          current->children.emplace(c, Trie());
        }
        current = &current->children[c];
      }
    }

    std::vector<std::pair<std::string, Trie*>> get_parts() {
      std::vector<std::pair<std::string, Trie*>> ret;
      for(auto c : characters) {
        if(!children.contains(c)) {
          continue;
        }
        std::string part;
        part += c;
        Trie* current = &children[c];
        while(current->children.size() == 1) {
          auto &[child_c, child_node] = *current->children.begin();
          part += child_c;
          current = &child_node;
        }
        ret.emplace_back(part, current);
      }
      return ret;
    }
  };

public:
  struct EDatHeader {
    uint8_t magic[4];
    uint32_t unk0;
    uint8_t pad0[17];
    uint32_t offset_dictionary;
    uint32_t size_dictionary;
    uint32_t offset_files;
    uint32_t size_files;
    uint8_t pad1[4];
    uint32_t sectorSize;
    uint8_t checksum[16];
    uint8_t pad2[972];
  } __attribute__((packed));

  struct EDatFileHeader {
    uint32_t offset;
    uint32_t pad0;
    uint32_t size;
    uint32_t pad;
    uint8_t checksum[16];
  } __attribute__((packed));

  EDatHeader edat_header;
  std::map<std::string, EDatFileHeader> file_headers;

  using Base::get;
  using Base::get_field;
  using Base::get_offset;

  bool read_files = true;
  uint32_t sectorSize = 8192;
  std::string outpath = "./out/";

  explicit EDat(PrivateBase) : Base(PrivateBase()) {}

  static std::shared_ptr<EDat> create() {
    return std::make_shared<EDat>(PrivateBase());
  }

  std::any get() override {
    auto ret = std::vector<std::string>();
    ret.reserve(file_headers.size());
    for(auto &[path, _] : file_headers) {
      ret.push_back(path);
    }
    return ret;
  }

  size_t get_size() override {
    // we would need to build the radixtree here to get the size...
    etcetera::custom_assert(false);
    return 0;
  }

  /*
   * This method only parses the dictionary itself and fills the file_headers
   * */
  void parsePath(std::istream &stream, std::string path, uint32_t ending) {
    spdlog::debug("EDat::parsePath start {:02X} {} {:02X}", (size_t)stream.tellg(), path, ending);
    while(stream.tellg() < ending) {
      spdlog::debug("EDat::parsePath loopstart {:02X} {:02X}", (size_t)stream.tellg(), ending);
      size_t c = stream.tellg();

      uint32_t pathSize = 0;
      stream.read((char *)&pathSize, sizeof(pathSize));
      uint32_t entrySize = 0;
      stream.read((char *)&entrySize, sizeof(entrySize));

      size_t endpos = ending;
      if (entrySize != 0) {
        endpos = c + entrySize;
      }
      spdlog::debug("EDat::parsePath {:02X} pathSize {:02X} entrySize {:02X} endpos {:02X}", (size_t)stream.tellg(), pathSize, entrySize, endpos);

      // pathSize != 0 -> more parts of the path
      // pathSize == 0 -> the FileHeader
      if (pathSize != 0 and stream.tellg() != ending) {
        auto sc = etcetera::Aligned::create(2, etcetera::CString8l::create());
        sc->parse(stream);
        etcetera::custom_assert((size_t)stream.tellg() == (size_t)(c + pathSize));

        std::string subpath = sc->get<std::string>();

        spdlog::debug("EDat::parsePath {:02X} endpos {:02X} ending {:02X}", (size_t)stream.tellg(), endpos, ending);

        parsePath(stream, path + subpath, endpos);
      } else {
        spdlog::debug("EDat::parsePath end path {:02X} {}", (size_t)stream.tellg(), path);
        EDatFileHeader header = {};
        spdlog::debug("EDat::parsePath before reading file header {:02X}", (size_t)stream.tellg());
        stream.read((char *)&header, sizeof(EDatFileHeader));
        spdlog::debug("EDat::parsePath after reading file header {:02X}", (size_t)stream.tellg());

        auto sc = etcetera::Aligned::create(2, etcetera::CString8l::create());
        sc->parse(stream);

        std::string fPath = path + sc->get<std::string>();
        std::replace(fPath.begin(), fPath.end(), '\\', '/');
        std::filesystem::path filePath = fPath;
        file_headers.emplace(filePath, header);

        size_t endoffset = (size_t)stream.tellg();
        size_t tmp_offset = header.offset;
        size_t tmp_size = header.size;
        spdlog::debug("EDat::parsePath reading file {:02X} {}", (size_t)stream.tellg(), filePath.c_str());
        spdlog::debug("EDat::parsePath reading file @{:02X} {}", tmp_offset, tmp_size);

        if (!read_files) {
          continue;
        }

        parse_single_file(stream, filePath, header);

        stream.seekg(endoffset);
      }

      spdlog::debug("EDat::parsePath end of fn {:02X} {} {:02X} {:02X}", (size_t)stream.tellg(), path, endpos, ending);
      etcetera::custom_assert((size_t)stream.tellg() == endpos);
    }
  }

  void parse_single_file(std::istream &stream, fs::path filePath, const EDatFileHeader &header) {
    // create directory for path
    std::filesystem::path dirpath = outpath / filePath;
    dirpath = dirpath.parent_path();
    std::filesystem::create_directories(dirpath);

    if (header.size == 0) {
      // create empty file
      std::ofstream file(outpath / filePath, std::ios::binary);
      file.close();
      return;
    }
    // copy file to outpath
    char buf[4096];
    std::ofstream file(outpath / filePath, std::ios::binary);
    stream.seekg(edat_header.offset_files + header.offset, std::ios::beg);
    size_t remaining = header.size;
    while(remaining > 0) {
      size_t toRead = std::min(remaining, (size_t)sizeof(buf));
      stream.read(buf, toRead);
      file.write(buf, toRead);
      remaining -= toRead;
    }
    etcetera::custom_assert(remaining == 0);
  }

  std::any parse(std::istream &stream) override {
    file_headers.clear();
    edat_header = {};
    stream.read((char *)&edat_header, sizeof(edat_header));
    spdlog::debug("EDat::parse {:02X} EDatHeader", (size_t)stream.tellg());

    assert (edat_header.magic[0] == 'e');
    assert (edat_header.magic[1] == 'd');
    assert (edat_header.magic[2] == 'a');
    assert (edat_header.magic[3] == 't');
    assert (edat_header.unk0 == 2);
    for (unsigned char i : edat_header.pad0) {
      etcetera::custom_assert(i == 0);
    }
    for (unsigned char i : edat_header.pad1) {
      etcetera::custom_assert(i == 0);
    }
    for (unsigned char i : edat_header.pad2) {
      etcetera::custom_assert(i == 0);
    }
    etcetera::custom_assert(edat_header.offset_dictionary == sizeof(edat_header));

    sectorSize = edat_header.sectorSize;

    etcetera::custom_assert(edat_header.offset_dictionary == stream.tellg());

    if(edat_header.size_dictionary == 0) {
      return get();
    }

    uint32_t empty = 0;
    stream.read((char *)&empty, sizeof(empty));
    if(empty == 0x01) {
      return get();
    }
    if(empty != 0xA) {
      throw std::runtime_error("Expected 0x01 or 0x0A, got " + std::to_string(empty));
    }
    uint8_t pad[6];
    stream.read((char *)pad, sizeof(pad));
    for (unsigned char i : pad) {
      etcetera::custom_assert(i == 0);
    }

    // ensure outpath exists
    std::filesystem::create_directories(outpath);

    parsePath(stream, "", edat_header.offset_dictionary + edat_header.size_dictionary);

    return get();
  }

  void buildTrie(std::iostream &stream, Trie *trie, std::string path, bool is_last = false) {
    auto parts = trie->get_parts();

    size_t i = 0;
    for(auto &[part, node] : parts) {
      bool is_last_item = i == parts.size() - 1;
      i+=1;
      if(node->end()) {
        auto current_path = path + part;

        // already create aligned string, so we can use its size
        auto str = etcetera::CString8l::create();
        str->value = part;
        std::replace(str->value.begin(), str->value.end(), '/', '\\');
        auto aligned_str = etcetera::Aligned::create(2, str);


        // get the corresponding file header
        etcetera::custom_assert(file_headers.contains(current_path));
        EDatFileHeader &header = file_headers[current_path];

        uint32_t pathSize = 0;
        uint32_t entrySize = 0;
        if(!is_last_item) {
          entrySize = 8 + sizeof(header) + aligned_str->get_size();
        }

        stream.write((char *)&pathSize, sizeof(pathSize));
        stream.write((char *)&entrySize, sizeof(entrySize));

        spdlog::debug("EDat::buildTrie {:02X} path {} pathSize {:02X} entrySize {:02X}", (size_t)stream.tellp(), current_path, pathSize, entrySize);

        stream.write((char *)&header, sizeof(header));

        aligned_str->build(stream);

        is_last = true;
      } else {
        // construct path already, so we can get the length
        auto str = etcetera::CString8l::create();
        str->value = part;
        std::replace(str->value.begin(), str->value.end(), '/', '\\');
        auto aligned_str = etcetera::Aligned::create(2, str);

        size_t offset_part = stream.tellp();
        uint32_t pathSize = 8 + aligned_str->get_size();
        // first write empty entrySize
        uint32_t entrySize = 0;
        stream.write((char *)&pathSize, sizeof(pathSize));
        stream.write((char *)&entrySize, sizeof(entrySize));

        spdlog::debug("EDat::buildTrie {:02X} part {} pathSize {:02X} entrySize {:02X} {} {}", (size_t)stream.tellp(), part, pathSize, entrySize, is_last_item, is_last);

        // build path
        aligned_str->build(stream);

        // now write the rest of the tree
        buildTrie(stream, node, path + part, is_last_item);

        // correct entrySize
        if(!is_last_item) {
          size_t end_offset = stream.tellp();
          entrySize = end_offset - offset_part;
          // the 4 is from the pathSize
          stream.seekp(offset_part + 4, std::ios_base::beg);
          stream.write((char *)&entrySize, sizeof(entrySize));
          stream.seekp(end_offset, std::ios_base::beg);
        }
      }
    }
  }

  void build(std::iostream &stream) override {
    edat_header = {};
    strcpy((char *)edat_header.magic, "edat");
    edat_header.unk0 = 2;
    edat_header.sectorSize = sectorSize;
    edat_header.offset_dictionary = sizeof(edat_header);
    stream.seekp(sizeof(edat_header), std::ios_base::beg);

    uint32_t empty = 0;
    if(file_headers.empty()) {
      empty = 0x01;
    } else {
      empty = 0x0A;
    }
    stream.write((char *)&empty, sizeof(empty));
    uint8_t pad[6] = {0};
    stream.write((char *)pad, sizeof(pad));

    // get all filepaths
    std::vector<std::string> paths;
    for(auto &[path, _] : file_headers) {
      paths.push_back(path);
    }

    // sort all file paths
    std::sort(paths.begin(), paths.end(), sorter);

    // update all file headers (files possibly changed)
    // recalculate offsets, sizes and checksums
    if(read_files) {
      size_t current_file_offset = 0;
      for(auto &path : paths) {
        // update the file offset
        auto &file_header = file_headers[path];
        file_header.offset = current_file_offset;
        // get the file path based on the current outpath
        auto filepath = std::filesystem::path(outpath) / std::filesystem::path(path);
        // calculate the filesize modulo sectorSize
        file_header.size = std::filesystem::file_size(filepath);
        //  and update the offset
        if(file_header.size % sectorSize != 0) {
          current_file_offset += (int(file_header.size / sectorSize) + 1) * sectorSize;
        } else {
          current_file_offset += file_header.size;
        }

        // calculate the md5 sum of the file
        std::ifstream file(filepath, std::ios::binary);
        Chocobo1::MD5 md5;
        size_t remaining = file_header.size;
        std::vector<char> buf;
        buf.resize(sectorSize);
        while(remaining != 0) {
          std::fill(buf.begin(), buf.end(), 0);
          size_t size = std::min(remaining, (size_t)sectorSize);
          file.read(buf.data(), (long)size);
          md5.addData(buf.data(), (long)size);
          remaining -= size;
        }
        auto result = md5.finalize().toVector();
        for(size_t i = 0; i < 16; i++) {
          file_header.checksum[i] = result[i];
        }
      }
    }

    // now build the trie
    Trie trie;
    for(auto &path : paths) {
      trie.insert(path);
    }
    size_t offset_trie = stream.tellp();
    buildTrie(stream, &trie, "");
    size_t end_offset_trie = stream.tellp();
    edat_header.size_dictionary = end_offset_trie - offset_trie + 10;

    // now we are at the start of the files
    edat_header.offset_files = stream.tellp();
    stream.write((char *)&pad, sizeof(pad));

    // first update the dictionary checksum
    {
      stream.seekg(edat_header.offset_dictionary, std::ios_base::beg);
      size_t remaining = edat_header.size_dictionary;
      while (remaining != 0) {
        char buf[4096];
        size_t toRead = std::min(remaining, (size_t)sizeof(buf));
        stream.read(buf, toRead);
        dictionary_md5.addData(buf, toRead);
        remaining -= toRead;
      }
      auto result = dictionary_md5.finalize().toVector();
      for (size_t i = 0; i < 16; i++) {
        edat_header.checksum[i] = result[i];
      }
    }
    spdlog::debug("EDat::build {:02X} EDatHeader", (size_t)stream.tellp());

    stream.seekp(edat_header.offset_files, std::ios_base::beg);

    if(edat_header.offset_files % sectorSize != 0) {
      edat_header.offset_files = (int(edat_header.offset_files / sectorSize) + 1) * sectorSize;
    }

    // now write files
    std::vector<char> buf;
    buf.resize(sectorSize);
    for(auto &path : paths) {
      auto &file_header = file_headers[path];
      stream.seekp(edat_header.offset_files + file_header.offset, std::ios_base::beg);
      auto filepath = std::filesystem::path(outpath) / std::filesystem::path(path);
      std::ifstream file(filepath, std::ios::binary);
      size_t remaining = file_header.size;
      while(remaining > 0) {
        std::fill(buf.begin(), buf.end(), 0);
        size_t toRead = std::min(remaining, (size_t)sectorSize);
        file.read(buf.data(), toRead);
        stream.write(buf.data(), buf.size());
        remaining -= toRead;
      }
      etcetera::custom_assert(remaining == 0);
    }
    edat_header.size_files = (size_t)stream.tellp() - edat_header.offset_files;

    // finally update the header
    spdlog::debug("EDat::build {:02X} EDatHeader", (size_t)stream.tellp());
    stream.seekp(0, std::ios_base::beg);
    stream.write((char *)&edat_header, sizeof(edat_header));
  }

  void parse_xml(pugi::xml_node const &node, std::string name, bool is_root) override {
    auto n = node;
    if(!is_root){
      n = node.child(name.c_str());
    }
    spdlog::debug("EDat::parse_xml name {}", name);
    sectorSize = n.attribute("sectorSize").as_uint();
    spdlog::debug("EDat::parse_xml sectorSize {}", sectorSize);
    for(auto file : n.children("File")) {
      std::string path = file.attribute("path").as_string();
      EDatFileHeader header = {};
      if(!read_files) {
        file.attribute("offset").as_uint(header.offset);
        file.attribute("pad0").as_uint(header.pad0);
        file.attribute("size").as_uint(header.size);
        file.attribute("pad").as_uint(header.pad);
        std::string checksum = file.attribute("checksum").as_string();
        etcetera::custom_assert(checksum.size() == 32);
        for(size_t i = 0; i < 16; i++) {
          std::string byte = checksum.substr(i * 2, 2);
          header.checksum[i] = std::stoi(byte, nullptr, 16);
        }
      }
      file_headers.emplace(path, header);
    }
  }

  pugi::xml_node build_xml(pugi::xml_node &parent, std::string name) override {
    auto root = parent.append_child(name.c_str());
    root.append_attribute("sectorSize").set_value(sectorSize);
    for(auto &[path, header] : file_headers) {
      auto file = root.append_child("File");
      file.append_attribute("path").set_value(path.c_str());
      if(!read_files) {
        file.append_attribute("offset").set_value(header.offset);
        file.append_attribute("pad0").set_value(header.pad0);
        file.append_attribute("size").set_value(header.size);
        file.append_attribute("pad").set_value(header.pad);
        // checksum as hex
        std::stringstream ss;
        for (unsigned char i : header.checksum) {
          ss << std::hex << std::setw(2) << std::setfill('0') << (int)i;
        }
        file.append_attribute("checksum").set_value(ss.str().c_str());
      }
    }
    return root;
  }

};
