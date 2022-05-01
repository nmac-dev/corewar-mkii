/// Runs a game of corewar debugging via CLI

// #define CLI_DEBUG

#include "corewar.hpp"

using Players = std::unordered_map<int, int>; // 

int main()
{
    Filenames filenames = { "warriors/rock.asm", "warriors/paper.asm", "warriors/scissors.asm" };

    printf("\n CLI_DEBUG::main: testing files... \n");
    for (int i = 0; i < filenames.size(); i++)
        printf("\t'%s' \n", filenames[i].c_str());

    Corewar corewar;    // instance of a game
    OS::Report report;  // contains OS FDE cycle infomation

    // check game status
    switch (corewar.init(filenames))
    {
    case GameStatus::ERR_INI:       // bad/no 'corewar.ini'
        printf("\n Error: bad/no 'corewar.ini' (EXIT_FAILURE)");
        return EXIT_FAILURE;
    
    case GameStatus::ERR_WARRIORS:  // missing warrior
        printf("\n Error: selected warriors not found or contains syntax errors (EXIT_FAILURE)");
        return EXIT_FAILURE;
    }

    int round = corewar.round();
    /* Run game */
    while (corewar.status() == GameStatus::RUNNING)
    {
        if (round != corewar.round())
        {
            round = corewar.round();
            
            #ifdef CLI_DEBUG
            printf("\n CLI_DEBUG::main: Round |%d| complete \n", round -1);
            #endif
        }
        corewar.next(report);

        // Psudo update UI
    }
    printf("\n CLI_DEBUG::main: Game complete! \n");

    return EXIT_SUCCESS;
}