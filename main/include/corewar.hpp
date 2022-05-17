/// Initialise & Run a game of corewar
#pragma once

#define COREWAR_DEBUG

#include "settings.hpp"
#include "parser.hpp"
#include "OS/memory.hpp"
#include "OS/scheduler.hpp"
#include "OS/cpu.hpp"
#include "warrior.hpp"

namespace Corewar
{
using WarriorFiles = std::vector<std::string>;
using Warriors     = std::unordered_map<UUID, Warrior>;

/// Initialisation Status: used to report exceptions back to the caller
enum class State
{ 
    WAITING,        // waiting to be initialised
    RUNNING,        // no errors to report
    COMPLETE,       // game somplete
    ERR_INI,        // "corewar.ini" failed to load
    ERR_WARRIORS    // failed to load specified warriors
};

class Game
{
 private:
    static char constexpr warriors_dir[] = "warriors/";
    static int  constexpr max_players    = 6;

    int      m_round;              // current round number
    State    m_state;              // current game state
    Warriors m_warriors;           // hash table of warriors 

    Asm::ProgramVec asm_programs;  // contains all assembly programs
    OS::Memory      os_memory;     // memory array simulator
    OS::Scheduler   os_sched;      // process scheduler
    OS::CPU         os_cpu;        // cpu of the operating system
    OS::Report      os_report;     // operating system details of the FDE cycle

    /// Restore operating system to default 
    inline void restore_os()
    {
        os_memory = OS::Memory(     /* Always before scheduler (needs program counter addresses) */
            &asm_programs, 
            Settings::get().min_separation()
        );
        os_sched  = OS::Scheduler(
            &asm_programs,
            Settings::get().max_cycles(), 
            Settings::get().max_processes()
        );
        os_cpu    = OS::CPU(&os_memory, &os_sched);
        
        // leave report untouched, used after game complete, overridden on next turn
    }

 public:
    Game();

 /* Functions */

    /// Creates a new game using the warrior files (wipes previous game state)
    /// @param _filenames warrior (program) filenames to load ("warrior/")
    enum State new_game(WarriorFiles _filenames);

    /// Resets the game, ready to run again using the same warriors
    inline void reset_game()
    {
        m_round = 0;
        for (auto itr : m_warriors)
        {
            itr.second.clear_stats();
        }
        restore_os();
        m_state = State::RUNNING;
    }

    /// Runs the next turn in the game, updates the report with the new information
    /// @param _buffer recieves a copy of the warrior information
    void next_turn(Warrior *_buffer);

    /// Return the current round
    inline int const round() const { return m_round; }

    /// Return the max rounds
    inline int const max_rounds() const { return Settings::get().max_rounds(); }

    /* Player */


 /* OS::Report */

    /// Return the game state
    inline State const state() const { return m_state; }

    /// Returns report containing operating system details of the FDE cycle
    inline OS::Report const report() const { return os_report; }
 
 /* OS::Scheduler */

    /// Returns number of executed cycles
    inline int const cycles() const { return os_sched.cycles(); }

    /// Returns active warriors in execution
    inline int const active_warriors() const { return os_sched.programs(); }

    /// Returns total processes executing
    inline int const total_processes() const { return os_sched.processes(); }

 /* OS::Memory */

    /// Returns the size of the OS's RAM
    inline int constexpr core_size() const { return os_memory.size(); }

    /// Returns a string of the assembly instruction at the address
    inline std::string const assembly_at(int _pos) const
    { 
        return os_memory[os_report.exe.address].to_assembly();
    }

}; /* Game */

} /* ::Corewar */
