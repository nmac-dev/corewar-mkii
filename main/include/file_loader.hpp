/// Handles loading files
#pragma once

// #define FILE_LOADER_DEBUG

#include <iostream>
#include <string>
#include <fstream>
#include <vector>

namespace File_Loader
{
namespace /* {anonymous} */
{    
/// Return true if the character is white space
/// @param val to be compared
inline bool is_space(char val)
{
    return val == '\t' || val == ' '; 
}
} /* ::{anonymous} */

/// Load the File Data to a string collection
/// @param filename directory/filename location
/// @param comment  (optional) character considered a comment within the file
/// @return collection containing each line from the data file
inline std::vector<std::string> load_file_data(std::string filename, const char comment = 0)
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
                {
                    while (is_space(line[--i])); // find last non-whitespace
                    line.erase(++i);            // then erase
                    break;
                }
                // unset blank flag if line contains content
                if (!is_space(val))
                    blank = false;
            }
            if (!blank)
                file_data.push_back(line);

            #ifdef FILE_LOADER_DEBUG
            std::cout << "File_Loader::load_file_data: \t" 
                << (!blank ? "Added: " : "Ignored: ") 
                << line << std::endl;
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
inline void remove_whitespace(std::string &str)
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
        << "File_Loader::remove_whitespace: \t" << str.c_str()
        << std::endl;
    #endif
}

} /* ::File_Loader */
