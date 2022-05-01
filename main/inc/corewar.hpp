/// Initialise & Run a game of corewar

// #define COREWAR_DEBUG

#include "settings.hpp"
#include "parser.hpp"
#include "core.hpp"

using Filenames = std::vector<std::string>;

/// Initialisation Status: used to report exceptions back to the caller
enum class GameStatus 
{ 
    RUNNING,        // no errors to report
    COMPLETE,       // game somplete
    ERR_INI,        // "corewar.ini" failed to load
    ERR_WARRIORS    // failed to load specified warriors from "warriors/"
};

class Corewar
{
private:
    int              m_round;   // current round number
    GameStatus       m_status;  // current game status
    ASM::WarriorList m_warriors;  // contains all warriors in play
    OS::Core         m_core;   // core operating system

    // Re-initialise Core 
    inline void resetCore()
    {
        m_core = OS::Core(
            &m_warriors,
            Settings::get().min_separation(),
            Settings::get().max_cycles(),
            Settings::get().max_processes()
        );
    }
    
public:
    Corewar();

    /// Return the current round
    inline int round()         const { return m_round; }
    /// Return the max rounds
    inline int max_rounds()    const { return Settings::get().max_rounds(); }
    /// Return the game's status
    inline GameStatus status() const { return m_status; }

    /// Initialise an instance of the game, ready to be run
    /// @param _filenames warrior filenames in "warrior/"
    /// @return running or error status
    enum GameStatus init(Filenames _filenames);

    /// Runs the next turn in the game, updates the games status
    /// @param _report buffer for the game info
    void next(OS::Report &_report);
};
