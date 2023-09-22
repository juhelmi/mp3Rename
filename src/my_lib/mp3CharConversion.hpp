#pragma once

#include <string>

void convert_user_chars_to_utf8(std::string const & u8str, std::string & utf8dest);
void convert_wildcards_to_utf8(std::string const & u8str, std::string & utf8dest);
void decode_windows_chars(std::string const & utf8str, std::wstring& wstr);
void decode_ascii_chars(std::string const & utf8str, std::wstring& wstr);

void decode_utf8(std::string const & utf8str, std::wstring& wstr);

void encode_utf8(const std::wstring& wstr, std::string& bytes);
