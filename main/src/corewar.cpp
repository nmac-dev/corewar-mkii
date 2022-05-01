/// Runs the game of corewar

#include "corewar.hpp"

Corewar::Corewar() = default;

enum GameStatus Corewar::init(Filenames _filenames)
{
    int n_warriors = _filenames.size();

    m_round = 0;
    m_warriors.reserve(n_warriors);

    /* Load Settings */
    try
    {
        Settings::get();
    }
    catch (const std::exception e) { return GameStatus::ERR_INI; }

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
            Parser::AssemblyCode asm_code = file_loader::getFileData(filename, ASM_CDOE_COMMENT);

            m_warriors.push_back(
                ASM::UniqWarrior(Parser::asmCodeToWarrior(filename, asm_code, Settings::get().max_warrior_len()))
            );

            #ifdef COREWAR_DEBUG
            if (i == 0) printf("\n Corewar::init: loaded warrior: \n");
            printf("\t [%d] '%s' \n", i, filename.c_str());
            #endif
        }
    }
    catch (const std::exception e) { return GameStatus::ERR_WARRIORS; }

    return GameStatus::RUNNING;
}

void Corewar::next(OS::Report &_report)
{
    // Re-initialise Core
    if (m_round == 0)
    {
        resetCore();
        m_round++;
    }

    // buffer OS report
    _report = m_core.nextFDECycle();

    // round complete
    if (_report.status == OS::Status::EXIT || _report.status == OS::Status::HAULTED)
    {
        // check end game condition
        if (m_round > Settings::get().max_rounds())
        {
            m_round = 0;
            m_status = GameStatus::COMPLETE;

            #ifdef COREWAR_DEBUG
            printf("\n Corewar::next:\t GAME OVER \n");
            #endif
        }
        else // move to next round
        {
            m_round++;
            resetCore();
            m_status = GameStatus::RUNNING;

            #ifdef COREWAR_DEBUG
            std::string winner = m_warriors[_report.warrior_ID -1].get()->getName().c_str();
            printf("\n Corewar::next:\t Round |%d| complete\t winner '%s' \n", 
                    m_round -1, 
                    _report.status == OS::Status::EXIT ? winner.c_str() : "Draw");
            #endif
        }    
    }
}