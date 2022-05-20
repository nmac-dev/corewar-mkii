/// GUI display for the operating systems memory

#include "memory.gui.hpp"

namespace Corewar { namespace GUI
{

void Memory::init(Game *_corewar)
{
    ptr_corewar = _corewar;

    m_cells.clear();
    m_cells.reserve(ptr_corewar->core_size());
    m_cells.resize( ptr_corewar->core_size());

    reset_display();
}

void Memory::reset_display()
{
    // set all to default
    for (int i = 0; i < m_cells.size(); i++)
        m_cells[i] = Cell();

    // set initial program
    for (int plr = 1; plr <= ptr_corewar->players(); plr++)
    {
        int adr_ = ptr_corewar->program_address( (Player) plr );
        int len_ =  ptr_corewar->program_length( (Player) plr );

        // add assembly to cells
        for (int i = 0; i < len_; i++)
        {
            if (adr_ >= m_cells.size())
                adr_ = 0;

            m_cells[adr_].owner    = (Player) plr;
            m_cells[adr_].editor   = Player::NONE;
            m_cells[adr_].event    = OS::Event::EXECUTE;
            update_cell_assembly(adr_);
            adr_++;
        }
    }
}

void Memory::update_cells()
{
    Player player_ = ptr_corewar->warrior().player();

    int _adr;
    Cell *cell_;
    /** EXE: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
        _adr  = ptr_corewar->report().exe.address;
        cell_ = &m_cells[_adr];

        cell_->owner = player_;
        cell_->event = ptr_corewar->report().exe.event;
        update_cell_assembly(_adr);
    /** SRC: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
        _adr  = ptr_corewar->report().src.address;
        cell_ = &m_cells[_adr];

        cell_->editor = player_;
        cell_->event  = ptr_corewar->report().src.event;

        if (_adr != ptr_corewar->report().exe.address)
            update_cell_assembly(_adr);

    /** DEST: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
        _adr  = ptr_corewar->report().dest.address;
        cell_ = &m_cells[_adr];

        cell_->editor = player_;
        cell_->event  = ptr_corewar->report().dest.event;

        if (_adr != ptr_corewar->report().exe.address)
            update_cell_assembly(_adr);
}

void Memory::draw()
    {
     /* Display */
        static ImGuiWindowFlags disp_flags ( global_window_flags );

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

        if (ImGui::Begin("Live Memory", NULL, global_window_flags))
        {
            Cell   *cell_;
            ImVec4 color_;

         /* Create Cells */
            int push_;
            int counter_   = 0;
            while ( counter_ < m_cells.size() )
            {
                ImGui::BeginGroup();
                for (int col = 0; col < (int) disp_cell_distro; col++)
                {
                    push_ = 0;
                    if ( counter_ >= m_cells.size() )
                        break;

                 /* Colors Cells */
                    cell_ = &m_cells[counter_];

                    if (cell_->event == OS::Event::READ || cell_->event == OS::Event::WRITE)
                    {
                        color_ = plr_colors.at(cell_->editor);
                        adjust_color(color_, 0.f, 0.7f);
                    }
                    else
                    {
                        color_ = plr_colors.at(cell_->owner);
                        // mark executing cell
                        if (counter_ == ptr_corewar->report().exe.address)
                            adjust_color(color_);
                    }

                 /* Create Cell Style */
                    ImGui::PushStyleColor(ImGuiCol_Button,  color_);        push_++;
                    adjust_color(color_);
                    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, color_);  push_++;
                    ImGui::PushStyleColor(ImGuiCol_ButtonActive, color_);   push_++;

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

}}/* ::Corewar::GUI */
