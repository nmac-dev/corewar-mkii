/// Defines types for player information
#pragma once

#include <string>
#include "uuid.hpp"
#include "scheduler.hpp"
#include "report.hpp"

namespace Core
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
using Score = int;

/// Contains information for a core warrior (warrior)
class Warrior
{
 private:
    OS::UUID const *m_id;     // unique id
    std::string     m_name;   // name (without directory)
    Player          m_player; // enum representing the warrior

    /* Game Stats */
    int   m_prcs;         // number of running procrsses
    Score m_score;        // current number of points scored
 public:

    /// Create new warrior info
    /// @param _uuid   warrior's uuid
    /// @param _name   warrior's name
    /// @param _player enum representing warrior
    Warrior(OS::UUID const &_uuid, std::string _name, Player _player)
    {
        m_id     = &_uuid;
        m_name   = _name;
        m_player = _player;
        clear_stats();
    }
    Warrior() = default;

 /* Utility */

    /// Returns warriors unique ID
    inline OS::UUID const &id()      const { return *m_id;       }

    /// Returns warrior's name
    inline std::string const &name() const { return m_name;      }

    /// Returns enum of the warrior's play
    inline Player const &player()    const { return m_player;    }

    /// Returns current number of processes
    inline int const &prcs()         const { return m_prcs;      }

    /// Returns current number of points scored
    inline Score const &score()      const { return m_score;      }

    /// Returns false: when all the warrior's processes have been killed
    inline bool const is_alive()     const { return m_prcs > 0;  }

    /// Returns the player as a string
    inline std::string player_string() const
    {
        return std::string ("P" + std::to_string( (int) m_player ));
    }

    inline std::string to_string() const
    {
        return std::string( player_string() + " '" + name() + "'" );
    }

    /// Updates the processes using the Game's scheduler
    inline void update_prcs(OS::Scheduler &_sched) { m_prcs = _sched.processes(*m_id); }

    /// Updates game stats based on the final report
    inline void update_game_results(OS::Report &_report)
    {
        switch (_report.status)
        {
            case OS::Status::EXIT:    m_score += 2;  break;  // wins are worth double
            case OS::Status::HAULTED: m_score++;     break;
            default: break;
        }
    }

    /// Clears the warriors stats ready for a new game
    inline void clear_stats()
    {
        m_prcs = m_score = 0;
    }
};

} /* ::Core */
