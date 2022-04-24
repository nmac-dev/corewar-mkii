/// Memory array redcode simulator RAM  instruction objects and decoding functions
#pragma once

// #define MARS_DEBUG

#include <stdint.h>
#include <time.h>
#include "template/ram.hpp"
#include "asm.hpp"

namespace OS
{
namespace { enum class OPR { A, B }; } // Used to select operand A or B

/// Defines event types applied to the core
enum class Event{ NOOP, READ, WRITE, EXECUTE };

/// ControlUnit: stores the interpretation of the instruction register for the execute phase
struct ControlUnit
{
    /// Type: specifies types for the opcode operation and modifier argument
    struct Type {
        ASM::OPCODE_TYPE code;  // opcode   operation
        ASM::MOD_TYPE    mod;   // modifier argument
    };
    /// Operand: operand's core index address, pointer, value, and event
    struct Operand {
        int *ptr,       // pointer to instruction object's operand
             val;       // operand value
        Operand(int *_ptr);
        Operand();
    };
    /// Register: Instruction register for the control unit decoding
    struct Register {
        int index;          // absolute Core address
        ASM::Inst *inst;    // register instruction pointer

        Operand A, B;       // operands pointer and value
        Event event;        // event applied to the address

        Register(int _index, ASM::Inst *_inst);
        Register();
    };

    Type     TYPE;
    Register EXE,         // executing   register from the program counter
             SRC,         // source      register from MAIN's A operand
             DEST;        // destination register from MAIN's B operand     
    int *post_A, *post_B; // points to value to be post-incremented (or nullptr)
    
    /// Creates an Instruction ControlUnit (pre-configured for DAT opcode)
    /// @param _pc  instruction register program counter
    /// @param _RAM array containing instruction objects
    ControlUnit(int _pc, RAM_T<ASM::Inst> *_RAM);
    ControlUnit();
};

/// Memory Array Redcode Simulator (MARS): handles assembly instruction objects within RAM
class MARS
{
 private:
    inline static int core_size = 8000; // number of memory addresses within the core 
    int min_seperation;                 // min distance between warriors at the start of a round (config.ini) 

    RAM_T<ASM::Inst> RAM;               // Array of instruction objects (circular)

 public:
    /// Initialises the simulator by loading a default asm instruction (dat #0, #0) into every address,
    /// then places each warrior at a random location in accordance with the 'min_seperation' setting
    /// @param warrior_list collection of warriors to be loaded into the core
    /// @param _seperation (min_seperation) minimum seperation between warriors in the simulator
    MARS(ASM::WarriorList *warriors, int _seperation);
    MARS();

 /* Functions */
 private:
    /// Generates a random positive integer using the process clock and xorshift algorithm
    /// @param maxRange output will not exceed this value
    uint32_t randomInt(uint32_t maxRange);

    /// Returns a pointer to either operand A or B for the index given
    /// @param index instruction index within the MARS
    /// @param select selects operand A or B
    inline int *selectOperand(int index, OPR select)
    {
        return select == OPR::A ? &RAM[index]->A.val : &RAM[index]->B.val;
    }
    
    /// Swaps the two elements
    /// @param _X swaped to _Y
    /// @param _Y swaped to _X
    template <typename T>
    inline void swapElements(T &_X, T &_Y)
    {
        T swap_i = _X;      // swap index
        _X       = _Y;
        _Y       = swap_i;  // = _X
    }

    /// Decodes the addressing mode for the input instruction
    /// @param _CTRL buffer for the decoded results
    /// @param exe_select selects operand A or B
    ControlUnit::Register decodeAdmo(ControlUnit *_CTRL, OPR const exe_select);

    /// Decodes the modifier for the input instruction
    /// @param _CTRL buffer for the decoded results
    void decodeModifier(ControlUnit *_CTRL);
 
 public:
    /// Returns a control unit which has decoded the addressing modes and modifier
    /// @param _pc instruction register program counter to decode
    ControlUnit generateCTRL(int const _pc);

    /// Returns the size of the simulator
    inline int size() const { return core_size; }

    /// access index of core simulators's memory array
    ASM::Inst &operator[](int index) const;
    /// modify index of core simulators's memory array
    ASM::Inst &operator[](int index);
}; // MARS

} // namespace OS
