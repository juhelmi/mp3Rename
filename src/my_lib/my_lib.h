#pragma once

#include <map>
#include <string>

typedef  std::map<std::string, std::string> rename_map_t;

void get_new_filename_list_from_mp3_tags(int argc, char **argv, rename_map_t& result);

