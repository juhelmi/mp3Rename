#pragma once

#include <map>
#include <string>

typedef  std::map<std::string, std::string> rename_map_t;

rename_map_t print_tags_for_files(int argc, char **argv);

