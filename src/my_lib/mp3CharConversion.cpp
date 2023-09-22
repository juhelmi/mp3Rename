

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

void convert_wildcards_to_utf8(std::string const & u8str, std::string & utf8dest)
{
    //std::vector<unsigned int, std::string> conversion_values;
    static std::string table[256];
    static bool table_initialized = false;
    //std::vector<std::pair<unsigned char, std::string>> pairs;

    utf8dest = "";
    if (!table_initialized) {
        string fname(wildcard_conversion_json_file);
        if (!std::filesystem::exists(fname)) {
            std::vector<std::pair<char, std::string>> start_values = {{'*', "_"}, {'?', "_"}, {'/', "_"}, {'\\', "_"}, {';', "_"}, {'(', "_"}, {')', "_"}};
            nlohmann::json j;
            for (auto elem : start_values) {
                j.push_back(elem);
            }
            std::ofstream f;
            f.open(fname);
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
                //pairs.push_back(elem);
                if (i < 256) {
                    table[i] = elem[1];
                }
            }
            input_file.close();
        }
        table_initialized = true;
    }
     for (int i=0; i<u8str.size(); i++) {
         unsigned char c = u8str[i];
         if (table[c].size() > 0) {
                utf8dest += table[c];
         } else {
             utf8dest += c;
         }
     }
}

void decode_windows_chars(std::string const & utf8str, std::wstring& wstr)
{
    std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> conv;
    std::string tmp1, tmp;
    convert_user_chars_to_utf8(utf8str, tmp1);
    convert_wildcards_to_utf8(tmp1, tmp);

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
#endif

#if (FALSE)
    template <typename octet_iterator>
    octet_iterator append(uint32_t cp, octet_iterator result)
    {
        if (!utf8::internal::is_code_point_valid(cp))
            throw invalid_code_point(cp);

        if (cp < 0x80)                        // one octet
            *(result++) = static_cast<uint8_t>(cp);
        else if (cp < 0x800) {                // two octets
            *(result++) = static_cast<uint8_t>((cp >> 6)            | 0xc0);
            *(result++) = static_cast<uint8_t>((cp & 0x3f)          | 0x80);false
        }
        else if (cp < 0x10000) {              // three octets
            *(result++) = static_cast<uint8_t>((cp >> 12)           | 0xe0);
            *(result++) = static_cast<uint8_t>(((cp >> 6) & 0x3f)   | 0x80);
            *(result++) = static_cast<uint8_t>((cp & 0x3f)          | 0x80);
        }
        else {                                // four octets
            *(result++) = static_cast<uint8_t>((cp >> 18)           | 0xf0);
            *(result++) = static_cast<uint8_t>(((cp >> 12) & 0x3f)  | 0x80);
            *(result++) = static_cast<uint8_t>(((cp >> 6) & 0x3f)   | 0x80);
            *(result++) = static_cast<uint8_t>((cp & 0x3f)          | 0x80);
        }
        return result;
    }

    template <typename octet_iterator, typename u32bit_iterator>
    octet_iterator utf32to8 (u32bit_iterator start, u32bit_iterator end, octet_iterator result)
    {
        while (start != end)
            result = utf8::append(*(start++), result);

        return result;
    }
#endif

void encode_utf8(const std::wstring& wstr, std::string& bytes)
{
    utf8::utf32to8(wstr.begin(), wstr.end(), std::back_inserter(bytes));
}


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
