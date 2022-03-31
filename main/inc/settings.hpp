/// Handles match settings
#pragma once

// #define SETTINGS_DEBUG

#include <unordered_map>

#include "file_loader.hpp"

/// Singleton class to load & store match settings from a configuration file
class Settings
{
 private:
    inline static const std::string ini_section = "[Match Parameters]";
    inline static const std::string filename    = "corewar.ini";
    inline static const char        comment     = '#';
    
    /// Stores the setting values using the parameter name as a key
    std::unordered_map<std::string, int> setting_values;


    /// Constructor is blocked
    Settings() {}

 public:
    /// Settings instance access method
    static Settings& get()
    {
        static Settings match_settings;
        
        // flag to ensure the settings .ini file has been loaded before any values are used
        static int flag_loaded;
        // ensure .ini has been loaded before any value is accessed
        if (!flag_loaded)
        {
            match_settings.loadSettings();
            flag_loaded++;
        }

        return match_settings;
    }

    /// Copy creation method deleted
    Settings(Settings const &)       = delete;
    /// Assignment operator method deleted
    void operator= (Settings const &) = delete;

    /// Uses file_loader:: to read match_settings.txt, then processes the contents into the hash table
    inline void loadSettings()
    {
        std::vector<std::string> data = file_loader::getFileData(filename, comment);
        std::string name;
        int value;
        setting_values.clear();

        // validate data is from correct file
        if (data[0] != ini_section)
        {
            std::cerr 
                << "Error: expected.. " << ini_section << " ..but found.. " << data[0]
                <<  " ..in config file.. |" << filename << '|' << std::endl;
            throw std::exception(); // begin stack unwind to main()
        }

        // process each parameter & value
        for (int i = 1; i < data.size(); i++)
        {
            // clean data, get setting name & value
            file_loader::removeWhiteSpace(data[i]);
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
        
        #ifdef SETTINGS_DEBUG
            for (auto itr = setting_values.begin(); itr != setting_values.end(); itr++)
            {
                std::cout
                    << "Settings::loadSettings: \t" 
                    << "Key= \""    << itr->first << "\""
                    << " \tValue= " << itr->second
                    << std::endl;
            }
        #endif
    }
    
    /// Filename of the .ini configuration file containing the settings values
    inline const std::string file_name() const {return filename;}

    /// Total number of memory addresses within the core
    inline int core_size() const {return setting_values.at("core_size");}

    /// Max number of rounds before the game is concluded
    inline int max_rounds() const {return setting_values.at("max_rounds");}

    /// Max number of cycles before the round has been concluded
    inline int max_cycles() const {return setting_values.at("max_cycles");}

    /// Max number of processes a single warrior can create
    inline int max_processes() const {return setting_values.at("max_processes");}

    /// Max instructions a warrior can consist of
    inline int max_warrior_len() const {return setting_values.at("max_warrior_len");}

    /// Min distance between warriors at the start of a round
    inline int min_separation() const {return setting_values.at("min_separation");}
};
