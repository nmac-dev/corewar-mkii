/// corewar.cpp (main)

// #define __COREWAR_DEBUG__

#include "settings.hpp"
// #include "parser.hpp"

int main(int argc, char const *argv[])
{
    /* Load Settings */
    try
    {
        Settings::get().loadSettings();
    }
    catch(const std::exception& e) { return EXIT_FAILURE; }

    /* Load & Parse Warriors */
    /// TODO: implement parser

    std::cout
        <<  "\tSTART"
        <<  "\n\t 1 | Load Settings \t(settings.hpp) \t[complete]"
        <<  "\n\t 2 | Load Warriors \t(parser.hpp)"
        <<  "\n\t 3 | Initialise Core \t(core_simulator.cpp)"
        <<  "\n\t 4 | Populate Core \t(loader.cpp)"
        <<  "\n\t 5 | Start Round-Robin \t(processor.cpp)"
        <<  "\n\t 6 | Track Warriors \t(scoreboard.cpp)"
        <<  "\n\t 7 | Conclude Round \t(processor.cpp)"
        <<  "\n\t 8 | REPEAT [3] \t{round != max_round}"
        <<  "\n\t 9 | Conclude Game \t(processor.cpp)"
        <<  "\n\tEND"
        << std::endl;

    return EXIT_SUCCESS;
}