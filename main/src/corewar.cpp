/// Runs the game of corewar (main)

// #define COREWAR_DEBUG
#define COREWAR_DEBUG_FAKE_ARGS

#include "settings.hpp"
#include "parser.hpp"
#include "core.hpp"

// namespace { using Players = std::vector<std::unique_ptr<ASM::Warrior>>; }

int main(int argc, char const *argv[])
{
    int n_warriors = argc -1;
    char const **files = argv;

    #ifdef COREWAR_DEBUG_FAKE_ARGS
        int         fake_argc = 3;
        char const *fake_argv[] = {"warriors/rock.asm", "warriors/paper.asm", "warriors/scissors.asm"};

        n_warriors = fake_argc;
        files      = fake_argv;

        printf("\nmain()::COREWAR_DEBUG_FAKE_ARGS: testing files... \n");
        for (int i = 0; i < n_warriors; i++) 
            printf("\t'%s' \n", files[i]);
    #endif

    ASM::WarriorList warriors;      // Stores warriors objects for the core
    warriors.reserve(n_warriors);

    /* Load Settings */
    try 
    {
        Settings::get();
    }
    catch(const std::exception& e) { return EXIT_FAILURE; }

    #ifdef COREWAR_DEBUG
        printf("\nmain():: loaded settings: 'config.ini' \n");
    #endif

    /* Load & Parse Warriors */
    try
    {
        for (int i = 0; i < n_warriors; i++)
        {
            std::string filename = files[i];
            // load warrior file contents
            Parser::AssemblyCode asm_code = file_loader::getFileData(filename, ASM_CDOE_COMMENT);

            warriors.push_back(
                Parser::asmCodeToWarrior(filename, asm_code, Settings::get().max_warrior_len())
            );


            #ifdef COREWAR_DEBUG
                if (i == 0) printf("\nmain():: loaded warrior: \n");
                printf("\t [%d] '%s' \n", i, filename.c_str());
            #endif
        }
    }
    catch(const std::exception& e) { return EXIT_FAILURE; }

    /* Initialise Core Operating System  */
    OS::Core core_os(
        warriors, 
        Settings::get().core_size(), 
        Settings::get().min_separation(),
        Settings::get().max_rounds(),
        Settings::get().max_cycles()
    );
    
    #ifdef COREWAR_DEBUG
        printf("\nmain():: initialise core: \n"
               "\t MARS size: %d \n\t Scheduler processes: %d", core_os.size(), core_os.n_processes()
        );
    #endif

    //// TODO:
    /* Run Core (MARS & Scheduler) Operating System */    
    OS::Core::Report core_report;
    core_report = core_os.nextFDECycle();
    core_report = core_os.nextFDECycle();
    core_report = core_os.nextFDECycle();

    /* Completed Tasks */
    std::cout
        <<  "\n\tSTART"
        <<  "\n\t 1 | Load Settings     \t(settings.hpp)            [complete]"
        <<  "\n\t 2 | Load Warriors     \t(parser.hpp)              [complete]"
        <<  "\n\t 3 | Initialise Core   \t(MARS.cpp & schedule.cpp) [complete]"
        <<  "\n\t 4 | Setup/Start Game  \t(core.cpp)                [todo]"
        <<  "\n\t 6 | REPEAT [3]        \t{round != max_round}      [todo]"
        <<  "\n\t 7 | Conclude Game     \t(corewar.cpp)             [todo]"
        <<  "\n\tEND"
        << std::endl;

    return EXIT_SUCCESS;
}