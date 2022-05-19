/// Defines types for player information
#pragma once

#include <string>
#include "OS/uuid.hpp"
#include "OS/scheduler.hpp"
#include "OS/report.hpp"

namespace Corewar
{
/// Enum to represent each player
enum class Player : int
{ 
    NONE,   // Grey
    P1,     // Red
    P2,     // Blue
    P3,     // Green
    P4,     // Pink
    P5,     // Cyan
    P6,     // Orange
    P7,     // Teal
    P8,     // Yellow
    P9,     // Purple
};

/// Contains information for a corewar warrior (warrior)
class Warrior
{
 private:
    OS::UUID    m_id;     // unique id
    std::string m_name;   // name (without directory)
    Player      m_player; // enum representing the warrior

    /* Game Stats */
    int  m_prcs,          // number of running procrsses
         m_wins,          // total wins
         m_draws;         // Total draws
 public:

    /// Create new warrior info
    /// @param _uuid   warrior's uuid
    /// @param _name   warrior's name
    /// @param _player enum representing warrior
    Warrior(OS::UUID _uuid, std::string _name, Player _player)
    {
        m_id     = _uuid;
        m_name   = _name;
        m_player = _player;
        clear_stats();
    }
    Warrior() = default;

 /* Utility */

    /// Returns warriors unique ID
    inline OS::UUID const id()      const { return m_id;        }

    /// Returns warrior's name
    inline std::string const name() const { return m_name;      }

    /// Returns enum of the warrior's play
    inline Player const player()    const { return m_player;    }
    
    /// Returns current number of processes
    inline int const prcs()         const { return m_prcs;      }
    
    /// Returns total wins
    inline int const wins()         const { return m_wins;      }

    /// Returns total draws
    inline int const draws()        const { return m_draws;     }

    /// Returns false: when all the warrior's processes have been killed
    inline bool const is_alive()    const { return m_prcs > 0;  }

    /// Updates the processes using the Game's scheduler
    inline void update_prcs(OS::Scheduler &_sched)
    {
         m_prcs = _sched.processes(m_id);
    }

    /// Updates game stats based on the final report
    inline void update_game_results(OS::Report &_report)
    {
        switch (_report.status)
        {
            case OS::Status::EXIT:    m_wins++;   break;
            case OS::Status::HAULTED: m_draws++;  break;
            default: break;
        }
    }

    /// Clears the warriors stats ready for a new game
    inline void clear_stats()
    { 
        m_prcs = m_draws = m_wins = 0;
    }
};

} /* ::Corewar */
