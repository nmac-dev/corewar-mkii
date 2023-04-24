/// GUI display for the operating systems memory
#pragma once

#include "imgui_required.hpp"

namespace Core { namespace GUI
{
/// A static class to display the memory of the core operating system
class MemoryViewer
{
 public:
    /// Stores information for a single memory address within the operating system
    struct Cell
    {
        Player      owner    = Player::NONE;              // owner of the cell
        Player      editor   = Player::NONE;              // player who last edited the cell ( Event::READ|WRITE )
        OS::Event   event    = OS::Event::NOOP;           // os event commited to the cell
        std::string assembly = Asm::Inst().to_assembly(); // assembly instruction at the address
    };
 private:
    /* Display */
    static inline float  constexpr PAD = 9.f;
    static inline bool m_init_flag     = false, // true when the display is initialised
                       m_reset_flag    = false; // true when the display needs to be reset
    static inline ImVec2 constexpr disp_window_size = ImVec2(1024.f + PAD, 540.f);
    static inline ImVec2 constexpr disp_cell_size   = ImVec2(7.f, 7.f);  // size of a single cell

    /* Data */
    static inline std::vector<Cell> memory_cells;   // collection of cells in the viewer
    static inline Core::Game       *ptr_core;       // pointer to the running core game

    MemoryViewer() = default;                      /// Constructor blocked
 public:
    MemoryViewer(MemoryViewer const &)   = delete; /// Copy creation deleted
    void operator=(MemoryViewer const &) = delete; /// Assignment operator deleted

 /* Functions */

    /// Inititalises the memory viewer, must be called before use
    /// @param _core pointer to a running core game
    static void init(Core::Game *_core);

    /// Returns true if the memory display has been initialised
    static inline bool &init_flag() { return m_init_flag; }

    /// Resets all cell data to default
    static void reset();

    /// Updates the cell using the OS report and program
    static void update_cells();

    /// Draw the memory viewer
    static void draw();

    /// Updates the assembly string for the cell at the address given
    static inline void update_cell_assembly(int _adr)
    {
        memory_cells[_adr].assembly = ptr_core->assembly_at(_adr);
    }
};

}}/* ::Core::GUI */
