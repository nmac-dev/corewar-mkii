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

Warrior::Warrior(std::string warrior_name, Inst *inst_array, const int n_inst)
{
    name         = warrior_name;
    uuid         = createUUID();
    length       = n_inst;
    instructions = inst_array;
    // core_index is set in the core
}
Warrior::~Warrior() { delete[] instructions; }

Inst Warrior::operator[](int index) const
{
    return instructions[index];
}

Inst &Warrior::operator[](int index)
{
    return instructions[index];
}

} // namespace ASM
