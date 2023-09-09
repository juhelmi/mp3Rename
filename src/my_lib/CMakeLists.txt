set(LIBRARY_SOURCES
    "my_lib.cc"
    "mp3Tags.cpp"
    "mp3CharConversion.cpp")
set(LIBRARY_HEADERS
    "my_lib.h"
    "mp3Tags.hpp"
    "utf8.h"     "utf8/core.h"
    "utf8/checked.h"
    "utf8/unchecked.h"
    "mp3CharConversion.hpp" )

add_library(${LIBRARY_NAME} STATIC
    ${LIBRARY_SOURCES}
    ${LIBRARY_HEADERS})
target_include_directories(${LIBRARY_NAME} PUBLIC
    "./"
    "${CMAKE_BINARY_DIR}/configured_files/include")

# Taglib include directories
include_directories("${CMAKE_SOURCE_DIR}/external/taglib/taglib"
  "${CMAKE_SOURCE_DIR}/external/taglib/taglib/toolkit"
  "${CMAKE_SOURCE_DIR}/build/external/taglib"
  )

# Pointer to json include also
include_directories(  "${CMAKE_SOURCE_DIR}/external/json/include")
