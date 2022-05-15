/// Defines types for player information
#pragma once

#include <string>
#include "OS/uuid.hpp"

namespace Corewar
{
/// Enum of up to 4 players
enum class Player
{ 
    P1, // Red
    P2, // Green
    P3, // Blue
    P4  // Pink
};

/// Contains information for a corewar program
struct Program
{
 private:
    UUID        m_id;     // unique id
    std::string m_name;   // name (without directory)
    Player      m_player; // enum representing the program

    int  m_prcs,          // number of running procrsses
         m_wins;          // total wins
 public:

    /// Create new program info
    /// @param _uuid   warrior's uuid
    /// @param _name   warrior's name
    /// @param _player enum representing program
    Program(UUID _uuid, std::string _name, Player _player)
    {
        m_id     = _uuid;
        m_name   = _name;
        m_player = _player;
        m_prcs   = 0;
        m_wins   = 0;
    }
    Program() = default;

    inline UUID        const id()     const   { return m_id;     }
    inline std::string const name()     const { return m_name;     }
    inline int         const prcs()     const { return m_prcs;     }
    inline int         const wins()     const { return m_wins;     }
    inline bool        const is_alive() const { return m_prcs > 0; }

    inline void set_prcs(int _prcs) { m_prcs = _prcs; }
    inline void add_win()           { m_wins++; }

    inline void clear_stats()
    { 
        m_wins = 0;
        m_prcs = 0;
    }
};

} /* ::Corewar */
