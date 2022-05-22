/// GUI display for loading warrior files and tracking stats
#include "control_panel.gui.hpp"

namespace Corewar { namespace GUI
{
void ControlPanel::init(Game *_corewar)
{
    ptr_corewar = _corewar;

    ImGui::GetStyle().FrameRounding = 2.f;

    reload_files();

    // initilaise memory display
    if (MemoryViewer::init_flag() == false)
    {
        MemoryViewer::init( _corewar );
    }
    m_init_flag  = true;
}

void ControlPanel::run_corewar_systems()
{
    if ( !init_flag())
        return;    

    if (ptr_corewar->state() == State::NEW_ROUND)
        new_round_flag = true;

    ptr_corewar->next_turn();
    MemoryViewer::draw();
}

void ControlPanel::reset()
{
    ptr_corewar->restart_game();
    MemoryViewer::reset();
}

void ControlPanel::reload_files()
{
    static std::string path_ = ptr_corewar->warriors_directory();

    warrior_stats.clear();
    warrior_stats.reserve(Game::max_players());

    dir_files.clear();
    dir_files.reserve(Game::max_players());

    // load all files in 'warriors/'
    for (auto const &filename : FileSys::directory_iterator(path_))
    {
        dir_files.push_back(filename.path().filename().string());
    }
    select_indexes.clear();
    select_indexes.resize( (dir_files.size() < Game::max_players()) ? dir_files.size()
                                                                    : Game::max_players()
    );
    clear_selected();

    ptr_corewar->hault_game();

    // Memory cannot load empty game
    if (m_init_flag)
        MemoryViewer::reset();

    warriors_locked = false;
}

void ControlPanel::load_selected()
{
    if ( !init_flag())
        return;
    
    WarriorFiles filename_;
    filename_.reserve(Game::max_players());

    /* New Game w/ Selected */
    for (int i = 0; i < select_indexes.size(); i++)
    {
        for (int count = select_indexes[i]; count >= 1; count--)
        {
            filename_.push_back(
                dir_files[ i ]
            );
        }
    }
    if (filename_.size() <= 0)
        return;

    ptr_corewar->new_game(filename_);
    MemoryViewer::reset();

    /* Load Initial Stats */
    for (int plr = 1; plr <= filename_.size(); plr++)
    {
        Warrior const &warrior_ = ptr_corewar->warrior( (Player) plr);

        warrior_stats.push_back(
            {
                warrior_.player(),
                &warrior_.id(),
                &warrior_.name(),
                &warrior_.prcs(),
                &warrior_.score(),
            }
        );
    }
    clear_selected();
    warriors_locked = true;
}

}}/* ::Corewar::GUI */
