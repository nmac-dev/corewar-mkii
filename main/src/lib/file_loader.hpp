/// Handles loading files for corewar (match settings, warriors)
#pragma once

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
std::vector<std::string> getFileData(const char *filename, const char comment = 0)
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
        }
    else
        perror("Error: cannot open file");
    fs.close();
    return file_data;
}
}