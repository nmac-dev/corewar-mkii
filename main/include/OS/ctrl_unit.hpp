/// Control Unit handles registers and decoding using the program counter
#pragma once

#include "assembly.hpp"

namespace OS
{
namespace /* {anonymous} */ { using namespace Asm; }

enum class Event{ NOOP, READ, WRITE, EXECUTE }; // Defines event types applied to the core

/// Register: Instruction register for the control unit decoding
struct Register 
{
    int address;                // absolute memory address
    Inst::Operation *OP;        // operation pointer
    Inst::Operand *A, *B;       // operand pointer
    Event event;                // event applied to the address

    Register(int _index, Inst *_inst);
    Register();

    /// Swaps the operands of the register
    inline void swap_operands()
    {
        Inst::Operand *swap_ = A;
        A = B;                      // A -> B
        B = swap_;                  // B -> A
    }
};

/// ControlUnit: stores the interpretation of the instruction register for the execute phase
struct ControlUnit
{
    ArgType     TYPE;              // [code] + <mod> type
    Register EXE,               // executing program counter
             SRC,               // EXE's A field
             DEST;              // EXE's B field
    int *post_A, *post_B;       // points to value to be post-incremented (or nullptr)
    
    /// Creates an Instruction ControlUnit (pre-configured for DAT opcode)
    /// @param _EXE  executing   register
    /// @param _SRC  source      register (EXE.A)
    /// @param _DEST destination register (EXE.B)
    ControlUnit(Register _EXE = {}, Register _SRC = {}, Register _DEST = {});
};

} /* ::OS */
