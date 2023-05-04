/// Initialise & Run a game of core
#pragma once

// #define CORE_DEBUG

#include <list>
#include "settings.hpp"
#include "parser.hpp"
#include "memory.hpp"
#include "scheduler.hpp"
#include "cpu.hpp"
#include "warrior.hpp"

namespace Core
{
using WarriorFiles     = std::vector<std::string>;
using WarriorFilesList = std::list<std::string>;
using UUIDTable        = std::unordered_map<OS::UUID, Warrior *>;
using Warriors         = std::unordered_map<Player,   Warrior>;

/// Used to automate handling, reporting exceptions and game state
enum class State : int
{
    ERR_WARRIORS,   // failed to load specified warrior programs
    ERR_INI,        // failed to load configuration file
    WAITING,        // waiting for command (OS is not initialised)
    RESET,          // game has been reset
    READY,          // game paused and ready to run
    NEW_ROUND,      // a new round has started (Reset -> Ready)
    RUNNING,        // game running, OS is reporting data
    COMPLETE,       // game complete, requires restart
};

class Game
{
 private:
    static char constexpr warriors_path[] = "warriors/";
    static int  constexpr max_players_cap = 9;  // max players capacity

    /* Hash Tables */
    UUIDTable uuid_tbl;   // maps OS uuids to warrior references
    Warriors  m_warriors; // maps players to warriors

    /* Stats */
    int   m_round;                 // current round number
    State m_state;                 // current game state
    std::vector<Player> m_results; // tracks each rounds results

    /* Operating System */
    Asm::ProgramVec asm_programs;  // contains all assembly programs
    OS::Memory      os_memory;     // memory array simulator
    OS::Scheduler   os_sched;      // process scheduler
    OS::CPU         os_cpu;        // cpu of the operating system
    OS::Report      os_report;     // operating system details of the FDE cycle

    /// Restore operating system to default
    void restore_os();

 public:
    Game();

    static inline char const *warriors_directory() { return warriors_path;   }
    static inline int  constexpr max_players()     { return max_players_cap; }

 /* Game State */

    /// Sets the game state to running, if inititalised
    void play_game();

    /// Pause the game by setting the state to ready
    void pause_game();

    /// Sets the game state to complete, requires new game or reset after
    void end_game();

    /// Haults the game, same as pause but the state is set to waiting
    void hault_game();

    /// Creates a new game using the warrior files (wipes previous game state)
    /// @param _filenames warrior (program) filenames to load ("warrior/")
    State new_game(WarriorFiles &_filenames);

    /// Resets the game, ready to run again using the same warriors
    void restart_game();

    /// Runs the next turn in the game, updates the OS report with the new information
    State next_turn();

    /// Return the current round
    inline int const &round() const { return m_round; }

    /// Return the max rounds
    inline int const &max_rounds() const { return Settings::max_rounds(); }

 /* Warrior Utility */

    /// [default] Returns the executing warrior
    inline Warrior const &warrior() const { return *uuid_tbl.at(os_report.program_id); }

    /// Returns the warrior requested
    /// @param _uuid warrior's ID
    inline Warrior const &warrior(OS::UUID _uuid) const { return *uuid_tbl.at(_uuid); }

    /// Returns the warrior associated with the UUID
    /// @param _player warrior's player enum
    inline Warrior const &warrior(Player _player) const { return m_warriors.at(_player); }

    /// Returns the full string of the warrior
    inline std::string warrior_string(Player _player) const
    {
        return (_player != Player::NONE) ? m_warriors.at(_player).to_string()
                                         : "P0 '/None/'";
    }

 /* Player Utility */

    /// Returns the total number of players
    inline int const players() const { return m_warriors.size(); }

    /// Returns the player who won the last round, or the round specified
    /// @param _round override which round to return, else defaults to last round
    Player const round_winner(int _round = -1) const;

    /// Returns the player who won the game overall, or None if the game has no finished
    Player const match_winner() const;

 /* OS::Report */

    /// Return the game state
    inline State const &state() const { return m_state; }

    /// Returns report containing operating system details of the FDE cycle
    inline OS::Report const &report() const { return os_report; }

 /* OS::Scheduler */

    /// Returns active programs in execution
    inline int const &active_programs() const { return os_sched.programs();  }

    /// Returns number of executed cycles
    inline int const &cycles()     const { return os_sched.cycles(); }

    /// Returns max allowed execution cycles (.ini)
    inline int const &max_cycles() const { return os_sched.max_cycles(); }

    /// Returns total processes executing
    inline int const &total_processes() const { return os_sched.processes(); }

    /// Returns max allowed processes for a single programs (.ini)
    inline int const &max_processes()   const { return os_sched.max_processes(); }

 /* OS::Memory */

    /// Returns a string of the assembly instruction at the address
    inline std::string const assembly_at(int _adr) const { return os_memory[_adr].to_assembly(); }

    /// Returns the warrior's program address
    inline int const program_address(Player _player)
    {
        int adr_ = -1;
        if (_player != Player::NONE)
            adr_ = asm_programs[(int) _player - 1].get()->address();

        return adr_;
    }

    /// Returns the warrior's program length
    inline int const program_length(Player _player)
    {
        int len_ = -1;
        if (_player != Player::NONE)
            len_ = asm_programs[(int) _player - 1].get()->len();

        return len_;
    }

    /// Returns the size of the OS's RAM
    static inline int constexpr memory_size()   { return OS::Memory::size();            }

    /// Returns min seperation between programs in memory (.ini)
    inline int const &min_separation()    const { return Settings::min_separation();    }

    /// Returns max instructions allows in a single programs (.ini)
    inline int const &max_program_insts() const { return Settings::max_program_insts(); }

}; /* Game */

} /* ::Core */
