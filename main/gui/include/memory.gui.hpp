/// GUI display for the operating systems memory
#pragma once

#include "imgui_required.hpp"

namespace Corewar { namespace GUI
{
/// Stores information for a single memory address within the operating system
struct Cell
{
    Player      owner    = Player::NONE;              // owner of the cell
    Player      editor   = Player::NONE;              // player who last edited the cell ( Event::READ|WRITE )
    OS::Event   event    = OS::Event::NOOP;           // os event commited to the cell
    std::string assembly = Asm::Inst().to_assembly(); // assembly instruction at the address
};

/// A static class to display the memory of the corewar operating system
class Memory
{
 private:
    /* Display */
    static inline float  constexpr PAD = 8.f;
    static inline ImVec2 constexpr disp_window_size = ImVec2(1024.f + PAD, 540.f);
    static inline ImVec2 constexpr disp_window_pos  = ImVec2(0.f, 0.f);
    static inline ImVec2 constexpr disp_cell_size   = ImVec2(7.f, 7.f);  // size of a single cell

    /* Data */
    static inline std::vector<Cell> m_cells;    // collections of cells in the display
    static inline Corewar::Game    *ptr_corewar;  // pointer to the running corewar game

    Memory() = default;                         /// Constructor blocked

 public:
    Memory(Memory const &)         = delete;    /// Copy creation deleted
    void operator=(Memory const &) = delete;    /// Assignment operator deleted

 /* Functions */

    /// Inititalises the memory display, must be called before use but after corewar game is running
    /// @param _corewar pointer to a running corewar game
    static void init(Game *_corewar);

    /// Resets all cell data to default
    static void reset_display();

    /// Updates the cell using the OS report and program 
    static void update_cells();

    /// Draw the memory display on the GUI
    static void draw();

    /// Updates the assembly string for the cell at the address given
    static inline void update_cell_assembly(int _adr)
    {
        m_cells[_adr].assembly = ptr_corewar->assembly_at(_adr);
    }
};

}}/* ::Corewar::GUI */
