/// Runs the game of corewar

#include "corewar.hpp"

namespace Corewar
{

Game::Game()
{
    m_round = 0;
    m_state = State::WAITING;
}

enum State Game::new_game(WarriorFiles _filenames)
{
    int n_warriors = (_filenames.size() > max_players) ? max_players
                                                       : _filenames.size();
    m_round = 0;
    m_state = State::WAITING;
    m_warriors.clear();
    asm_programs.clear();
    asm_programs.reserve(n_warriors);

    /* Load Settings */
    try
    {
        Settings::get();
    }
    catch (const std::exception e) { return State::ERR_INI; }

    #ifdef COREWAR_DEBUG
    printf("\n Corewar::Game::init: loaded settings: 'config.ini' \n");
    #endif

    /* Load Warriors */
    Player player_;
    UUID   warrior_id;
    for (int i = 0; i < n_warriors; i++)
    {
        // load + parse warrior files
        std::string filename = _filenames[i];
        try
        {
            Parser::AssemblyCode asm_code(
                File_Loader::load_file_data(warriors_dir + filename, ASSEMBLY_COMMENT)
            );

            asm_programs.push_back(
                Asm::UniqProgram(
                    Parser::create_program(filename, asm_code, Settings::get().max_program_insts())
                )
            );
        } catch (const std::exception e) { return State::ERR_WARRIORS; }
        warrior_id = asm_programs[i].get()->uuid();

        // select player
        switch (i)
        {
            case 0: player_ = Player::P1; break;
            case 1: player_ = Player::P2; break;
            case 2: player_ = Player::P3; break;
            case 3: player_ = Player::P4; break;
            case 4: player_ = Player::P5; break;
            case 5: player_ = Player::P6; break;
            default: break;
        }

        // set warrior info
        m_warriors[warrior_id] = Warrior(
            warrior_id,
            asm_programs[i].get()->name(),
            player_
        );

        #ifdef COREWAR_DEBUG
        if (i == 0) printf("\n Corewar::Game::init: loaded warriors: \n");
        printf("\t [%d] '%s%s' \n", i, warriors_dir, filename.c_str());
        #endif
    }
    restore_os();

    return m_state = State::RUNNING;
} /* init() */

void Game::next_turn(Warrior *_buffer)
{
    /* Next Turn */
    os_report         = os_cpu.run_fde_cycle();
    Warrior *warrior_ = &m_warriors[os_report.program_id];

    // update processes
    warrior_->update_prcs(os_sched);

    /* Round End */
    if (m_round == 0 || os_report.status >= OS::Status::HAULTED)
    {
        /* Warrior Win/Draw */
        if (os_report.status == OS::Status::EXIT)
        {
            warrior_->update_game_results(os_report);
        }
        else if (os_report.status == OS::Status::HAULTED)
        {
            for (int i = 0; i < m_warriors.size(); i++)
            {
                if (m_warriors[i].is_alive())
                    m_warriors[i].update_game_results(os_report);
            }
        }

        /* Game Complete  */
        if (m_round >= Settings::get().max_rounds())
        {
            m_state = State::COMPLETE;

            #ifdef COREWAR_DEBUG
            printf("\n Corewar::Game::next_turn:\t GAME OVER \n");
            #endif
        }
        /* New Round */
        else
        {
            restore_os();
            m_round++;

            #ifdef COREWAR_DEBUG
            printf("\n Corewar::Game::next:\t Round |%d| complete\t winner '%s' \n", 
                    m_round, 
                    os_report.status == OS::Status::EXIT ? warrior_->name().c_str() : "Draw");
            #endif
        }
    }
    (*_buffer) = (*warrior_);
}

} /* ::Corewar */
