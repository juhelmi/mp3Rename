#include "mp3Tags.hpp"

#include "fileref.h"
#include "tag.h"
#include "tpropertymap.h"

// Taglib use taken from taglib examples, tagreader.cpp
// Taglib uses
#include <iostream>
#include <iomanip> //VERSION
#include <cstdio>

#include <string>
#include <vector>
#include <filesystem>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/regex.hpp>
#include <boost/xpressive/xpressive.hpp>
#include <regex>
#include <iterator>

#include <codecvt>
#include <locale>
#include "utf8.h"

#include "mp3CharConversion.hpp"
#include <fstream>
#include "config.hpp"
#include <boost/filesystem.hpp>
//#include <nlohmann/json.hpp>

//using json = nlohmann::json;

using std::string;
using namespace std;

int Mp3Tags::running_index = 1;

Mp3Tags::Mp3Tags()
{
    this->full_path = "";   // contains root directory
    this->local_path = "";
    this->file_index = -1;  // running_index
    this->artist = "";
    this->title = "";
    this->seconds = -1;     // This can be read from valid mp3
    this->combinedName = "";
}

Mp3Tags::Mp3Tags(std::string filepath, int rootLen) : Mp3Tags()
{
        this->full_path = filepath;
        this->file_index = this->running_index++;

        if (rootLen > 0)
        {
            this->local_path = filepath.substr(rootLen+1);
        } else {
            this->local_path = filepath;
        }

        TagLib::FileRef f(filepath.c_str());

        if(!f.isNull() && f.tag())
        {
            TagLib::Tag *tag = f.tag();

            this->title = tag->title().toCString();
            this->artist = tag->artist().toCString();

            TagLib::PropertyMap tags = f.file()->properties();
        }

        if(!f.isNull() && f.audioProperties())
        {
            TagLib::AudioProperties *properties = f.audioProperties();

            this->seconds = properties->lengthInSeconds();

            int seconds = properties->lengthInSeconds() % 60;
            //int minutes = (properties->lengthInSeconds() - seconds) / 60;
        }
}

/**
 * This checks if mp3 title tag is not included to filename. Adds to end of basename when tag not found.
 * Characters from A to z are used in comparison. mp3 files are not expected to have standard character set.
 */
std::string Mp3Tags::getCombinedName()
{
    string basename = "";
    //if (true)
    {
    string path_sep(1, std::filesystem::path::preferred_separator); // (1, std::filesystem::path::preferred_separator)
    std::vector<std::string> tokens;
    boost::split(tokens, this->local_path, boost::is_any_of(path_sep), boost::token_compress_on);
    if (tokens.size() >= 0)
    {
        string filename = tokens.back();
        string lower_name = boost::to_lower_copy(filename);
        string ext = ".mp3";
        string utf8title = "";
        int end_pos = lower_name.size() - ext.size();
        if (end_pos <= 0) end_pos = 0;
        string lower_end = lower_name.substr(end_pos, ext.size());
        if (lower_end.compare(ext) == 0) {
            filename = filename.substr(0, end_pos);
        }
        // Check if title is in basename
        // title space could be replaced to dots
        wstring ws_str; // = this->title;
        try {
                  decode_windows_chars(this->title, ws_str);  //decode_utf8(this->title, ws_str);
                  encode_utf8(ws_str, utf8title);
        } catch (...) {
            cout << "Problem in " << filename << " conversion, title." << endl;
            decode_ascii_chars(this->title, ws_str);
        }
        boost::wregex rx(L"[^A-Za-z0-9]");
        std::vector<int> match_positions;
        for (auto it = boost::wsregex_iterator(ws_str.begin(), ws_str.end(), rx);
             it != boost::wsregex_iterator(); it++) {
            match_positions.push_back(it->position());
        }
        for (int i : match_positions) {
            ws_str[i] = '.';
        }
        wstring wfname;
        try {
            decode_utf8(filename, wfname);
        } catch (...) {
            cout << "Problem in " << filename << " conversion, filename." << endl;
            decode_ascii_chars(filename, wfname);
        }
        std::vector<int> match_fpositions;
        for (auto it = boost::wsregex_iterator(wfname.begin(), wfname.end(), rx);
             it != boost::wsregex_iterator(); it++) {
            match_fpositions.push_back(it->position());
        }
        for (int i : match_fpositions) {
            wfname[i] = '.';
        }

        boost::wregex re(ws_str,  boost::regex::icase);
        boost::wsmatch m;

        if (!boost::regex_search(wfname, m, re))
        {
            // combine tokens except last one, basename, title and .mp3Tags
            string new_local_name = "";
            for (int i=0; i<tokens.size()-1; i++) {
                new_local_name += tokens[i];
                new_local_name += path_sep;
            }
            new_local_name += filename;
            new_local_name += "_";
            new_local_name += utf8title;
            new_local_name += ".mp3";
            this->combinedName = new_local_name;
            return new_local_name;
        }
    }
    }
    if (this->combinedName.size() > 0) {
        return this->combinedName;
    } else {
        return this->local_path;
    }
}


std::string Mp3Tags::toString() const
{
    std::string result = "Path:";
    result += this->local_path;
    result +=" seconds:"+std::to_string(seconds);
    result +=" read index:" + std::to_string(file_index);
    return result;
}

int Mp3Tags::getSeconds()
{
    return this->seconds;
}

bool Mp3Tags::isMp3()
{
    return this->seconds > 0;
}


std::string Mp3Tags::getTitle()
{
    return this->title;
}

std::string Mp3Tags::getPath()
{
    return this->local_path;
}

std::string Mp3Tags::getFullpath()
{
    return this->full_path;
}

int Mp3Tags::getRunningNr()
{
    return this->file_index;
};


