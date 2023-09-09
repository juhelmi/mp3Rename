#include <iostream>

#include "my_lib.h"
#include "mp3Tags.hpp"

#include <utility>

// from tagreader.cpp
#include <iostream>
#include <iomanip> //VERSION
#include <cstdio>

#include "fileref.h"
#include "tag.h"
#include "tpropertymap.h"

// Unicode feature check
// https://stackoverflow.com/questions/18534494/convert-from-utf-8-to-unicode-c

#include <boost/algorithm/string/classification.hpp> // Include boost::for is_any_of
#include <boost/algorithm/string/split.hpp> // Include for boost::split
#include <bits/stdc++.h>
#include <boost/algorithm/string.hpp>

#include <boost/filesystem.hpp>
#include <filesystem>

#include <map>
#include <list>

#include <bits/stdc++.h>

using namespace std;
namespace fs = std::filesystem;

void print_hello_world()
{
    std::cout << "Heippa maailma!\n";
    // https://en.cppreference.com/w/cpp/filesystem/path
    string separator(1, std::filesystem::path::preferred_separator);
    //string separator(1, boost::filesystem::path::preferred_separator);
    cout << "Path separator is: '" << separator << "'\n";
}

string get_last_folder(string s)
{
    vector<string> folders;
    // string separator = boost::filesystem::path("/").native();
    string separator(1, std::filesystem::path::preferred_separator);
    boost::split(folders, s, boost::is_any_of(separator));
    //for (int i = 0; i < folders.size(); i++)
    //    cout << folders[i] << endl;
    if (folders.size() > 1)
    {
        return folders[folders.size()-2];
    } else {
        return string("");
    }
}

vector< pair<fs::path, Mp3Tags>> get_files_under_directory(fs::path dir, std::string& root)
{
    vector< pair<fs::path, Mp3Tags>> result;

    //std::cout << "Tarkistetaan " << dir.c_str() << std::endl;
    if (!fs::exists(dir))
    {
        return result;
    }
    //if (boost::filesystem::is_regular_file(dir))
    if (fs::is_regular_file(dir))
    {
        //std::cout << "Tiedosto " << dir.c_str() << std::endl;
        pair<fs::path, Mp3Tags> temp;
        temp.first = dir;
        temp.second = Mp3Tags(dir.c_str(), root.length());
        cout << temp.second.getCombinedName() << "\n";
        result.push_back(temp);
    }
    if (fs::is_directory(dir))
    {
        // Get directory listing for dir
        for (auto const& dir_entry : std::filesystem::directory_iterator{dir})
        {
            //std::cout << dir_entry.path() << '\n';
            vector< pair<fs::path, Mp3Tags>> sub = get_files_under_directory(dir_entry, root);
            result.insert(result.end(), sub.begin(), sub.end());
        }
    }
    return result;
}

void print_tags_for_files(int argc, char **argv)
{
    bool extra_print = false;
    fs::path fpath;
    std::map<string, Mp3Tags> m;
    vector<Mp3Tags> skippedFiles;

    if (argc == 1)
    {
        fpath = argv[0];
        std::string root = fpath.string();
        vector< pair<fs::path, Mp3Tags>> paths = get_files_under_directory(fpath, root);
        for (int i = 0; i<paths.size(); i++)
        {
            string s = paths[i].first.c_str();
            m.insert({s, paths[i].second});
            Mp3Tags* mp3 = &(paths[i].second);
            //std::cout << "Polku " << paths[i].first << "Title:" << mp3->getTitle() << " " <<mp3->getSeconds() << "s\n";
        }
        int mp3Count = 0;
        for (auto& [k, v] : m) {
            if (v.isMp3())
            {
                std::cout  << v.toString() << std::endl;
                mp3Count++;
            } else {
                // std::cout << "Skips " << v.getPath() << std::endl;
                skippedFiles.insert(skippedFiles.end(), v);
            }
        }
        for (auto& v: skippedFiles)
        {
            std::cout << "Skips " << v.toString() << std::endl;
            m.erase(v.getFullpath());
        }
        std::cout << "Mp3 count " << mp3Count << " Skipped " << skippedFiles.size() << " Total " << m.size()+skippedFiles.size() << "\n";
    }

    return ;
}
