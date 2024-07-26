cmake_minimum_required(VERSION 3.22...3.25)
project(wgrd-tools LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -Wextra -g -O0 -DFORTIFY_SOURCE=2 -D_GLIBCXX_ASSERTIONS -fasynchronous-unwind-tables -fexceptions -fstack-clash-protection -fstack-protector-all -fstack-protector-strong -grecord-gcc-switches -fcf-protection -pipe -Werror=format-security")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wall -Wextra -O3 -Werror=format-security -g")
endif()

set(VERSION_MAJOR 0)
set(VERSION_MINOR 1)
set(VERSION_PATCH 0)
set(VERSION_STRING "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")

#add_subdirectory(deps/etceterapp)

add_subdirectory(deps/argparse)
add_subdirectory(deps/spdlog)
add_subdirectory(deps/fmt)
add_subdirectory(deps/Catch2)
add_subdirectory(deps/pugixml)

# add version information
find_package(Git)
if (GIT_FOUND)
  execute_process(COMMAND ${GIT_EXECUTABLE} rev-parse HEAD OUTPUT_VARIABLE VERSION_GIT_COMMIT OUTPUT_STRIP_TRAILING_WHITESPACE)
  execute_process(COMMAND ${GIT_EXECUTABLE} rev-parse --short HEAD OUTPUT_VARIABLE VERSION_GIT_SHORT OUTPUT_STRIP_TRAILING_WHITESPACE)
  execute_process(COMMAND ${GIT_EXECUTABLE} branch --show-current OUTPUT_VARIABLE VERSION_GIT_BRANCH OUTPUT_STRIP_TRAILING_WHITESPACE)
  execute_process(COMMAND ${GIT_EXECUTABLE} describe --tags OUTPUT_VARIABLE VERSION_GIT_TAG OUTPUT_STRIP_TRAILING_WHITESPACE)
else ()
  set(VERSION_GIT "unknown")
  set(VERSION_GIT_SHORT "unknown")
  set(VERSION_GIT_BRANCH "unknown")
  set(VERSION_GIT_TAG "unknown")
endif ()

add_library(ndf src/ndf.cpp src/ndf_bin.cpp)
target_link_libraries(ndf PUBLIC argparse::argparse spdlog::spdlog fmt::fmt pugixml::pugixml)
target_include_directories(ndf PUBLIC .)
add_executable(ndf_helper
        src/ndf_helper.cpp
)
target_link_libraries(ndf_helper PRIVATE ndf)

add_executable(ndfbin_helper
        src/ndfbin_helper.cpp
)
target_link_libraries(ndfbin_helper PRIVATE ndf)

#add_executable(ndfbin src/ndfbin.cpp)
#target_link_libraries(ndfbin PRIVATE etceterapp)
#
#add_executable(edat
#        src/edat.cpp
#)
#target_link_libraries(edat PRIVATE etceterapp)
#
#add_executable(edat_helper
#        src/edat_helper.cpp
#)
#target_link_libraries(edat_helper PRIVATE etceterapp)

