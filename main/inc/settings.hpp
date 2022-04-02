/// Handles match settings
#pragma once

// #define SETTINGS_DEBUG

#include <unordered_map>

#include "file_loader.hpp"

/// Singleton class to load & store match settings from a configuration file
class Settings
{
 private:
    inline static const std::string INI_SECTION = "[Match Parameters]";
    inline static const std::string FILENAME    = "corewar.ini";
    inline static const char        COMMENT     = '#';
    inline static const int         N_SETTINGS  = 6;

    /// .ini setting names
    inline static const std::string INI_NAMES[N_SETTINGS] =
    {"core_size", "max_rounds", "max_cycles", "max_processes", "max_warrior_len", "min_separation"};

    /// default values for the .ini settings, used if parameters are missing
    inline static const int INI_DEFAULT_VALUES[N_SETTINGS] = {8000, 3, 8 ,8, 12, 8};

    /// stores the .ini setting values
    std::unordered_map<std::string, int> ini_values;

    /// Constructor is blocked
    Settings(){}

    /// Uses file_loader:: to read match_settings.ini, then processes the contents into the hash table
    inline void loadINI()
    {
        std::vector<std::string> data = file_loader::getFileData(FILENAME, COMMENT);
        std::string name; int value; // Map<key, value>
        ini_values.clear();

        // validate data is from correct file
        if (data[0] != INI_SECTION)
        {
            printf("Error: expected '%s' but found '%s' in config file |%s|\n",
                    INI_SECTION.c_str(), data[0].c_str(), FILENAME.c_str()
            );
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
                    name  = data[i].substr(0, j);
                    value = std::stoi(data[i].substr(j +1, data[i].length()));
                    break;
                }
            }
            ini_values.insert(std::make_pair(name, value));
        }

        // add default value for any missing parameters
        for (int i = 0; i < N_SETTINGS; i++)
        {
            if (!ini_values.count(INI_NAMES[i]))
            {
                ini_values.insert(std::make_pair(INI_NAMES[i], INI_DEFAULT_VALUES[i]));
            }

            #ifdef SETTINGS_DEBUG
                if (i == 0) printf("\nSettings::loadINI: \n");
                printf("\t\"%s\" = %d\n", INI_NAMES[i].c_str(), ini_values.at(INI_NAMES[i]));
            #endif
        }
    }

 public:
    /// Settings instance access method
    static Settings& get()
    {
        static Settings match_settings;
        
        // flag to ensure the settings .ini file has been loaded before any values are used
        static int flag_loaded;
        if (!flag_loaded)
        {
            match_settings.loadINI();
            flag_loaded++;
        }
        return match_settings;
    }

    /// Copy creation method deleted
    Settings(Settings const &)       = delete;
    /// Assignment operator method deleted
    void operator= (Settings const &) = delete;

    
    /// FILENAME of the .ini configuration file containing the settings values
    inline const std::string file_name() const {return get().FILENAME;}

    /// Total number of memory addresses within the core
    inline int core_size()               const {return get().ini_values.at(INI_NAMES[0]);}

    /// Max number of rounds before the game is concluded
    inline int max_rounds()              const {return get().ini_values.at(INI_NAMES[1]);}

    /// Max number of cycles before the round has been concluded
    inline int max_cycles()              const {return get().ini_values.at(INI_NAMES[2]);}

    /// Max number of processes a single warrior can create
    inline int max_processes()           const {return get().ini_values.at(INI_NAMES[3]);}

    /// Max instructions a warrior can consist of
    inline int max_warrior_len()         const {return get().ini_values.at(INI_NAMES[4]);}

    /// Min distance between warriors at the start of a round
    inline int min_separation()          const {return get().ini_values.at(INI_NAMES[5]);}
};
