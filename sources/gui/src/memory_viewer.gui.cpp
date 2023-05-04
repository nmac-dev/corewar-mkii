/// GUI display for the operating systems memory
#include "memory_viewer.gui.hpp"

namespace Core { namespace GUI
{
void MemoryViewer::init(Game *_core)
{
    ptr_core = _core;

    memory_cells.clear();
    memory_cells.resize(Game::memory_size());

    reset();
    m_init_flag  = true;
}

void MemoryViewer::reset()
{
    // set all to default
    for (int i = 0; i < memory_cells.size(); i++)
        memory_cells[i] = MemoryViewer::Cell();

    if (ptr_core->state() == State::WAITING )
        return;

    // set initial program
    for (int plr = 1; plr <= ptr_core->players(); plr++)
    {
        int adr_ = ptr_core->program_address( (Player) plr );
        int len_ = ptr_core->program_length(  (Player) plr );

        // add assembly to cells
        for (int i = 0; i < len_; i++)
        {
            if (adr_ >= memory_cells.size())
                adr_ = 0;

            memory_cells[adr_].owner    = (Player) plr;
            memory_cells[adr_].editor   = Player::NONE;
            memory_cells[adr_].event    = OS::Event::EXECUTE;
            update_cell_assembly(adr_);
            adr_++;
        }
    }
    m_reset_flag = true;
}

void MemoryViewer::update_cells()
{
    // only update if the game is running, or trigger a reset
    if ( !init_flag() )
        return;

    State state_ = ptr_core->state();

    // trigger a single reset and dont update until the game state changes
    if (state_ == State::RESET)
    {
        if ( !m_reset_flag)
            reset();
    }

    if (state_ == State::RUNNING)
    {
        m_reset_flag = false;
    }
    else return;

    /* Update Cell */
    static const OS::Report &report_ = ptr_core->report();
    Player player_                   = ptr_core->warrior().player();

    int _adr;
    Cell *cell_;
    /** EXE: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
        _adr  = report_.exe.address;
        cell_ = &memory_cells[_adr];

        cell_->owner = player_;
        cell_->event = report_.exe.event;
        update_cell_assembly(_adr);
    /** SRC: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
        _adr  = report_.src.address;
        cell_ = &memory_cells[_adr];

        cell_->editor = player_;
        cell_->event  = report_.src.event;

        if (_adr != report_.exe.address)
            update_cell_assembly(_adr);

    /** DEST: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
        _adr  = report_.dest.address;
        cell_ = &memory_cells[_adr];

        cell_->editor = player_;
        cell_->event  = report_.dest.event;

        if (_adr != report_.exe.address)
            update_cell_assembly(_adr);
}

void MemoryViewer::draw()
{
    update_cells();
    /* Display */
    static ImGuiWindowFlags disp_flags ( GLOBAL_WINDOW_FLAGS );

    /* Save style, then edit */
    ImGuiStyle &edit_style  = ImGui::GetStyle(),
    saved_style             = edit_style;

    edit_style.WindowPadding  = ImVec2(5.f, 5.f);
    edit_style.ItemSpacing    = ImVec2(1.f, 1.f);

    /* Cell Distro */
    // distro cells across display width
    float disp_cell_distro = ((disp_window_size.x - PAD)
                           / (disp_cell_size.x + edit_style.ItemSpacing.x));

    ImGui::SetNextWindowSize(disp_window_size);

    if (ImGui::Begin("Live Memory", NULL, GLOBAL_WINDOW_FLAGS))
    {
        Cell   *cell_;
        ImVec4 color_;

        /* Create Cells */
        int counter_   = 0;
        while ( counter_ < memory_cells.size() )
        {
            ImGui::BeginGroup();
            for (int col = 0; col < (int) disp_cell_distro; col++)
            {
                if ( counter_ >= memory_cells.size() )
                    break;

                /* Colors Cells */
                cell_ = &memory_cells[counter_];

                if (cell_->event == OS::Event::READ || cell_->event == OS::Event::WRITE)
                {
                    color_ = PLR_COLORS.at(cell_->editor);
                    adjust_color(color_, 0.f, 0.55f);
                }
                else
                {
                    color_ = PLR_COLORS.at(cell_->owner);
                    // mark executing cell
                    if (init_flag()
                        && ptr_core->state() == State::RUNNING
                        && counter_ == ptr_core->report().exe.address)
                        adjust_color(color_);
                }

                /* Create Cell */
                int push_ = 0;
                ImGui::PushStyleColor(ImGuiCol_Button,  color_);        push_++;
                adjust_color(color_);
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, color_);  push_++;
                ImGui::PushStyleColor(ImGuiCol_ButtonActive,  color_);  push_++;

                ImGui::Button(" ", disp_cell_size);
                    if (ImGui::IsItemHovered())
                    {
                        ImGui::SetTooltip("%s\n|%d|", cell_->assembly.c_str(), counter_ +1);
                    }

                ImGui::PopStyleColor(push_);
                // next cell
                ImGui::SameLine();
                counter_++;
            }
            ImGui::EndGroup();
        }
    }
    ImGui::End();

    ImGui::GetStyle() = saved_style;
}

}}/* ::Core::GUI */
