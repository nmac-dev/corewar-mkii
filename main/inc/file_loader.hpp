/// Handles loading files for corewar (match settings, warriors)
#pragma once

// #define FILE_LOADER_DEBUG

#include <iostream>
#include <string>
#include <fstream>
#include <vector>

namespace file_loader
{
/// Get the File Data object
/// @param filename directory/filename location
/// @param comment  (optional) character considered a comment within the file
/// @return collection containing each line from the data file
inline std::vector<std::string> getFileData(std::string filename, const char comment = 0)
{
    std::vector<std::string> file_data;
    std::ifstream fs (filename, std::ios::in);
    std::string line;
    
    if (fs.is_open())
        while(std::getline(fs, line))
        {
            char first = line[0]; // first character of the line
            if (first == comment || first == 0 || first == '\n')
                continue;
            
            // remove comments and ignore blank strings
            bool blank = true;
            for (int i = 0; i < line.size(); i++)
            {
                char val = line[i];
                // slice comment
                if (val == comment)
                    line.erase(i);
                
                // ignore blank line
                if (val != ' ' && val != '\t' && val != '\n')
                    blank = false;
            }
            if (!blank)
                file_data.push_back(line);

            #ifdef FILE_LOADER_DEBUG
                std::cout << "file_loader::getFileData: \t" << !blank ? "Added: " : "Ignored: " << line << std::endl;
            #endif
        }
    else 
    {
        std::cerr << "Error: cannot open file... |" << filename << "|" << std::endl;
        throw std::exception(); // begin stack unwind to main()
    }
    fs.close();
    return file_data;
}

/// Shuffles all whitespace to the end of the string and erases the whitespace segment
/// @param str reference
inline void removeWhiteSpace(std::string &str)
{
    int ws = 0; // whitespace counter
    for (int i = 0; i < str.length() - ws; i++)
    {
        while (str[i + ws] == ' ' || str[i + ws] == '\t')
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
} /// namespace file_loader
