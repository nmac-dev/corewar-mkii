/// Handles match setting
#pragma once

#include "file_loader.hpp"
#include <unordered_map>

/// Defines: Singleton class to store & retrieve match settings
class Settings
{
public:
    Settings(const Settings&) = delete; // Blocks singleton copy creation

    static Settings& Get()  // Singleton instance access method
    {
        return match_settings;
    }

    void loadSettings();
    int core_size() const,
        max_rounds() const,
        max_cycles() const,
        max_processes() const,
        max_warrior_len() const,
        min_separation() const;

private:
    const char *filename = "match_settings.txt";
    const char  comment = '#';
    
    /**
     * @brief Stores the setting values using the parameter name as a key
     */
    std::unordered_map<std::string, int> setting_values;

    // Singleton instance
    static Settings match_settings;
    Settings() {}

    void removeWhiteSpace(std::string &str);
};

/// Defines singleton class instance
Settings Settings::match_settings;

/**
 * @brief Uses file_loader:: to read match_settings.txt, then processes the contents into the hash table
 */
void Settings::loadSettings() 
{
    std::vector<std::string> data = file_loader::getFileData(filename, comment);
    std::string name;
    int value;
    setting_values.clear();

    /// clean data, get setting name & value
    for (int i = 0; i < data.size(); i++)
    {
        removeWhiteSpace(data[i]);
        for (int j = 0; j < data[i].length(); j++)
        {
            if (data[i][j] == '=')
            {
                name = data[i].substr(0, j);
                value = std::stoi(data[i].substr(j +1, data[i].length()));
                break;
            }
        }
        setting_values.insert(std::make_pair(name, value));
    }
}

/**
 * @brief Shuffles all whitespace to the end of the string and erases the whitespace segment
 * @param str reference
 */
void Settings::removeWhiteSpace(std::string &str)
{
    int ws = 0; // whitespace counter
    for (int i = 0; i < str.length() - ws; i++)
    {
        while (str[i + ws] == ' ')
            ws++;
        str[i] = str[i + ws];
    }
    str.erase(str.length() - ws);
}

/// number of memory addresses within the core
int Settings::core_size() const       {return setting_values.find("core_size")->second;}

/// max number of rounds before the game is concluded
int Settings::max_rounds() const      {return setting_values.find("max_rounds")->second;}

/// max number of cycles before the round has been concluded
int Settings::max_cycles() const      {return setting_values.find("max_cycles")->second;}

/// max number of processes a single warrior can create
int Settings::max_processes() const   {return setting_values.find("max_processes")->second;}

/// max instructions a warrior can consist of
int Settings::max_warrior_len() const {return setting_values.find("max_warrior_len")->second;}

/// min distance between warriors at the start of a round
int Settings::min_separation() const  {return setting_values.find("min_separation")->second;}