 
#include <iostream>

#include <nlohmann/json.hpp>

#include "config.hpp"
#include "my_lib.h"

using namespace std;

int main(int argc, char *argv[])
{

    if (argc <= 1)
    {
        std::cout << project_name << '\n';
        std::cout << project_version << '\n';

        std::cout << "JSON Lib Version:" << NLOHMANN_JSON_VERSION_MAJOR << "."
                << NLOHMANN_JSON_VERSION_MINOR << "."
                << NLOHMANN_JSON_VERSION_PATCH << "\n";

        std::cout << "Se toimii!\n";
        std::cout << "No files given as parameter\n";
    } else {
        setlocale(LC_ALL, "en_US.UTF-8");
        //setlocale(LC_ALL, "fi_FI.UTF-8");
        rename_map_t map_of_files = print_tags_for_files(argc-1, argv+1);
        cout << "Got " << map_of_files.size() << " files to rename on copy.\n";
        for (auto& [k, v] : map_of_files) {
        }
    }

    return 0;
}
