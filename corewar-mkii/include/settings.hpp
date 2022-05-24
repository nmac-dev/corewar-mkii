/// Handles match settings
#pragma once

// #define SETTINGS_DEBUG

#include <unordered_map>

#include "file_loader.hpp"

/// Singleton class to load & store match settings from a configuration file
class Settings
{
 using ConfigINI = std::unordered_map<std::string, int>;
 private:
    static char constexpr ini_section[]  = "[Match Parameters]";
    static char constexpr ini_filename[] = "corewar.ini";
    static char constexpr ini_comment    = '#';
    static int  constexpr ini_total_vals = 5;
    static inline bool    flag_loaded    = false; // true when the '.ini' file has been loaded

    /// .ini setting names
    static char constexpr ini_names[ini_total_vals][32] {
        "max_rounds",
        "max_cycles",
        "max_processes",
        "max_program_insts",
        "min_separation",
    };

    /// default values for the .ini settings, used if parameters are missing
    static int constexpr ini_defaults[ini_total_vals] {
        3,  // max_rounds
        8,  // max_cycles
        8,  // max_processes
        12, // max_program_insts
        8   // min_separation
    };

    /// stores the .ini contents as a hash table
    static inline ConfigINI config_ini;

    /// Constructor is blocked
    Settings(){}

 public:
    /// Uses File_Loader:: to read match_settings.ini, then processes the contents into the hash table
    static void load_ini()
    {
        std::vector<std::string> data = File_Loader::load_file_data(ini_filename, ini_comment);
        std::string name; int value; // Map<key, value>
        config_ini.clear();

        // validate data is from correct file
        if (data[0] != ini_section)
        {
            printf("Error: expected '%s' but found '%s' in config file |%s|\n",
                    ini_section, data[0].c_str(), ini_filename
            );
            throw std::exception(); // begin stack unwind to main()
        }

        // process each parameter & value
        for (int i = 1; i < data.size(); i++)
        {
            // clean data, get setting name & value
            File_Loader::remove_whitespace(data[i]);
            for (int j = 0; j < data[i].length(); j++)
            {
                if (data[i][j] == '=')
                {
                    name  = data[i].substr(0, j);
                    value = std::stoi(data[i].substr(j +1, data[i].length()));
                    break;
                }
            }
            config_ini.insert(std::make_pair(name, value));
        }

        // add default value for any missing parameters
        for (int i = 0; i < ini_total_vals; i++)
        {
            if (!config_ini.count(ini_names[i]))
            {
                config_ini.insert(std::make_pair(ini_names[i], ini_defaults[i]));
            }

            #ifdef SETTINGS_DEBUG
            if (i == 0) printf("\nSettings::load_ini: \n");
            printf("\t\"%s\" = %d\n", ini_names[i], config_ini.at(ini_names[i]));
            #endif
        }
        flag_loaded = true;;
    }  /* ::load_ini() */

 private:
    /// Always called before fetching setting to ensure file is loaded
    static inline void fetch()
    {
        // flag to ensure the settings .ini file has been loaded before any values are used
        if (!flag_loaded)
            load_ini();
    }

 public:
    /// Copy creation method deleted
    Settings(Settings const &)        = delete;
    /// Assignment operator method deleted
    void operator= (Settings const &) = delete;

 /* Parameters */

    /// Max number of rounds before the game is concluded
    static inline int const &max_rounds()        { fetch(); return config_ini.at(ini_names[0]); }

    /// Max number of cycles before the round has been concluded
    static inline int const &max_cycles()        { fetch(); return config_ini.at(ini_names[1]); }

    /// Max number of processes a single program can create
    static inline int const &max_processes()     { fetch(); return config_ini.at(ini_names[2]); }

    /// Max instructions a program can consist of
    static inline int const &max_program_insts() { fetch(); return config_ini.at(ini_names[3]); }

    /// Min distance between programs at the start of a round
    static inline int const &min_separation()    { fetch(); return config_ini.at(ini_names[4]); }

}; /* ::Settings */
