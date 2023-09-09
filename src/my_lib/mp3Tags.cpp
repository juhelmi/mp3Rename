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
//#include "utf8.h"

#include "mp3CharConversion.hpp"
#include <fstream>
#include "config.hpp"
#include <boost/filesystem.hpp>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

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

#ifdef NOT_USED
    bool extra_print = false;
#endif

    TagLib::FileRef f(filepath.c_str());

    if(!f.isNull() && f.tag())
    {
        TagLib::Tag *tag = f.tag();

#ifdef NOT_USED
        if (extra_print) {
            cout << "-- TAG (basic) --" << endl;
        }
#endif
        //cout << "title   - \"" << tag->title()   << "\"" << endl;
        //cout << "artist  - \"" << tag->artist()  << "\"" << endl;
        this->title = tag->title().toCString();
        this->artist = tag->artist().toCString();
#ifdef NOT_USED
        if (extra_print) {
            cout << "album   - \"" << tag->album()   << "\"" << endl;
            cout << "year    - \"" << tag->year()    << "\"" << endl;
            cout << "comment - \"" << tag->comment() << "\"" << endl;
            cout << "track   - \"" << tag->track()   << "\"" << endl;
            cout << "genre   - \"" << tag->genre()   << "\"" << endl;
        }
#endif

        TagLib::PropertyMap tags = f.file()->properties();

#ifdef NOT_USED
        if (extra_print)
        {
            unsigned int longest = 0;
            for (auto i = tags.cbegin(); i != tags.cend(); ++i)
            {
                if (i->first.size() > longest) {
                longest = i->first.size();
                }
            }
            cout << "-- TAG (properties) --" << endl;
            for(auto i = tags.cbegin(); i != tags.cend(); ++i) {
                for(auto j = i->second.begin(); j != i->second.end(); ++j) {
                cout << left << std::setw(longest) << i->first << " - " << '"' << *j << '"' << endl;
                }
            }
        }
#endif

    }

    if(!f.isNull() && f.audioProperties())
    {
        TagLib::AudioProperties *properties = f.audioProperties();

        this->seconds = properties->lengthInSeconds();

        int seconds = properties->lengthInSeconds() % 60;
        int minutes = (properties->lengthInSeconds() - seconds) / 60;

#ifdef NOT_USED
        if (extra_print)
        {
            cout << "-- AUDIO --" << endl;
            cout << "bitrate     - " << properties->bitrate() << endl;
            cout << "sample rate - " << properties->sampleRate() << endl;
            cout << "channels    - " << properties->channels() << endl;
        }
#endif
        //cout << "length      - " << minutes << ":" << setfill('0') << setw(2) << seconds << endl;
    }
}

#if (FALSE)
inline void decode_utf8(const std::string& bytes, std::wstring& wstr)
{
    utf8::utf8to32(bytes.begin(), bytes.end(), std::back_inserter(wstr));
}

inline void encode_utf8(const std::wstring& wstr, std::string& bytes)
{
    utf8::utf32to8(wstr.begin(), wstr.end(), std::back_inserter(bytes));
}
#endif
void decode_utf8(std::string const & utf8str, std::wstring& wstr)
{
     std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> conv;
     std::u32string utf32str = conv.from_bytes(utf8str);

     wstr = L"";
     for (char32_t u : utf32str)
     {
         /* ... */
         wstr += u;
    }
}


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
        //std::locale::global(std::locale("fi_FI.UTF-8"));
        //string fname = filename;
        // std::wstring wfname(filename.begin(), filename.end());
        wstring wfname;
        //decode_utf8(filename, wfname);
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

        //std::wstring wfname(fname.begin(), fname.end());
        std::cout << "filename " << filename << std::endl;
        boost::wregex re(ws_str);
        boost::wsmatch m;
        //if (!std::regex_search(filename, m, re))
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
            new_local_name += this->title;
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
    result +=" addition pos:" + std::to_string(file_index);
    result +=" seconds:"+std::to_string(seconds);
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




