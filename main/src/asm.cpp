/// Defines elements and entities in relation to assembly instructions

#include "asm.hpp"

namespace ASM
{
Inst::Inst()  
{
    opcode   = nullptr;
    modifier = nullptr;
    mode_a   = nullptr;
    mode_b   = nullptr;
    op_a     = 0;
    op_b     = 0;
}
Inst::~Inst() {}

Warrior::Warrior()  {}
Warrior::~Warrior() {}
} // namespace ASM
