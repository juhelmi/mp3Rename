

#include <string>
#include <vector>
#include <filesystem>

#include <iostream>
#include <fstream>
#include "config.hpp"
#include <boost/filesystem.hpp>
#include <nlohmann/json.hpp>
#include <exception>

#include "utf8.h"
#include <codecvt>

using namespace std;
using std::string;
using json = nlohmann::json;

class char_exception: public std::exception
{
  virtual const char* what() const throw()
  {
    return "Exception in character conversion.";
  }
} charex;

void convert_user_chars_to_utf8(std::string const & u8str, std::string & utf8dest)
{
    //std::vector<unsigned int, std::string> conversion_values;
    static std::string table[256];
    static bool table_initialized = false;
    std::vector<std::pair<unsigned int, std::string>> pairs;

    utf8dest = "";
    if (!table_initialized) {
        string fname(char_conversion_json_file);
        if (!std::filesystem::exists(fname)) {
            std::vector<std::pair<unsigned int, std::string>> start_values = {{196, "Ä"}, {228, "ä"}, {132, "ä"}, {246, "ö"}, {148, "ö"}};
            nlohmann::json j;
            for (auto elem : start_values) {
                j.push_back(elem);
            }
            std::ofstream f;
            f.open(string(char_conversion_json_file));
            f << std::setw(4) << j << std::endl;
            f.close();
        }
        if (!std::filesystem::exists(fname)) {
            throw char_exception();
        } else {
            std::ifstream input_file(fname);
            //auto j = nlohmann::json::parse(input_file);
            nlohmann::json j;
            input_file >> j;
            string s = j.dump();
            for (auto elem : j) {
                unsigned int i = elem[0];
                pairs.push_back(elem);
                if (i < 256) {
                    table[i] = elem[1];
                }
            }
            input_file.close();
        }
        table_initialized = true;
    }
     for (int i=0; i<u8str.size(); i++) {
         char c = u8str[i];
         if (c < 0) {
             unsigned char uc = c;
            if (table[uc].size() > 0) {
                utf8dest += table[uc];
            }
         } else {
             utf8dest += c;
         }
     }
}

void decode_windows_chars(std::string const & utf8str, std::wstring& wstr)
{
    std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> conv;
    std::string tmp;
    convert_user_chars_to_utf8(utf8str, tmp);

     std::u32string utf32str = conv.from_bytes(tmp);

     wstr = L"";
     for (char32_t u : utf32str)
     {
         /* ... */
         switch (u) {
             default:
                wstr += u;
                break;
         }
    }
}

void decode_ascii_chars(std::string const & utf8str, std::wstring& wstr)
{
     std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> conv;
     std::u32string utf32str = conv.from_bytes(utf8str);

     wstr = L"";
     for (char32_t u : utf32str)
     {
         /* ... */
         if (u <= 127) {
                wstr += u;
         } else {
             wstr += L' ';
         }
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
