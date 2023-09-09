cmake_minimum_required(VERSION 3.16)

project(mp3IndexAndRename VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD          17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS        OFF)

set(CHAR_CONVERSION_FILE    "char_conversion.json")

# BOOST_ROOT=path
#set(BOOST_ROOT /home/juha/work/Cpp/mp3Rename/build/boost_root)

set(LIBRARY_NAME Library)
set(EXECUTABLE_NAME mp3Index)

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake/")
include(AddGitSubmodule)

# Debug in KDevelop
set(CMAKE_BUILD_TYPE Debug)

# https://stackoverflow.com/questions/68324093/using-taglib-in-cmake-project-without-installing-it

# taglib
# https://github.com/taglib/taglib
# git@github.com:taglib/taglib.git
# cd external; git submodule add git@github.com:nlohmann/json.git

add_git_submodule(external/json)
add_git_submodule(external/taglib)
#add_git_submodule(external/boost)

# https://cmake.org/cmake/help/git-master/module/FindBoost.html
# Use followin lines when boost is OS library
FIND_PACKAGE( Boost 1.83 COMPONENTS program_options REQUIRED )
# FIND_PACKAGE( Boost 1.70 COMPONENTS program_options REQUIRED )
INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIR} )

add_subdirectory(configured)
add_subdirectory(external)
add_subdirectory(src)
add_subdirectory(app)


