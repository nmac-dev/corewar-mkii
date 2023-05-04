/// Defines all the sub-sections of the ControlPanel
#include "control_panel.gui.hpp"

namespace Core { namespace GUI
{
inline void ControlPanel::select_file(int _index)
{
    if ( !init_flag() || _index < 0 || _index >= select_indexes.size())
        return;

    if (total_selected < Game::max_players())
    {
        select_indexes[_index]++;
        total_selected++;
    }
}

inline void ControlPanel::section_master_controls()
{
    ImVec2 item_size   = ImGui::GetContentRegionAvail();
           item_size.x *= 0.49f;
           item_size.y = 0.0f;

    ImGui::BeginGroup();
    {
        ImGui::Text("Master Controls");       ImGui::SameLine();
            ImGui::TextDisabled("(Help?)");
        if (ImGui::IsItemHovered())
        {
            ImGui::BeginTooltip();
            ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
            ImGui::TextUnformatted(
                "Be careful with these controls!\n"
                " * Restart: Resets the game to round 1 (warriors must be loaded)\n"
                " * Reload:  Select files to use from the 'warriors/' folder\n"
            );
            ImGui::PopTextWrapPos();
            ImGui::EndTooltip();
        }
        ImGui::Separator();
     /** MASTER:CONTROLS: ~~~// Restart Game | Reload Warriors //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
        ImVec4 color_ = PLR_COLORS.at(Player::P6);  adjust_color(color_, -0.25f);
        int push_     = push_btn_color(color_);

        if(ImGui::Button("Restart Game", item_size) && m_init_flag)
            if (warriors_locked)
                reset();  // game must be in play
            if (ImGui::IsItemHovered())
                ImGui::SetTooltip("Warning: progress will be wiped");

        ImGui::SameLine();
        if (ImGui::Button("Reload Warriors", item_size))
            reload_files();
            if (ImGui::IsItemHovered())
                ImGui::SetTooltip("Select programs from the 'warriors/' folder (also starts a new game)");

        ImGui::PopStyleColor(push_);
    }
    ImGui::EndGroup();
}

inline void ControlPanel::section_loader_menu()
{
    ImVec2 item_size    = ImGui::GetContentRegionAvail();
           item_size.x *= 0.49f;
           item_size.y  = 0.0f;

    ImGui::Text("Warrior Loader");       ImGui::SameLine();
        ImGui::TextDisabled("(Help?)");
    if (ImGui::IsItemHovered())
    {
        ImGui::BeginTooltip();
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted(
            "Here you can select all the warrior programs you want to run\n"
            " * Warriors are loaded from the 'warrior/' folder\n"
            " * The same warrior can be selected multiple times\n"
            " * |2| warriors is the minimum required"
            " * You can select up to |9| total\n"
        );
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
    ImGui::Separator();
 /** CONTROLS: ~~// Load Selected | Clear Selected //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    ImVec4 color_;
    int push_;

    ImGui::BeginGroup();
    {
        color_ = PLR_COLORS.at(Player::P3);  adjust_color(color_, -0.35f);
        push_  = push_btn_color(color_);

        if(ImGui::Button("Load Selected", item_size))
            if (total_selected > 1 && total_selected <= ptr_core->max_players())
            {
                load_selected();
            }
            if (ImGui::IsItemHovered())
                ImGui::SetTooltip("Load selected programs (min: 2)  (max: 9) ");

        ImGui::PopStyleColor(push_);

        ImGui::SameLine();
        if (ImGui::Button("Clear Selected", item_size))
            {
                clear_selected();
            }
            if (ImGui::IsItemHovered())
                ImGui::SetTooltip("Clear program selection");
    }
    ImGui::EndGroup();

 /** FILES:TABLE: ~~// Filename | no. //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    if (ImGui::BeginTable("FILES:TABLE", 2, ImGuiTableFlags_NoSavedSettings))
    {
        item_size.x *= 0.15f;
        ImGui::TableSetupColumn(" 'warriors/'", ImGuiTableColumnFlags_WidthStretch, -1.f );
        ImGui::TableSetupColumn("No.",          ImGuiTableColumnFlags_WidthFixed,   item_size.x );

     /* Show files */
        ImGui::TableHeadersRow();
        for (int row = 0; row < dir_files.size(); row++)
        {
            ImGui::TableNextRow();
            {
                ImGui::TableSetColumnIndex(0); // Filename
                    if (ImGui::Button(dir_files[row].c_str(), ImVec2(-1.f, 0.f)))  // add selected on click
                        if (total_selected <= Game::max_players())
                            select_file(row);

                ImGui::TableSetColumnIndex(1); // No.
                    ImVec4 color_ = PLR_COLORS.at( (Player) select_indexes[row] );
                    ImGui::TextColored(color_,std::to_string( select_indexes[row] ).c_str());
            }
        }
    }
    ImGui::EndTable();
}

inline void ControlPanel::section_warriors_data()
{
    static std::string item_txt  = "";
 /** WARRIOR:STATS: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    ImGui::Text("Warrior Data");       ImGui::SameLine();
        ImGui::TextDisabled("(Help?)");
    if (ImGui::IsItemHovered())
    {
        ImGui::BeginTooltip();
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted(
            "A live feed of warrior data\n"
            " * If a warrior is 'grey' they have been killed\n"
            "P#    : Player Number\n"
            "ID    : Operating System UUID of the warrior (program)\n"
            "Prcs  : Number of running processes\n"
            "Score : Overall score . . . Win (+ 2) | Draw (+ 1)\n"
        );
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
    ImGui::Separator();
 /** STATS:TABLE: ~~// # (Player )| ID | Name | Prcs | Score //~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    if (ImGui::BeginTable("WARRIORS:TABLE", 5, ImGuiTableFlags_NoSavedSettings))
    {
        ImGui::TableSetupColumn(" P#",     ImGuiTableColumnFlags_WidthFixed,   -1.f );
        ImGui::TableSetupColumn("Name",    ImGuiTableColumnFlags_WidthStretch, -1.f );
        ImGui::TableSetupColumn("ID\t",    ImGuiTableColumnFlags_WidthFixed,   -1.f );
        ImGui::TableSetupColumn("Prcs\t",  ImGuiTableColumnFlags_WidthFixed,   -1.f );
        ImGui::TableSetupColumn("Score\t", ImGuiTableColumnFlags_WidthFixed,   -1.f );

        static ImVec4 color_;

        ImGui::TableHeadersRow();
        for (int row = 0; row < ptr_core->players(); row++)
        {
            Stats *stats = &warrior_stats[row];
            // select color
            if (*stats->prcs > 0)
            {
                color_ = PLR_COLORS.at(stats->player);
            }
            else
            {
                color_ = PLR_COLORS.at(Player::NONE);
                adjust_color(color_, 0.f, 0.45f);
            }

            ImGui::TableNextRow();
            {
                item_txt = "  " + std::to_string((int) stats->player );
                ImGui::TableSetColumnIndex(0); // # (player)
                    ImGui::TextColored(color_, item_txt.c_str());

                item_txt = " " + *stats->name;
                ImGui::TableSetColumnIndex(1); // Name
                    ImGui::TextColored(color_, item_txt.c_str());

                item_txt = " " + std::to_string( *stats->uuid );
                ImGui::TableSetColumnIndex(2); // ID
                    ImGui::TextColored(color_, item_txt.c_str());

                item_txt = " " + std::to_string( *stats->prcs );
                ImGui::TableSetColumnIndex(3); // Prcs
                    ImGui::TextColored(color_, item_txt.c_str());

                item_txt = " " + std::to_string( *stats->score );
                ImGui::TableSetColumnIndex(4); // Score
                    ImGui::TextColored(color_, item_txt.c_str());
            }
        }
    }
    ImGui::EndTable();
}

inline void ControlPanel::section_os_data()
{
 static std::string item_txt  = "";
 /** OS:DATA: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    ImGui::Text("Operating System Data");       ImGui::SameLine();
        ImGui::TextDisabled("(Help?)");
    if (ImGui::IsItemHovered())
    {
        ImGui::BeginTooltip();
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted(
            "A live feed of operating system data\n"
            "Cycles  : Total CPU cycles executed\n"
            "Prcs    : Total running processes\n"
            "Active  : Number of active program\n"
            "Memory  : Number of memory addresses"
        );
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
    ImGui::Separator();
 /** DATA:TABLE: ~~// Cycles | Prcs | Active | Memory Size //~~~~~~~~~~~~~~~~~~~*/
    static int const &cycles_     = ptr_core->cycles(),
                     &cycles_max_ = ptr_core->max_cycles(),
                     &active_     = ptr_core->active_programs(),
                     &memory_     = ptr_core->memory_size();

    if (ImGui::BeginTable("OS:DATA:TABLE", 6, ImGuiTableFlags_NoSavedSettings))
    {
        static float col_width = ImGui::GetContentRegionAvail().x * 0.22f;

        ImGui::TableSetupColumn(" Cycles",  ImGuiTableColumnFlags_WidthFixed,   -col_width );
        ImGui::TableSetupColumn("/Max",     ImGuiTableColumnFlags_WidthStretch, -col_width );
        ImGui::TableSetupColumn("Prcs",     ImGuiTableColumnFlags_WidthFixed,   -col_width );
        ImGui::TableSetupColumn("/Max",     ImGuiTableColumnFlags_WidthStretch, -col_width );
        ImGui::TableSetupColumn("Active\t", ImGuiTableColumnFlags_WidthFixed,   -col_width );
        ImGui::TableSetupColumn("Memory",   ImGuiTableColumnFlags_WidthStretch, -col_width );

        ImGui::TableHeadersRow();
        ImGui::TableNextRow();
        {
            item_txt = "  " + std::to_string( cycles_ );
            ImGui::TableSetColumnIndex(0); // CPU Cycles
                ImGui::TextColored(PLR_COLORS.at(Player::P5), item_txt.c_str());

            ImGui::TableSetColumnIndex(1); // MAX Cycles
                ImGui::TextColored(PLR_COLORS.at(Player::P5), std::to_string( cycles_max_ ).c_str());

            int prcs_     = ptr_core->total_processes();
            int prcs_max_ = ptr_core->max_processes();
            item_txt = " " + std::to_string( prcs_ );
            ImGui::TableSetColumnIndex(2); // Total Prcs
                ImGui::TextColored(PLR_COLORS.at(Player::P6), item_txt.c_str());


            int players_ = ptr_core->players();
            ImGui::TableSetColumnIndex(3); // MAX Prcs
                ImGui::TextColored(PLR_COLORS.at(Player::P6), std::to_string( prcs_max_ * players_ ).c_str());

            item_txt = " " + std::to_string( active_ );
            ImGui::TableSetColumnIndex(4); // Active Programs
                ImGui::TextColored(PLR_COLORS.at(Player::P3), item_txt.c_str());

            item_txt = " " + std::to_string( memory_ );
            ImGui::TableSetColumnIndex(5); // Memory Size
                ImGui::TextColored(PLR_COLORS.at(Player::P9), item_txt.c_str());
        }
    }
    ImGui::EndTable();
}

inline void ControlPanel::section_game_state()
{
 /** GAME:STATE: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    ImGui::Text("Game State");       ImGui::SameLine();
        ImGui::TextDisabled("(Help?)");
    if (ImGui::IsItemHovered())
    {
        ImGui::BeginTooltip();
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted(
            "Here is the current game's state\n"
            "State   : Live feed of the game's state\n"
            "Round   : Current round number\n"
            "Alive   : Number of warriors alive\n"
            "Winner  : Player who has the most points \n"
        );
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
    ImGui::Separator();
 /** STATE: ~~// State | Round | Alive | Winner//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    ImGui::BeginTable("##STATE:TABLE", 2);
    {
        static State const &state_ = ptr_core->state();
        static int   const &round_ = ptr_core->round(),
                           &alive_ = ptr_core->active_programs();

        static Player winner_;
        static std::string winner_string;
        if (state_ == State::COMPLETE)
        {
            winner_ = ptr_core->match_winner();
        }
        else winner_ = ptr_core->round_winner();

        winner_string = ptr_core->warrior_string( winner_ );

        static std::string item_txt;
        static ImVec4      item_color;
        switch (ptr_core->state())
        {
            default:
            {
                item_txt   = "WAITING";
                item_color = PLR_COLORS.at(Player::P1);
                break;
            }
            case State::RESET:
            {
                item_txt   = "RESET";
                item_color = PLR_COLORS.at(Player::P5);
                break;
            }
            case State::NEW_ROUND:
            case State::READY:
            {
                if (new_round_flag)
                {
                    item_txt = "NEW_ROUND";
                }
                else item_txt = "READY";

                item_color = PLR_COLORS.at(Player::P9);
                break;
            }
            case State::RUNNING:
            {
                item_txt   = "RUNNING";
                item_color = PLR_COLORS.at(Player::P3);
                break;
            }
            case State::COMPLETE:
            {
                item_txt   = "COMPLETE";
                item_color = PLR_COLORS.at(Player::P4);
                break;
            }
        }

        ImGui::TableSetupColumn("##Game:L",  ImGuiTableColumnFlags_WidthFixed,   -0.f );
        ImGui::TableSetupColumn("##Game:R",  ImGuiTableColumnFlags_WidthStretch, -0.f );

        // State
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
            ImGui::Text("State: ");
        ImGui::TableSetColumnIndex(1);
            ImGui::TextColored(item_color, item_txt.c_str());

        ImGui::Separator();
        ImGui::TableNextRow();        // State

        item_txt = std::to_string(round_) + " / " + std::to_string(ptr_core->max_rounds());
        ImGui::TableSetColumnIndex(0);
            ImGui::Text("Round: ");
        ImGui::TableSetColumnIndex(1);
            ImGui::TextColored(PLR_COLORS.at(Player::P1), item_txt.c_str());

        ImGui::Separator();
        ImGui::TableNextRow();        // Alive

        item_txt = std::to_string(alive_) + " / " + std::to_string(ptr_core->players());
        ImGui::TableSetColumnIndex(0);
            ImGui::Text("Alive: ");
        ImGui::TableSetColumnIndex(1);
            ImGui::TextColored(PLR_COLORS.at(Player::P3), item_txt.c_str());

        ImGui::Separator();
        ImGui::TableNextRow();        // Winner

        ImVec4 color_ = PLR_COLORS.at(winner_);
        if (winner_ == Player::NONE)
            adjust_color(color_, 0.f, 0.45f);

        ImGui::TableSetColumnIndex(0);
            ImGui::Text("Winner: ");
        ImGui::TableSetColumnIndex(1);
            ImGui::TextColored(color_, winner_string.c_str());
    }
    ImGui::EndTable();
}

inline void ControlPanel::section_game_controls()
{
    ImVec2 item_size   = ImGui::GetContentRegionAvail();
           item_size.y = 40.f;

    ImGui::Text("Game Controls");       ImGui::SameLine();
        ImGui::TextDisabled("(Help?)");
    if (ImGui::IsItemHovered())
    {
        ImGui::BeginTooltip();
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted(
            "Allows you to control the game\n"
            " * Warriors must be loaded\n"
            " * On 'COMPLETE', 'restart' or 'reload' is required\n"
            " * Play:  start/resume the game\n"
            " * Pause: freeze the game\n"
            "   * You can browse memory while paused\n"
        );
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
    ImGui::Separator();
 /** SIM:CONTROLS: ~~~// Play | Pause //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    ImGui::BeginGroup();
    {
        static char constexpr txt_play[]     = "Play",
                              txt_pause[]    = "Pause",
                              txt_complete[] = "Complete";
        ImVec4 color_;
        State state_ = ptr_core->state();

        int push_;
        /* Play */
        if (state_ != State::RUNNING)
        {
            if (new_round_flag)
            {
                color_ = PLR_COLORS.at(Player::P9);  adjust_color(color_, 0.0f);
            }
            else color_ = PLR_COLORS.at(Player::P3);  adjust_color(color_, -0.35f);

            push_ = push_btn_color(color_);

            if (state_ == State::COMPLETE)
            {
                ImGui::PopStyleColor(push_);
                color_ = PLR_COLORS.at(Player::P4);  adjust_color(color_, -0.15f);

                push_ = push_btn_color(color_);

                ImGui::Button(txt_complete, item_size);
                    if (ImGui::IsItemHovered())
                        ImGui::SetTooltip("Game: COMPLETE! you can 'restart game' or 'reload warriors");
            }
            else
            {
                if(ImGui::Button(txt_play, item_size) && m_init_flag)
                {
                    if (new_round_flag)
                    {
                        MemoryViewer::reset();
                        new_round_flag = false;
                    }
                    ptr_core->play_game();
                }
            }
        }
        else /* Pause */
        {
            color_ = PLR_COLORS.at(Player::P6);  adjust_color(color_, -0.25f);
            push_ = push_btn_color(color_);

            if(ImGui::Button(txt_pause, item_size) && m_init_flag)
                ptr_core->pause_game();
        }
        ImGui::PopStyleColor(push_);
    }
    ImGui::EndGroup();
}

void ControlPanel::draw()
{
    run_core_systems();

    /* Display */
    static ImGuiWindowFlags disp_flags ( GLOBAL_WINDOW_FLAGS );
    ImGuiStyle &edit_style    = ImGui::GetStyle(),
                saved_style   = edit_style;
    edit_style.ItemSpacing.y *= 1.5f;

    if (ImGui::Begin("Core Control Panel", NULL, GLOBAL_WINDOW_FLAGS & ~(ImGuiWindowFlags_NoResize)))
    {
        ImVec2 disp_window_size = ImGui::GetWindowSize();
        if ((disp_window_size.x + disp_window_size.y) < (m_min_window_size.x + m_min_window_size.y))
        {
            disp_window_size = m_min_window_size;
            ImGui::SetWindowSize(m_min_window_size);
        }
     /** MASTER:CONTROLS: ~~~// Restart Game | Reload Warriors //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
        section_master_controls();        ImGui::Separator();

     /** WARRIORS:SECTIONS: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
        static ImGuiWindowFlags disp_flags (
            GLOBAL_WINDOW_FLAGS
            & ~(ImGuiWindowFlags_AlwaysAutoResize)
            & ~(ImGuiWindowFlags_NoScrollbar)          // allow scroll
            & ~(ImGuiWindowFlags_NoScrollWithMouse)
        );
        ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);

        ImGui::BeginChild("MENU:SECTIONS", ImVec2(0.f, 0.f), true, disp_flags);    ImGui::PopStyleVar();
        {
        /** WARRIOR:LOADER:OR:DATA: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
            if (warriors_locked == false)
            {
                section_loader_menu();
            }
            else section_warriors_data();

            ImGui::Separator();
        /** DISPLAY:OS:DATA: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
            section_os_data();          ImGui::Separator();

        /** GAME:STATE: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
            section_game_state();       ImGui::Separator();

        /** SIM:CONTROLS: ~~~// Play | Pause //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
            section_game_controls();
        }
        ImGui::EndChild();
    }
    ImGui::End();
    edit_style = saved_style;
}

}}/* ::Core::GUI */
