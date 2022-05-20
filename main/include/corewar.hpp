/// Initialise & Run a game of corewar
#pragma once

// #define COREWAR_DEBUG

#include "settings.hpp"
#include "parser.hpp"
#include "OS/memory.hpp"
#include "OS/scheduler.hpp"
#include "OS/cpu.hpp"
#include "warrior.hpp"

namespace Corewar
{
using WarriorFiles = std::vector<std::string>;
using UUIDTable    = std::unordered_map<OS::UUID, Warrior *>;
using Warriors     = std::unordered_map<Player,   Warrior>;

/// Initialisation Status: used to report exceptions back to the caller
enum class State : int
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
    static int  constexpr max_players    = 9;

    /* Hash Tables */
    UUIDTable uuid_tbl;      // maps OS uuids to warrior references
    Warriors  m_warriors;    // maps players to warriors

    /* Stats */
    int      m_round;              // current round number
    State    m_state;              // current game state
    std::vector<Player> m_results; // tracks each rounds results

    /* Operating System */
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
    State new_game(WarriorFiles _filenames);

    /// Resets the game, ready to run again using the same warriors
    inline void reset_game()
    {
        m_round = 1;
        m_results.clear();
        m_results.resize( m_round + max_rounds() );
        m_results[m_round] = Player::NONE;            // round 0 is none
        for (auto &itr : m_warriors)
        {
            itr.second.clear_stats();
        }
        restore_os();
        m_state = State::RUNNING;
    }

    /// Runs the next turn in the game, updates the OS report with the new information
    void next_turn();

    /// Returns the player who won the last round, or the round specified
    /// @param _round override which round to return, else defaults to last round
    inline Player const round_winner(int _round = -1) const 
    {
        Player winner_ = Player::NONE;
        // default to last round
        if (_round < 0)
            _round = m_round -1;

        // bounds
        if (_round > 0  && _round <= m_round && m_round <= max_rounds())
        {
            winner_ = m_results[_round];
        }
        return winner_;
    }

    /// Returns the player who won the game overall, or None if the game has no finished
    inline Player const match_winner() const 
    {
        Player winner_ = Player::NONE;
        // game concluded
        if (m_round == max_rounds())
        {
            Warrior const *warrior_i; 
            int high_score = -1;
            int points     =  0;
            for ( int i = 1; i <= m_warriors.size(); i++ )
            {
                warrior_i = &m_warriors.at( (Player) i );

                points = (warrior_i->wins() * 2) + warrior_i->draws();
                if (points > high_score)
                {
                    high_score = points;
                    winner_ = warrior_i->player();
                }
                
            }
        }
        return winner_;
    }

    inline std::string player_to_str(Player _player)
    {
        std::string str_ = "Player[";

        str_ += std::to_string( (int) _player );
        str_.append( "] '" );
        if (_player == Player::NONE)
        {
            str_.append("None");
        }
        else str_.append(m_warriors[_player].name());

        return str_ += "'";
    }

    /// Return the current round
    inline int const round()      const { return m_round; }

    /// Return the max rounds
    inline int const max_rounds() const { return Settings::get().max_rounds(); }

 /* Warrior/Player */
    
    /// [default] Returns the executing warrior
    inline Warrior const &warrior() const
    {
        return *uuid_tbl.at(os_report.program_id);
    }

    /// Returns the warrior requested
    /// @param _uuid warrior's ID
    inline Warrior const &warrior(OS::UUID _uuid) const
    {
        return *uuid_tbl.at(_uuid);
    }

    /// Returns the warrior associated with the UUID
    /// @param _player warrior's player enum
    inline Warrior const &warrior(Player _player) const
    {
        return m_warriors.at(_player);
    }

    /// Returns the total number of players
    inline int const players() const { return m_warriors.size(); }


 /* OS::Report */

    /// Return the game state
    inline State const state() const { return m_state; }

    /// Returns report containing operating system details of the FDE cycle
    inline OS::Report const &report() const { return os_report; }
 
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
    inline std::string const assembly_at(int _adr) const
    { 
        return os_memory[_adr].to_assembly();
    }

    /// Returns the warrior's program address
    inline int const program_address(Player _player)
    {
        int adr_ = -1;
        if (_player != Player::NONE)
        {
            adr_ = asm_programs[(int) _player - 1].get()->address();
        }
        return adr_;
    }
    
    /// Returns the warrior's program length
    inline int const program_length(Player _player)
    {
        int len_ = -1;
        if (_player != Player::NONE)
        {
            len_ = asm_programs[(int) _player - 1].get()->len();
        }
        return len_;
    }

}; /* Game */

} /* ::Corewar */
