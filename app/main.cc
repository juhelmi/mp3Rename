 
#include <iostream>
#include <fstream>
#include <filesystem>
//#include <format>
#include <boost/format.hpp>




#include <nlohmann/json.hpp>

#include "config.hpp"
#include "my_lib.h"

using namespace std;
namespace fs = std::filesystem;

bool debug_run = false;

void print_directory_data(ofstream& o, std::vector<pair<int, string>> &dir_data, string dir_name)
{
    if (dir_data.size() > 0) {
        auto& p0 = dir_data[0];
        if (dir_data.size() == 1) {
            o <<      boost::format("Dir: %1$d ")%p0.first << dir_name << endl;
            cout << boost::format("Dir: %1$d ")%p0.first << dir_name << endl;
        } else {
            auto& p_last = dir_data.back();
            o <<      boost::format("Dir: %1$d-%2$d ")%p0.first%p_last.first << dir_name << endl;
            cout << boost::format("Dir: %1$d-%2$d ")%p0.first%p_last.first << dir_name << endl;
        }
    }
    for (auto& p : dir_data) {
        o <<      boost::format("%1$5d: ")% p.first << p.second << endl;
        cout << boost::format("%1$5d: ")% p.first << p.second << endl;
    }
    dir_data.clear();
}

void generate_file_index_for_files(rename_map_t& copied_files, std::string output_name)
{
    string prev_dir = "/dev/null";      // some name that was not used
    int running_index = 1;
    ofstream o;
    std::vector<pair<int, string>> dir_data = {};
    string current_dir = "/dev/null";

    o.open(output_name);
    for (auto& [k, v] : copied_files) {
        std::filesystem::path p = v;
        current_dir = p.parent_path();
        if (current_dir != prev_dir) {
            // print collected data for prev dir
            print_directory_data(o, dir_data, prev_dir);
            prev_dir = current_dir;
            //dir_data.clear();
        }
        dir_data.push_back(make_pair(running_index, p.filename()));
        running_index++;
    }
    // Last directory needs to be printed also
    print_directory_data(o, dir_data, current_dir);
    o.close();
}

void create_directory_and_parents(std::filesystem::path p)
{
    auto parent = p.parent_path();
    if (! fs::exists(parent)) {
        create_directory_and_parents(parent);
    }
    fs::create_directory(p);
}

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
        rename_map_t map_of_files;

        get_new_filename_list_from_mp3_tags(argc-1, argv+1, map_of_files);

        cout << "Got " << map_of_files.size() << " files to rename on copy.\n";
        cout << "Current path is " << fs::current_path() << endl;

        string target_root = "../copy_audio/";
        if (! fs::exists(target_root)) {
            cout << "Creates root directory " << target_root << endl;
            fs::create_directory(target_root);
        }

        int i = 0;
        for (auto& [k, v] : map_of_files) {
            // create first directory if it do not exist
            std::filesystem::path p = target_root + v;
            try {

                if (! fs::exists(p.parent_path())) {
                    cout << "Creates parent path " << p.parent_path() << endl;
                    create_directory_and_parents(p.parent_path());
                }
                if (debug_run) {
                    cout << "Not copied in debug mode, file " << k << endl;
                } else {
                    if (! fs::exists(p)) {
                        cout << "Copying " << k << "\nto " << p << "\n";
                        fs::copy_file(k, p);
                    } else {
                        cout << "File " << p << " already exists, not copied\n";
                    }
                }
            }
            catch (fs::filesystem_error& e)
            {
                cout << "Could not copy to: " << target_root+v << " reason " << e.what() << '\n';
            }
            if (false && debug_run && (++i > 10) ) {
                cout << "Stopped copying\n";
                break;
            }
        }
        // generate file index
        generate_file_index_for_files(map_of_files, target_root + "file_index.txt");
    }

    return 0;
}
