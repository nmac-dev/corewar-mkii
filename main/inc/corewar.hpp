/// Initialise & Run a game of corewar
#pragma once

// #define COREWAR_DEBUG

#include "settings.hpp"
#include "parser.hpp"
#include "core.hpp"

using Filenames = std::vector<std::string>;

/// Initialisation Status: used to report exceptions back to the caller
enum class GameState
{ 
    WAITING,        // waiting to be initialised
    RUNNING,        // no errors to report
    COMPLETE,       // game somplete
    ERR_INI,        // "corewar.ini" failed to load
    ERR_WARRIORS    // failed to load specified warriors from "warriors/"
};

class Corewar
{
private:
    int       m_round;              // current round number
    GameState m_state;              // current game m_status

    ASM::WarriorVec asm_warriors;  // contains all warriors in play
    OS::MARS         os_memory;     // memory array simulator
    OS::Scheduler    os_sched;      // process scheduler
    OS::Core         os_core;       // core of the operating system

    // Re-initialise Core 
    inline void resetOS()
    {
        os_memory = OS::MARS(
            &asm_warriors, 
            Settings::get().min_separation()
        );
        os_sched  = OS::Scheduler(
            &asm_warriors,
            Settings::get().max_cycles(), 
            Settings::get().max_processes()
        );
        os_core = OS::Core(&os_memory, &os_sched);
    }
    
public:
    Corewar();

    /// Return the current round
    inline int round()         const { return m_round; }
    /// Return the max rounds
    inline int max_rounds()    const { return Settings::get().max_rounds(); }
    /// Return the game's m_status
    inline GameState state() const { return m_state; }

    /// Initialise an instance of the game, ready to be run
    /// @param _filenames warrior filenames in "warrior/"
    /// @return running or error m_status
    enum GameState init(Filenames _filenames);

    /// Runs the next turn in the game, updates the games m_status
    /// @param _report buffer for the game info
    void next(OS::Report &_report);

}; /* ::Corewar */
