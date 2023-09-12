#pragma once

#include <string>

class Mp3Tags
{
private:
    static int running_index;
    int file_index;
    std::string full_path;
    std::string local_path;     // proportional to root directory
    int seconds;
    std::string title;
    std::string artist;
    std::string combinedName;

public:
    Mp3Tags();
    Mp3Tags(std::string filepath, int rootLen);
    std::string toString() const;
    int getSeconds();
    bool isMp3();
    std::string getTitle();
    std::string getPath();
    std::string getFullpath();
    std::string getCombinedName();
    int getRunningNr();
};

