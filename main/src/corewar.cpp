/// Runs the game of corewar

#include "corewar.hpp"

Corewar::Corewar()
{
    m_round = 0;
    m_state = GameState::WAITING;
}

enum GameState Corewar::init(Filenames _filenames)
{
    int n_warriors = _filenames.size();

    m_round = 0;
    asm_warriors.reserve(n_warriors);

    /* Load Settings */
    try
    {
        Settings::get();
    }
    catch (const std::exception e) { return GameState::ERR_INI; }

    #ifdef COREWAR_DEBUG
    printf("\n Corewar::init: loaded settings: 'config.ini' \n");
    #endif

    /* Load & Parse Warriors */
    try
    {
        for (int i = 0; i < n_warriors; i++)
        {
            std::string filename = _filenames[i];
            // load warrior file contents
            Parser::AssemblyCode asm_code = File_Loader::load_file_data(filename, ASSEMBLY_COMMENT);

            asm_warriors.push_back(
                ASM::UniqWarrior(
                    Parser::create_warrior(filename, asm_code, Settings::get().max_warrior_len())
                )
            );

            #ifdef COREWAR_DEBUG
            if (i == 0) printf("\n Corewar::init: loaded warrior: \n");
            printf("\t [%d] '%s' \n", i, filename.c_str());
            #endif
        }
    } catch (const std::exception e) { return GameState::ERR_WARRIORS; }

    return m_state = GameState::RUNNING;
} /* ::init() */

void Corewar::next(OS::Report &_report)
{
    if (m_state != GameState::RUNNING)
        return;
    
    // Re-initialise Core
    if (m_round == 0)
    {
        resetOS();
        m_round++;
    }

    // buffer OS report
    _report = os_core.run_fde_cycle();

    // round complete
    if (_report.status >= OS::Status::HAULTED)
    {
        // check end game condition
        if (m_round >= Settings::get().max_rounds())
        {
            m_round = 0;
            m_state = GameState::COMPLETE;

            #ifdef COREWAR_DEBUG
            printf("\n Corewar::next:\t GAME OVER \n");
            #endif
        }
        else // move to next round
        {
            m_round++;
            resetOS();
            m_state = GameState::RUNNING;

            #ifdef COREWAR_DEBUG
            std::string winner = asm_warriors[_report.warrior_ID -1].get()->name().c_str();
            printf("\n Corewar::next:\t Round |%d| complete\t winner '%s' \n", 
                    m_round -1, 
                    _report.status == OS::Status::EXIT ? winner.c_str() : "Draw");
            #endif
        }    
    }
}
