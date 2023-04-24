/// GUI display for loading warrior files and tracking stats
#pragma once

#include <filesystem>
#include <ctime>
#include "imgui_required.hpp"
#include "memory_viewer.gui.hpp"

namespace FileSys = std::filesystem;

namespace Core { namespace GUI
{

/// A static class to display a control panel for core
class ControlPanel
{
 public:
    /// Stores stats for a single warriors using pointers to realtime data
    struct Stats
    {
        Player      player       =  Player::NONE;
        OS::UUID    const *uuid  =  NULL;
        std::string const *name  =  NULL;
        int         const *prcs  =  NULL;
        Score       const *score =  NULL;
    };

 private:
    /* Display */
    static inline bool             m_init_flag       = false; // set to true when the display is initialised
    static inline ImVec2 constexpr m_min_window_size = ImVec2(384.f, 540.f);

    /* Data */
    static inline bool               warriors_locked; // true if the warriors have been loaded
    static inline int                total_selected;  // total selected warriors
    static inline int                new_round_flag;  // true if a new round has started
    static inline WarriorFiles       dir_files;       // collection of all files in the 'warriors/' directory
    static inline std::vector<int>   select_indexes;  // counter for each selected file from the 'warriors/' directory
    static inline std::vector<Stats> warrior_stats;   // Collection of warrior stats
    static inline Core::Game        *ptr_core;        // pointer to the running core game

    ControlPanel() = default;                       /// Constructor blocked
 public:
    ControlPanel(ControlPanel const &)    = delete; /// Copy creation deleted
    void operator=(ControlPanel const &) = delete;  /// Assignment operator deleted

 /* Functions */

    /// Initialises the Warriors display, must be called before use but after core game is running
    /// @param _core pointer to a running core game
    static void init(Game *_core);

    /// Returns true if the memory display has been initialised
    static inline bool &init_flag() { return m_init_flag; }

    /// Runs the game of core and its displays
    static void run_core_systems();

    // resets the game of core and memory display
    static void reset();

    /// Reloads all the warrior files in 'warriors/'
    static void reload_files();

    /// Selects the warrior file
    static inline void select_file(int _index);

    /// Loads the selected warriors ito the corwar game
    static void load_selected();

    /// Clear all selected file indexes
    static void clear_selected()
    {
        for (int i = 0; i < select_indexes.size(); i++)
            select_indexes[i] = 0;
        total_selected = 0;
    }

    /// Draw the Warriors display on the GUI
    static void draw();

 private:
    /* Sections Of Control Panel Display (called from 'draw()')*/

    /// Section for the master control ( Restart | Reload )
    static inline void section_master_controls();

    /// Section for the warrior loader menu
    static inline void section_loader_menu();

    /// Section for the warrior data display
    static inline void section_warriors_data();

    /// Section for the operating system display
    static inline void section_os_data();

    /// Section for the game state display
    static inline void section_game_state();

    /// Section for the game controls display
    static inline void section_game_controls();
};

}}/* ::Core::GUI */
