/// Runs a game of corewar debugging via CLI

#define CLI_OUTPUT_RATE 1000 // print every X cycles

// #define CLI_TOTALS
// #define CLI_WARRIORS

#include "corewar.hpp"

int main()
{
    char constexpr rock[]     = "rock.asm",
                   paper[]    = "paper.asm",
                   scissors[] = "scissors.asm";

    Corewar::ProgramFiles filenames = { rock, paper, scissors, scissors};

    printf("\n CLI: warrior files... \n");
    for (int i = 0; i < filenames.size(); i++)
        printf("\t'%s' \n", filenames[i].c_str());

    Corewar::Game corewar;      // instance of a game

    // check game status
    switch (corewar.new_game(filenames))
    {
        case Corewar::State::ERR_INI:       // bad/missing 'corewar.ini'
            printf("\n Error: bad/missing 'corewar.ini' (EXIT_FAILURE)");
            return EXIT_FAILURE;
        
        case Corewar::State::ERR_WARRIORS:  // missing warrior
            printf("\n Error: selected warriors missing or contain invalid assembly (EXIT_FAILURE)");
            return EXIT_FAILURE;
    }

    Corewar::Program exe_program; // executing program
    /* Run game */
    int round = corewar.round();
    while (corewar.state() == Corewar::State::RUNNING)
    {
        corewar.next_turn(&exe_program);
        
        if (round != corewar.round())
        {
            round = corewar.round();
            
            if (corewar.report().status == OS::Status::EXIT)
            {
                printf("Round |%d| finished! ... Result: 'Win' -> Warrior: [%d]'%s' \n", 
                        corewar.round(), 
                        exe_program.id(),
                        exe_program.name().c_str()
                );
            } else printf("Round |%d| finished! ... Result: 'Draw' \n", corewar.round());
        }

        /* Psudo: "update UI" */
        #ifdef CLI_WARRIORS
        if ( (corewar.cycles() % CLI_OUTPUT_RATE) == 0 )
        {
            printf("\n CLI_WARRIORS:: ID:[%d] Prcs:|%d| Wins|%d| Name:'%s' \n",
                exe_program.id(),
                exe_program.prcs(),
                exe_program.wins(),
                exe_program.name().c_str());
        }
        #endif

        #ifdef CLI_TOTALS
        if ( (corewar.cycles() % CLI_OUTPUT_RATE) == 0 )
        {
            printf("\n CLI_TOTALS:: Active:|%d| Total_Prcs:|%d| Cycles:|%d|  \n",
                corewar.active_programs(),
                corewar.total_processes(),
                corewar.cycles());
        }
        #endif
    }

    printf("\n Game complete! ... Winner -> Warrior: [%d]'%s' Prcs:|%d| Wins|%d| \n",
        exe_program.id(),
        exe_program.name().c_str(),
        exe_program.prcs(),
        exe_program.wins()
    );

    return EXIT_SUCCESS;
}
