/// Handles loading files for corewar (match settings, warriors)
#pragma once

// #define FILE_LOADER_DEBUG

#include <iostream>
#include <fstream>
#include <vector>

namespace file_loader 
{
/**
 * @brief Get the File Data object
 * 
 * @param filename directory/filename location
 * @param comment  (optional) character considered a comment within the file
 * @return std::vector<std::string> 
 */
static std::vector<std::string> getFileData(const char *filename, const char comment = 0)
{
    std::vector<std::string> file_data;
    std::ifstream fs (filename, std::ios::in);
    std::string line;
    
    if (fs.is_open())
        while(std::getline(fs, line))
        {
            char sc = line[0]; // starting character
            if (sc == comment || sc == 0 || sc == ' ' || sc == '\n') 
                continue;
            file_data.push_back(line);

            #ifdef FILE_LOADER_DEBUG
                std::cout << "file_loader::getFileData: \t" << line << std::endl;
            #endif
        }
    else perror("Error: cannot open file");

    fs.close();
    return file_data;
}
/**
 * @brief Shuffles all whitespace to the end of the string and erases the whitespace segment
 * @param str reference
 */
static void removeWhiteSpace(std::string &str)
{
    int ws = 0; // whitespace counter
    for (int i = 0; i < str.length() - ws; i++)
    {
        while (str[i + ws] == ' ')
            ws++;
        str[i] = str[i + ws];
    }
    str.erase(str.length() - ws);

    #ifdef FILE_LOADER_DEBUG
            std::cout 
                << "file_loader::removeWhiteSpace: \t" << str.c_str()
                << std::endl;
    #endif
}
}