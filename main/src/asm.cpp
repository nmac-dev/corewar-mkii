/// Defines elements and entities in relation to assembly instructions

#include "asm.hpp"

namespace ASM
{

//// Inst

Inst::Inst()
{
    opcode    = _OP::DAT;
    modifier  = _MOD::F;
    // A
    admo_a    = _AM::DIRECT;
    operand_a = 0;
    // B
    admo_b    = _AM::DIRECT;;
    operand_b = 0;
}
Inst::Inst(_OP op, _MOD mod, _AM am_a, int o_a, _AM am_b, int o_b)  
{
    opcode    = op;
    modifier  = mod;
    // A
    admo_a    = am_a;
    operand_a = o_a;
    // B
    admo_b    = am_b;
    operand_b = o_b;
}

//// Warrior

Warrior::Warrior(std::string _name, const int _length)
{
    uuid         = createUUID();
    name         = _name;
    length       = _length;
    core_index   = -1;              // core_index is late init
    instructions.reserve(length);
}

Inst  Warrior::operator[](int index) const { return instructions[index]; }
Inst &Warrior::operator[](int index)       { return instructions[index]; }

} // namespace ASM
