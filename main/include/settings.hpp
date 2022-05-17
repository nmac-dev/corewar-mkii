/// Handles match settings
#pragma once

// #define SETTINGS_DEBUG

#include <unordered_map>

#include "file_loader.hpp"

/// Singleton class to load & store match settings from a configuration file
class Settings
{
 private:
    inline static char const ini_section[]  = "[Match Parameters]";
    inline static char const ini_filename[] = "corewar.ini";
    inline static char const ini_comment    = '#';
    inline static int  const ini_total_vals = 5;

    /// .ini setting names
    inline static char const INI_NAMES[ini_total_vals][32] {
        "max_rounds",
        "max_cycles",
        "max_processes",
        "max_program_insts",
        "min_separation"
    };

    /// default values for the .ini settings, used if parameters are missing
    inline static const int INI_DEFAULT_VALUES[ini_total_vals] {
        3,  // max_rounds
        8,  // max_cycles
        8,  // max_processes
        12, // max_program_insts
        8   // min_separation
        };

    /// stores the .ini setting values
    std::unordered_map<std::string, int> ini_values;

    /// Constructor is blocked
    Settings(){}

    /// Uses File_Loader:: to read match_settings.ini, then processes the contents into the hash table
    inline void load_ini()
    {
        std::vector<std::string> data = File_Loader::load_file_data(ini_filename, ini_comment);
        std::string name; int value; // Map<key, value>
        ini_values.clear();

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
            ini_values.insert(std::make_pair(name, value));
        }

        // add default value for any missing parameters
        for (int i = 0; i < ini_total_vals; i++)
        {
            if (!ini_values.count(INI_NAMES[i]))
            {
                ini_values.insert(std::make_pair(INI_NAMES[i], INI_DEFAULT_VALUES[i]));
            }

            #ifdef SETTINGS_DEBUG
            if (i == 0) printf("\nSettings::load_ini: \n");
            printf("\t\"%s\" = %d\n", INI_NAMES[i].c_str(), ini_values.at(INI_NAMES[i]));
            #endif
        }
    }  /* ::load_ini() */

 public:
    /// Settings instance access method
    static Settings& get()
    {
        static Settings match_settings;
        
        // flag to ensure the settings .ini file has been loaded before any values are used
        static int flag_loaded;
        if (!flag_loaded)
        {
            match_settings.load_ini();
            flag_loaded++;
        }
        return match_settings;
    }

    /// Copy creation method deleted
    Settings(Settings const &)        = delete;
    /// Assignment operator method deleted
    void operator= (Settings const &) = delete;

    
    /// ini_filename of the .ini configuration file containing the settings values
    inline char const *file_name() const { return get().ini_filename; }

    /// Max number of rounds before the game is concluded
    inline int max_rounds()        const { return get().ini_values.at(INI_NAMES[0]); }

    /// Max number of cycles before the round has been concluded
    inline int max_cycles()        const { return get().ini_values.at(INI_NAMES[1]); }

    /// Max number of processes a single program can create
    inline int max_processes()     const { return get().ini_values.at(INI_NAMES[2]); }

    /// Max instructions a program can consist of
    inline int max_program_insts() const { return get().ini_values.at(INI_NAMES[3]); }

    /// Min distance between programs at the start of a round
    inline int min_separation()    const { return get().ini_values.at(INI_NAMES[4]); }

}; /* ::Settings */
