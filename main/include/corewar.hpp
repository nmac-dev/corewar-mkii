/// Initialise & Run a game of corewar
#pragma once

// #define COREWAR_DEBUG

#include "settings.hpp"
#include "parser.hpp"
#include "OS/cpu.hpp"
#include "player.hpp"

namespace Corewar
{
using ProgramFiles = std::vector<std::string>;
using Programs     = std::unordered_map<UUID, Program>;

/// Initialisation Status: used to report exceptions back to the caller
enum class State
{ 
    WAITING,        // waiting to be initialised
    RUNNING,        // no errors to report
    COMPLETE,       // game somplete
    ERR_INI,        // "corewar.ini" failed to load
    ERR_WARRIORS    // failed to load specified warriors from "warriors/"
};

class Game
{
 private:
    static char constexpr warriors_dir[] = "warriors/";
    static int  constexpr max_players    = 4;

    int      m_round;              // current round number
    State    m_state;              // current game state
    Programs m_programs;           // hash table of player programs 

    ASM::WarriorVec asm_warriors;  // contains all warriors assembly
    OS::Memory      os_memory;     // memory array simulator
    OS::Scheduler   os_sched;      // process scheduler
    OS::CPU         os_cpu;        // cpu of the operating system
    OS::Report      os_report;     // operating system details of the FDE cycle

    /// Restore operating system to default 
    inline void restore_os()
    {
        os_memory = OS::Memory(
            &asm_warriors, 
            Settings::get().min_separation()
        );
        os_sched  = OS::Scheduler(
            &asm_warriors,
            Settings::get().max_cycles(), 
            Settings::get().max_processes()
        );
        os_cpu    = OS::CPU(&os_memory, &os_sched);
        
        // leave report untouched, used after game complete, overridden on next turn
    }

 public:
    Game();

 /* Functions */

    /// Creates a new game using the program files (wipes previous game state)
    /// @param _filenames warrior (program) filenames to load ("warriors/")
    enum State new_game(ProgramFiles _filenames);

    /// Resets the game, ready to run again using the same programs
    inline void reset_game()
    {
        m_round = 0;
        for (auto itr : m_programs)
        {
            itr.second.clear_stats();
        }
        restore_os();
        m_state = State::RUNNING;
    }

    /// Runs the next turn in the game, updates the report with the new information
    /// @param _buffer recieves a copy of the program information
    void next_turn(Program *_buffer);

 /* Utility */

    /// Return the current round
    inline int round() const { return m_round; }

    /// Return the max rounds
    inline int max_rounds() const { return Settings::get().max_rounds(); }
    
    /// Return the game state
    inline State state() const { return m_state; }

    /// Returns report containing operating system details of the FDE cycle
    inline OS::Report const report() const { return os_report; }

    /// Returns number of executed cycles
    inline int const cycles() { return os_sched.cycles(); }

    /// Returns active programs in execution
    inline int const active_programs() { return os_sched.active(); }

    /// Returns total processes executing
    inline int const total_processes() { return os_sched.processes(); }

}; /* Game */

} /* ::Corewar */
