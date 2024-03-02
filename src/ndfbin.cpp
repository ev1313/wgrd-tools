#include "array.hpp"
#include "basic.hpp"
#include "conditional.hpp"
#include "helpers.hpp"
#include "number.hpp"
#include "pointer.hpp"
#include "special.hpp"
#include "string.hpp"
#include "struct.hpp"

#include "argparse/argparse.hpp"

#include <fstream>
#include <iostream>

using namespace etcetera;
using namespace std::string_literals;

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

  using NDFField = Switch<uint32_t>::SwitchField;
  auto NDFType = []() { return LazyBound::create([](std::weak_ptr<LazyBound> p) {
    //spdlog::info("NDFType");
    return Struct::create(
        Field("typeId", Int32ul::create()),
        Field(
            "data",
            Switch<uint32_t>::create(
                [](std::weak_ptr<Base> c) -> uint32_t {
                  return lock(c)->get<uint32_t>("typeId");
                },
                NDFField(0x00000000, "Boolean",
                         [p]() {
                           return Struct::create(
                               Field("value", Int8ul::create()));
                         }),
                NDFField(0x00000001, "Int8",
                         [p]() {
                           return Struct::create(
                               Field("value", Int8ul::create()));
                         }),
                NDFField(0x00000002, "Int32",
                         [p]() {
                           return Struct::create(
                               Field("value", Int32sl::create()));
                         }),
                NDFField(0x00000003, "UInt32",
                         [p]() {
                           return Struct::create(
                               Field("value", Int32ul::create()));
                         }),
                NDFField(0x00000004, "Unk0x4",
                         [p]() {
                           return Struct::create(
                               Field("value", Bytes::create(8)));
                         }),
                NDFField(0x00000005, "Float32",
                         [p]() {
                           return Struct::create(
                               Field("value", Float32l::create()));
                         }),
                NDFField(0x00000006, "Float64",
                         [p]() {
                           return Struct::create(
                               Field("value", Float64l::create()));
                         }),
                NDFField(0x00000007, "StringReference",
                         [p]() {
                           return Struct::create(
                               Field("stringIndex", Int32ul::create()));
                         }),
                NDFField(0x00000008, "WideString",
                         [p]() {
                           return Struct::create(
                               Field("str",
                                     PascalString16l<Int32ul>::create(
                                           Int32ul::create()
                                         )//
                                     )//
                               );
                         }),
                NDFField(
                    0x00000009, "Reference",
                    [p]() {
                      return Struct::create(
                          Field("typeId", Int32ul::create()),
                          Field("ref",
                                Switch<uint32_t>::create(
                                    [](std::weak_ptr<Base> c) -> uint32_t {
                                      return lock(c)->get<uint32_t>("typeId");
                                    },
                                    NDFField(0xAAAAAAAA, "TranReference",
                                             [p]() {
                                               return Struct::create(
                                                   Field("tranIndex",
                                                         Int32ul::create()));
                                             }),
                                    NDFField(0xBBBBBBBB, "ObjectReference",
                                             [p]() {
                                               return Struct::create(
                                                   Field("objectIndex",
                                                         Int32ul::create()),
                                                   Field("classIndex",
                                                         Int32ul::create()));
                                             }))));
                    }),
                NDFField(0x0000000B, "F32_vec3",
                         [p]() {
                           return Struct::create(
                               Field("x", Float32l::create()),
                               Field("y", Float32l::create()),
                               Field("z", Float32l::create()));
                         }),
                NDFField(0x0000000C, "F32_vec4",
                         [p]() {
                           return Struct::create(
                               Field("x", Float32l::create()),
                               Field("y", Float32l::create()),
                               Field("z", Float32l::create()),
                               Field("w", Float32l::create()));
                         }),
                NDFField(0x0000000D, "Color",
                         [p]() {
                           return Struct::create(Field("r", Int8ul::create()),
                                                 Field("g", Int8ul::create()),
                                                 Field("b", Int8ul::create()),
                                                 Field("a", Int8ul::create()));
                         }),
                NDFField(0x0000000E, "S32_vec3",
                         [p]() {
                           return Struct::create(Field("x", Int32sl::create()),
                                                 Field("y", Int32sl::create()),
                                                 Field("z", Int32sl::create()));
                         }),
                NDFField(0x0000000F, "Matrix",
                         [p]() {
                           return Struct::create(
                               Field("Matrix", Array::create(16, []() {
                                 return Float32l::create();
                               })));
                         }),
                NDFField(0x00000011, "List",
                         [p]() {
                           return Struct::create(
                               Field("length",
                                     Rebuild::create(
                                         [](std::weak_ptr<Base> c) -> uint32_t {
                                           return lock(lock(c)->get_field(
                                               "items"))
                                               ->length();
                                         },
                                         Int32ul::create())),
                               Field("items",
                                     Array::create(
                                         [](std::weak_ptr<Base> c) {
                                           return lock(c)->get_parsed<uint32_t>(
                                               "length");
                                         },
                                         [p]() {
                                           return LazyBound::create(
                                               lock(p)->get_lazy_fn());
                                         })));
                         }),
                NDFField(
                    0x00000012, "Map",
                    [p]() {
                      return Struct::create(
                          Field("count",
                                Rebuild::create(
                                    [](std::weak_ptr<Base> c) -> uint32_t {
                                      return lock(
                                          lock(c)->get_field("mapitems"))
                                          ->length();
                                    },
                                    Int32ul::create())),
                          Field("mapitems",
                                Array::create(
                                    [](std::weak_ptr<Base> c) {
                                      return lock(c)->get_parsed<uint32_t>(
                                          "count");
                                    },
                                    [p]() {
                                      return Struct::create(
                                          Field("key",
                                                LazyBound::create(
                                                    lock(p)->get_lazy_fn())),
                                          Field("value",
                                                LazyBound::create(
                                                    lock(p)->get_lazy_fn())));
                                    })));
                    }),
                NDFField(0x00000013, "Long",
                         [p]() {
                           return Struct::create(
                               Field("value", Int64ul::create()));
                         }),
                NDFField(0x00000014, "Blob",
                         [p]() {
                           return Struct::create(
                               Field("size", Int32ul::create()),
                               Field("data",
                                     Bytes::create([](std::weak_ptr<Base> c) {
                                       return lock(c)->get<uint32_t>("size");
                                     })));
                         }),
                NDFField(0x00000018, "S16",
                         [p]() {
                           return Struct::create(
                               Field("value", Int16sl::create()));
                         }),
                NDFField(0x00000019, "U16",
                         [p]() {
                           return Struct::create(
                               Field("value", Int16ul::create()));
                         }),
                NDFField(0x0000001A, "GUID",
                         [p]() {
                           return Struct::create(
                               Field("data", Bytes::create(16)));
                         }),
                NDFField(0x0000001C, "PathReference",
                         [p]() {
                           return Struct::create(
                               Field("stringIndex", Int32ul::create()));
                         }),
                NDFField(0x0000001D, "LocalisationHash",
                         [p]() {
                           return Struct::create(
                               Field("data", Bytes::create(8)));
                         }),
                NDFField(0x0000001F, "S32_vec2",
                         [p]() {
                           return Struct::create(Field("x", Int32sl::create()),
                                                 Field("y", Int32sl::create()));
                         }),
                NDFField(0x00000021, "F32_vec2",
                         [p]() {
                           return Struct::create(
                               Field("x", Float32l::create()),
                               Field("y", Float32l::create()));
                         }),
                NDFField(0x00000022, "Pair",
                         [p]() {
                           return Struct::create(
                               Field("first",
                                     LazyBound::create(lock(p)->get_lazy_fn())),
                               Field("second", LazyBound::create(
                                   lock(p)->get_lazy_fn())));
                         }),
                NDFField(0x00000025, "Hash",
                         [p]() {
                           return Struct::create(
                               Field("hash", Bytes::create(16)));
                         }) //
            )));
  });
  };

  auto NDFProperty = [NDFType]() {
    return Struct::create(Field("propertyIndex", Int32ul::create()),
                          Field("NDFType", IfThenElse::create(
                              [](std::weak_ptr<Base> c) {
                                return lock(c)->get<uint32_t>(
                                    "propertyIndex") !=
                                    0xABABABABU;
                              },
                              Field("NDFType", NDFType())//
                          )));
  };

  auto NDFObject = [NDFProperty]() {
    return Struct::create(
        Field("classIndex", Int32ul::create()),
        Field("Property",
              RepeatUntil::create(
                  [](std::weak_ptr<Base> obj, std::weak_ptr<Base>) -> bool {
                    return lock(obj)->get<uint32_t>("propertyIndex") ==
                        0xABABABABU;
                  },
                  NDFProperty)));
  };

  auto OBJETable = Struct::create(
      Field("magic", BytesConst::create("OBJE"s)),
      Field("pad0", BytesConst::create("\x00\x00\x00\x00"s)),
      Field("offset", Rebuild::create(
          [](std::weak_ptr<Base> c) {
            return std::make_any<uint32_t>(
                lock(c)->get_parsed<uint32_t>("_", "_", "headerSize"));
          },
          Int32ul::create())),
      Field("pad1", BytesConst::create("\x00\x00\x00\x00"s)),
      Field("size", Rebuild::create(
          [](std::weak_ptr<Base> c) {
            return std::make_any<uint32_t>(
                lock(c)->get_field<Area>("Object").lock()->get_ptr_size({}));
          },
          Int32ul::create())),
      Field("pad2", BytesConst::create("\x00\x00\x00\x00"s)),
      Field("Object", Area::create(
          [](std::weak_ptr<Base> c) {
            return lock(c)->get_parsed<uint32_t>("offset");
          },
          [](std::weak_ptr<Base> c) {
            return lock(c)->get_parsed<uint32_t>("size");
          },
          NDFObject)));

  auto TOPOTable = Struct::create(
      Field("magic", BytesConst::create("TOPO"s)),
      Field("pad0", BytesConst::create("\x00\x00\x00\x00"s)),
      Field("offset", Rebuild::create(
          [](std::weak_ptr<Base> c) {
            return std::make_any<uint32_t>(
                lock(c)->get<uint32_t>("_", "OBJE", "offset") + lock(c)->get<uint32_t>("_", "OBJE", "size")
                    );
          },
          Int32ul::create())),
      Field("pad1", BytesConst::create("\x00\x00\x00\x00"s)),
      Field("size", Rebuild::create(
          [](std::weak_ptr<Base> c) {
            return std::make_any<uint32_t>(
                lock(c)->get_field<Area>("TOPObject").lock()->get_ptr_size({}));
          },
          Int32ul::create())),
      Field("pad2", BytesConst::create("\x00\x00\x00\x00"s)),
      Field("TOPObject", Area::create(
          [](std::weak_ptr<Base> c) {
            return lock(c)->get_parsed<uint32_t>("offset");
          },
          [](std::weak_ptr<Base> c) {
            return lock(c)->get_parsed<uint32_t>("size");
          },
          []() { return Struct::create(Field("objectIndex", Int32ul::create())); }//
      )));

  auto CHNKTable = Struct::create(
      Field("magic", BytesConst::create("CHNK"s)),
      Field("pad0", BytesConst::create("\x00\x00\x00\x00"s)),
      Field("offset", Rebuild::create(
          [](std::weak_ptr<Base> c) {
            return std::make_any<uint32_t>(
            lock(c)->get<uint32_t>("_", "TOPO", "offset") + lock(c)->get<uint32_t>("_", "TOPO", "size"));
          },
          Int32ul::create())),
      Field("pad1", BytesConst::create("\x00\x00\x00\x00"s)),
      Field("size", Rebuild::create(
          [](std::weak_ptr<Base> c) {
            return std::make_any<uint32_t>(
                lock(c)->get_field<Area>("Chunk").lock()->get_ptr_size({}));
          },
          Int32ul::create())),
      Field("pad2", BytesConst::create("\x00\x00\x00\x00"s)),
      Field("Chunk", Area::create(
          [](std::weak_ptr<Base> c) {
            return lock(c)->get_parsed<uint32_t>("offset");
          },
          [](std::weak_ptr<Base> c) {
            return lock(c)->get_parsed<uint32_t>("size");
          },
          []() {
            return Struct::create(
                Field("unk0", Int32ul::create()),
                Field("objectCount", Int32ul::create())//
            );
          }//
      )));

  auto CLASTable = Struct::create(
      Field("magic", BytesConst::create("CLAS"s)),
      Field("pad0", BytesConst::create("\x00\x00\x00\x00"s)),
      Field("offset", Rebuild::create(
          [](std::weak_ptr<Base> c) {
            return std::make_any<uint32_t>(
            lock(c)->get<uint32_t>("_", "CHNK", "offset") + lock(c)->get<uint32_t>("_", "CHNK", "size"));
          },
          Int32ul::create())),
      Field("pad1", BytesConst::create("\x00\x00\x00\x00"s)),
      Field("size", Rebuild::create(
          [](std::weak_ptr<Base> c) {
            return std::make_any<uint32_t>(
                lock(c)->get_field<Area>("Class").lock()->get_ptr_size({}));
          },
          Int32ul::create())),
      Field("pad2", BytesConst::create("\x00\x00\x00\x00"s)),
      Field("Class", Area::create(
          [](std::weak_ptr<Base> c) {
            return lock(c)->get_parsed<uint32_t>("offset");
          },
          [](std::weak_ptr<Base> c) {
            return lock(c)->get_parsed<uint32_t>("size");
          },
          []() {
            return Struct::create(
                Field("str", PascalString8l<Int32ul>::create(
                      Int32ul::create())
                )//
            );
          }//
      )));

  auto PROPTable = Struct::create(
      Field("magic", BytesConst::create("PROP"s)),
      Field("pad0", BytesConst::create("\x00\x00\x00\x00"s)),
      Field("offset", Rebuild::create(
          [](std::weak_ptr<Base> c) {
            return std::make_any<uint32_t>(
                lock(c)->get<uint32_t>("_", "CLAS", "offset") + lock(c)->get<uint32_t>("_", "CLAS", "size"));
          },
          Int32ul::create())),
      Field("pad1", BytesConst::create("\x00\x00\x00\x00"s)),
      Field("size", Rebuild::create(
          [](std::weak_ptr<Base> c) {
            return std::make_any<uint32_t>(
                lock(c)->get_field<Area>("Property").lock()->get_ptr_size({}));
          },
          Int32ul::create())),
      Field("pad2", BytesConst::create("\x00\x00\x00\x00"s)),
      Field("Property", Area::create(
          [](std::weak_ptr<Base> c) {
            return lock(c)->get_parsed<uint32_t>("offset");
          },
          [](std::weak_ptr<Base> c) {
            return lock(c)->get_parsed<uint32_t>("size");
          },
          []() {
            return Struct::create(
                Field("str", PascalString8l<Int32ul>::create(
                      Int32ul::create())
                ),
                Field("classIndex", Int32ul::create())//
            );
          }//
      )));

  auto STRGTable = Struct::create(
      Field("magic", BytesConst::create("STRG"s)),
      Field("pad0", BytesConst::create("\x00\x00\x00\x00"s)),
      Field("offset", Rebuild::create(
          [](std::weak_ptr<Base> c) {
            return std::make_any<uint32_t>(
                lock(c)->get<uint32_t>("_", "PROP", "offset") + lock(c)->get<uint32_t>("_", "PROP", "size"));
          },
          Int32ul::create())),
      Field("pad1", BytesConst::create("\x00\x00\x00\x00"s)),
      Field("size", Rebuild::create(
          [](std::weak_ptr<Base> c) {
            return std::make_any<uint32_t>(
                lock(c)->get_field<Area>("String").lock()->get_ptr_size({}));
          },
          Int32ul::create())),
      Field("pad2", BytesConst::create("\x00\x00\x00\x00"s)),
      Field("String", Area::create(
          [](std::weak_ptr<Base> c) {
            return lock(c)->get_parsed<uint32_t>("offset");
          },
          [](std::weak_ptr<Base> c) {
            return lock(c)->get_parsed<uint32_t>("size");
          },
          []() {
            return Struct::create(
                Field("str", PascalString8l<Int32ul>::create(
                    Int32ul::create())
                )//
            );
          }//
      )));

  auto TRANTable = Struct::create(
      Field("magic", BytesConst::create("TRAN"s)),
      Field("pad0", BytesConst::create("\x00\x00\x00\x00"s)),
      Field("offset", Rebuild::create(
          [](std::weak_ptr<Base> c) {
            return std::make_any<uint32_t>(
                lock(c)->get<uint32_t>("_", "STRG", "offset") + lock(c)->get<uint32_t>("_", "STRG", "size"));
          },
          Int32ul::create())),
      Field("pad1", BytesConst::create("\x00\x00\x00\x00"s)),
      Field("size", Rebuild::create(
          [](std::weak_ptr<Base> c) {
            return std::make_any<uint32_t>(
                lock(c)->get_field<Area>("Tran").lock()->get_ptr_size({}));
          },
          Int32ul::create())),
      Field("pad2", BytesConst::create("\x00\x00\x00\x00"s)),
      Field("Tran", Area::create(
          [](std::weak_ptr<Base> c) {
            return lock(c)->get_parsed<uint32_t>("offset");
          },
          [](std::weak_ptr<Base> c) {
            return lock(c)->get_parsed<uint32_t>("size");
          },
          []() {
            return Struct::create(
                Field("str", PascalString8l<Int32ul>::create(
                    Int32ul::create())
                )//
            );
          }//
      )));

  auto IMPR = []() { return LazyBound::create([](std::weak_ptr<LazyBound> p) {
    return Struct::create(
      Field("tranIndex", Int32ul::create()),
      Field("objectIndex", Int32ul::create()),
      Field("count", Rebuild::create(
        [](std::weak_ptr<Base> c) {
          return std::make_any<uint32_t>(
            lock(c)->get_field<Array>("imprs").lock()->length());
        },
        Int32ul::create())),
      Field("improffsets", Rebuild::create(
        [](std::weak_ptr<Base> c) {
          std::vector<std::any> offsets;
          size_t count = lock(c)->get<uint32_t>("count");
          size_t base_offset = lock(c)->get_offset("count");

          for(size_t i = 0; i < count; i++) {
            uint32_t offset = 4*(count-1) + lock(c)->get_field<Array>("imprs").lock()->get_offset(i) - base_offset;
            offsets.push_back(std::make_any<uint32_t>(offset));
            spdlog::debug("improffsets: {} {}", i, offset);
          }
          spdlog::debug("improffsets assert: {} {}", offsets.size(), count);
          custom_assert(offsets.size() == count);
          return std::make_any<std::vector<std::any>>(offsets);
        },
        Array::create(
          [](std::weak_ptr<Base> c) {
            return lock(c)->get_parsed<uint32_t>("count");
          },
          []() { return Int32ul::create(); })
      )), //
      Field("imprs", 
        Array::create(
          [](std::weak_ptr<Base> c) {
            return lock(c)->get_parsed<uint32_t>("count");
          },
          [p]() { 
            return LazyBound::create(lock(p)->get_lazy_fn()); 
          }
        )//
      )//
    );
  });
  };
/*
      IMPR = Struct(
"tranIndex" / Int32ul,
"objectIndex" / Int32ul,
"count" / Rebuild(Int32ul, lambda ctx: len(ctx.imprs)),
"_impr_size" / Computed(lambda ctx: 12 + 4 * ctx.count + sum([x._impr_size for x in ctx.imprs]) if not ctx._parsing else 0),
# sizes of following IMPR
"improffsets" / Rebuild(Array(this.count, Int32ul), lambda ctx: [4*ctx.count +
                                                               sum([x._impr_size for x in ctx.imprs[0:i]])
                                                               for i in range(ctx.count) if ctx.count > 0
                                                              ]),
"imprs" / Array(this.count, LazyBound(lambda: IMPR)),
)
*/

  auto IMPRTable = Struct::create(
      Field("magic", BytesConst::create("IMPR"s)),
      Field("pad0", BytesConst::create("\x00\x00\x00\x00"s)),
      Field("offset", Rebuild::create(
          [](std::weak_ptr<Base> c) {
            return std::make_any<uint32_t>(
                lock(c)->get<uint32_t>("_", "TRAN", "offset") + lock(c)->get<uint32_t>("_", "TRAN", "size"));
          },
          Int32ul::create())),
      Field("pad1", BytesConst::create("\x00\x00\x00\x00"s)),
      Field("size", Rebuild::create(
          [](std::weak_ptr<Base> c) {
            return std::make_any<uint32_t>(
                lock(c)->get_field<Area>("Import").lock()->get_ptr_size({}));
          },
          Int32ul::create())),
      Field("pad2", BytesConst::create("\x00\x00\x00\x00"s)),
      Field("Import", Area::create(
          [](std::weak_ptr<Base> c) {
            return lock(c)->get_parsed<uint32_t>("offset");
          },
          [](std::weak_ptr<Base> c) {
            return lock(c)->get_parsed<uint32_t>("size");
          },
          IMPR
      )));

  auto EXPRTable = Struct::create(
      Field("magic", BytesConst::create("EXPR"s)),
      Field("pad0", BytesConst::create("\x00\x00\x00\x00"s)),
      Field("offset", Rebuild::create(
          [](std::weak_ptr<Base> c) {
            return std::make_any<uint32_t>(
                lock(c)->get<uint32_t>("_", "IMPR", "offset") + lock(c)->get<uint32_t>("_", "IMPR", "size"));
          },
          Int32ul::create())),
      Field("pad1", BytesConst::create("\x00\x00\x00\x00"s)),
      Field("size", Rebuild::create(
          [](std::weak_ptr<Base> c) {
            return std::make_any<uint32_t>(
                lock(c)->get_field<Area>("Export").lock()->get_ptr_size({}));
          },
          Int32ul::create())),
      Field("pad2", BytesConst::create("\x00\x00\x00\x00"s)),
      Field("Export", Area::create(
          [](std::weak_ptr<Base> c) {
            return lock(c)->get_parsed<uint32_t>("offset");
          },
          [](std::weak_ptr<Base> c) {
            return lock(c)->get_parsed<uint32_t>("size");
          },
          IMPR
      )));

  auto TOC0Header = Struct::create(
      Field("magic", BytesConst::create("TOC0"s)),
      Field("tableCount", Const<uint32_t>::create(9)), //
      Field("OBJE", OBJETable),
      Field("TOPO", TOPOTable),
      Field("CHNK", CHNKTable),
      Field("CLAS", CLASTable),
      Field("PROP", PROPTable),
      Field("STRG", STRGTable),
      Field("TRAN", TRANTable),
      Field("IMPR", IMPRTable),
      Field("EXPR", EXPRTable)
  );

  auto NdfBin = Struct::create(
      Field("magic", BytesConst::create("EUG0"s)),
      Field("magic2", BytesConst::create("\x00\x00\x00\x00"s)),
      Field("magic3", BytesConst::create("CNDF"s)),
      Field("compressed", Int32ul::create()),
      Field("toc0offset", Int32ul::create()),
      Field("unk0", BytesConst::create("\x00\x00\x00\x00"s)),
      Field("headerSize", Rebuild::create(
          [](std::weak_ptr<Base>) {
            return std::make_any<uint32_t>(40);
          },
          Int32ul::create())),
      Field("unk2", BytesConst::create("\x00\x00\x00\x00"s)),
      Field("size", Int32ul::create()),
      Field("unk4", BytesConst::create("\x00\x00\x00\x00"s)),
      Field("uncompressedSize", Int32ul::create()),
      Field("toc0header", Pointer::create(
          [](std::weak_ptr<Base> c) {
            return lock(c)->get<uint32_t>("toc0offset");
          },
          TOC0Header))//
  );

  if(!program.get<bool>("-p")) {
    std::ifstream input;
    input.open(program.get("input"), std::ios::binary);
    NdfBin->parse(input);

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

    std::fstream ofs(program.get("output") + "/ndfbin.bin", std::fstream::in | std::fstream::out | std::fstream::binary);
    NdfBin->build(ofs);
  }
}
