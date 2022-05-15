/// Runs the game of corewar

#include "corewar.hpp"

namespace Corewar
{

Game::Game()
{
    m_round = 0;
    m_state = State::WAITING;
}

enum State Game::new_game(ProgramFiles _filenames)
{
    int n_warriors = (_filenames.size() > max_players) ? max_players
                                                       : _filenames.size();
    m_round = 0;
    m_state = State::WAITING;
    m_programs.clear();
    asm_warriors.clear();
    asm_warriors.reserve(n_warriors);

    /* Load Settings */
    try
    {
        Settings::get();
    }
    catch (const std::exception e) { return State::ERR_INI; }

    #ifdef COREWAR_DEBUG
    printf("\n Corewar::Game::init: loaded settings: 'config.ini' \n");
    #endif

    /* Load Programs */
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

            asm_warriors.push_back(
                ASM::UniqWarrior(
                    Parser::create_warrior(filename, asm_code, Settings::get().max_warrior_len())
                )
            );
        } catch (const std::exception e) { return State::ERR_WARRIORS; }

        warrior_id = asm_warriors[i].get()->uuid();
        switch (i)
        {
            case 0: player_ = Player::P1; break;
            case 1: player_ = Player::P2; break;
            case 2: player_ = Player::P3; break;
            case 3: player_ = Player::P4; break;
            default: break;
        }

        // set program info
        m_programs[warrior_id] = Program(
            warrior_id,
            asm_warriors[i].get()->name(),
            player_
        );

        #ifdef COREWAR_DEBUG
        if (i == 0) printf("\n Corewar::Game::init: loaded warriors: \n");
        printf("\t [%d] '%s%s' \n", i, warriors_dir, filename.c_str());
        #endif
    }
    restore_os();

    return m_state = State::RUNNING;
} /* ::init(ProgramFiles ) */

void Game::next_turn(Program *_buffer)
{
    /* Next Turn */
    os_report         = os_cpu.run_fde_cycle();
    Program *program_ = &m_programs[os_report.warrior_id];

    // update processes
    (*program_).set_prcs(os_sched.processes( (*program_).id() ));

    /* Round End */
    if (m_round == 0 || os_report.status >= OS::Status::HAULTED)
    {
        /* Program Won */
        if (os_report.status == OS::Status::EXIT)
            (*program_).add_win();

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
                    os_report.status == OS::Status::EXIT ? program_.name().c_str() : "Draw");
            #endif
        }
    }
    (*_buffer) = (*program_);
}

} /* ::Corewar */
