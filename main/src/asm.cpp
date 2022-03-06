/// Defines elements and entities in relation to assembly instructions

#include "asm.hpp"

namespace ASM
{
Instr::Instr()  
{
    opcode   = nullptr;
    modifier = nullptr;
    mode_a   = nullptr;
    mode_b   = nullptr;
    op_a     = 0;
    op_b     = 0;
}
Instr::~Instr() {}

Warrior::Warrior()  {}
Warrior::~Warrior() {}
} // namespace ASM
