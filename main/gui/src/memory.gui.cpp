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
    for (int i = 0; i < m_cells.size(); i++)
        m_cells[i] = Cell();
}

void Memory::update_cells()
{
    Player player_ = ptr_corewar->warrior().player();

    int _adr;
    Cell *cell_;
    // EXE
    if (ptr_corewar->report().exe.event != OS::Event::NOOP)
    {
        _adr  = ptr_corewar->report().exe.address;
        cell_ = &m_cells[_adr];

        cell_->event = ptr_corewar->report().exe.event;

        if (cell_->event == OS::Event::ILLEGAL)
        {
            cell_->owner = Player::NONE;
        }
        else cell_->owner = player_;
        cell_->assembly = ptr_corewar->assembly_at(_adr);
    }
    // SRC
    if (ptr_corewar->report().src.event != OS::Event::NOOP)
    {
        _adr  = ptr_corewar->report().src.address;
        cell_ = &m_cells[_adr];

        cell_->editor = player_;
        cell_->event  = ptr_corewar->report().src.event;

        if (cell_->event == OS::Event::WRITE)
            cell_->assembly = ptr_corewar->assembly_at(_adr);
    }
    // DEST
    if (ptr_corewar->report().dest.event != OS::Event::NOOP)
    {
        _adr  = ptr_corewar->report().dest.address;
        cell_ = &m_cells[_adr];

        cell_->editor = player_;
        cell_->event  = ptr_corewar->report().dest.event;

        if (cell_->event == OS::Event::WRITE)
            cell_->assembly = ptr_corewar->assembly_at(_adr);
    }
}

void Memory::draw()
    {
     /* Display */
        static ImVec2 const disp_cell_size   = ImVec2(7.f, 7.f);  // size of a single cell
        float               disp_cell_distro = 0;                 // distrobution of cells across the displays width

        static ImGuiWindowFlags disp_flags ( 
            global_abs_flags
            // extra
        );

        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImVec2 window_pos  = viewport->Pos;
        ImVec2 window_size = viewport->Size;

        static ImVec2 const disp_max (1024.f, 540.f);
        static float constexpr scalar   = 0.65f;
        bool disp_under_max =  (window_size.x * (scalar + 0.1f)) < disp_max.x;
        // X Cap
        if (disp_under_max)
        {
            window_size.x *= scalar;
        }
        else window_size.x = disp_max.x;
        

        /* Save style, then edit */
        ImGuiStyle &edit_style  = ImGui::GetStyle(),
        saved_style             = edit_style;

        edit_style.WindowPadding  = ImVec2(5.f, 5.f);
        edit_style.ItemSpacing    = ImVec2(1.f, 1.f);

     /* Cell Distro */
        float cell_space = disp_cell_size.x + edit_style.ItemSpacing.x;
        disp_cell_distro = (window_size.x / cell_space); // distro cells across display width
        
        // Y Cap
        if (disp_under_max)
        {
            window_size.y = ((float) m_cells.size() / disp_cell_distro) * cell_space;
            window_size.y += cell_space;
        }
        else window_size.y = disp_max.y;

        window_size.x += cell_space;

        ImGui::SetNextWindowPos(window_pos);
        ImGui::SetNextWindowSize(window_size);
        ImGui::SetNextWindowViewport(viewport->ID);

        if (ImGui::Begin("Live Memory Execution", NULL, disp_flags))
        {
            Cell   *cell_;
            ImVec4 color_;

         /* Cells */
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

                 /* Select Player */
                    cell_ = &m_cells[counter_];
                    switch (cell_->event)
                    {
                        default:
                        case OS::Event::EXECUTE:
                        {
                            color_ = plr_colors.at(cell_->owner);
                            // mark executing cell
                            if (counter_ == ptr_corewar->report().exe.address)
                                adjust_color(color_);
                            break;
                        }
                        case OS::Event::READ:
                        case OS::Event::WRITE:
                        {
                            color_ = plr_colors.at(cell_->editor);
                            adjust_color(color_, 0.f, 0.8f);
                        }
                    }
                 /* Create Cell Style */
                    ImGui::PushStyleColor(ImGuiCol_Button,  color_);        push_++;
                    adjust_color(color_);
                    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, color_);  push_++;
                    adjust_color(color_);
                    ImGui::PushStyleColor(ImGuiCol_ButtonActive, color_);   push_++;

                    ImGui::Button(" ", disp_cell_size);
                    if (ImGui::IsItemHovered())
                    {
                        ImGui::SetTooltip(cell_->assembly.c_str());
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
