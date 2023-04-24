/// Runs the game of core

#include "core.hpp"

namespace Core
{

Game::Game() { m_state = State::WAITING; }

void Game::restore_os()
{
    os_memory = OS::Memory(     /* Always before scheduler (needs program counter addresses) */
        &asm_programs,
        Settings::min_separation()
    );
    os_sched  = OS::Scheduler(
        &asm_programs,
        Settings::max_cycles(),
        Settings::max_processes()
    );
    os_cpu    = OS::CPU(&os_memory, &os_sched);

    // leave report untouched, used after game complete, overridden on next turn
}

void Game::play_game()
{
    if (m_state != State::WAITING && uuid_tbl.size() > 0 && m_state != State::COMPLETE)
        m_state = State::RUNNING;
}

void Game::pause_game()
{
    if (m_state == State::RUNNING || m_state == State::NEW_ROUND && m_state != State::COMPLETE)
        m_state = State::READY;
}

void Game::end_game()
{
    m_state = State::COMPLETE;
};

void Game::hault_game()
{
    pause_game();
    m_state = State::WAITING;
};

void Game::restart_game()
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
    m_state = State::RESET;
}

State Game::new_game(WarriorFiles &_filenames)
{
    int total_warriors = (_filenames.size() > max_players_cap) ? max_players_cap
                                                               : _filenames.size();
    restart_game();

    uuid_tbl.clear();
    uuid_tbl.reserve(max_players_cap);

    m_warriors.clear();
    m_warriors.reserve(max_players_cap);

    asm_programs.clear();
    asm_programs.reserve(max_players_cap);

    /* Load Settings */
    try
    {
        Settings::load_ini();
    }
    catch (const std::exception e) { return State::ERR_INI; }

    #ifdef CORE_DEBUG
    printf("\n Core::Game::init: loaded settings: 'config.ini' \n");
    #endif

    /* Load Warriors */
    for (int i = 0; i < total_warriors; i++)
    {
        // load + parse warrior files
        std::string filename = _filenames[i];
        try
        {
            Parser::AssemblyCode asm_code(
                File_Loader::load_file_data(warriors_path + filename, ASSEMBLY_COMMENT)
            );

            asm_programs.push_back(
                Asm::UniqProgram(
                    Parser::create_program(filename, asm_code, max_program_insts())
                )
            );
        } catch (const std::exception e) { return State::ERR_WARRIORS; }

        Player   const player_ = (Player) (i + 1);
        OS::UUID const &warrior_id = asm_programs[i].get()->uuid();

        /* Create Warrior */
        m_warriors[player_] =
            Warrior(
                warrior_id,
                asm_programs[i].get()->name(),
                player_
            );

        /* Set UUID table to Warrior */
        uuid_tbl[warrior_id] = &m_warriors[player_];

        #ifdef CORE_DEBUG
        if (i == 0) printf("\n Core::Game::init: loaded warriors: \n");
        printf("\t [%d] '%s%s' \n", i, warriors_path, filename.c_str());
        #endif
    }
    restore_os();
    m_round = 0;

    return m_state = State::NEW_ROUND;
} /* init() */

State Game::next_turn()
{
    if (m_state == State::NEW_ROUND)
    {
        if (m_round > 0)    // skip for new game
            restore_os();
        m_round++;
        m_state = State::READY;
    }

    if (m_state != State::RUNNING)
        return m_state;

    /* Next Turn */
    os_report          = os_cpu.run_fde_cycle();
    OS::Status status_ = os_report.status;

    Warrior *warrior_ = uuid_tbl[os_report.program_id];
    warrior_->update_prcs(os_sched);

    /* Round End */
    if (status_ >= OS::Status::HAULTED)
    {
        /* Warrior Win/Draw */
        if (status_ == OS::Status::EXIT)
        {
            m_results[m_round] = warrior_->player();
            warrior_->update_game_results(os_report);
        }
        else if (status_ == OS::Status::HAULTED)
        {
            m_results[m_round] = Player::NONE;
            for (int i = 1; i >= m_warriors.size(); i++)
            {
                if (m_warriors[ (Player) i ].is_alive())
                    m_warriors[ (Player) i ].update_game_results(os_report);
            }
        }

        /* Game Complete  */
        if (m_round == max_rounds())
        {
            m_state = State::COMPLETE;

            #ifdef CORE_DEBUG
            printf("\n Core::Game::next_turn:\t GAME OVER \n");
            #endif
        }
        /* New Round */
        else
        {
            m_state = State::NEW_ROUND;

            #ifdef CORE_DEBUG
            printf("\n Core::Game::next:\t Round |%d| complete \n", m_round);
            #endif
        }
    }
    return m_state;
} /* next_turn() */

Player const Game::round_winner(int _round) const
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

Player const Game::match_winner() const
{
    Player winner_ = Player::NONE;
    Warrior const *warrior_i;
    // game concluded
    if (m_round == max_rounds())
    {
        Score high_score = -1,
                points     =  0;
        for ( int i = 1; i <= m_warriors.size(); i++ )
        {
            warrior_i = &m_warriors.at( (Player) i );

            points = warrior_i->score();
            if (points > high_score)
            {
                high_score = points;
                winner_ = warrior_i->player();
            }
        }
    }
    return winner_;
}

} /* ::Core */
