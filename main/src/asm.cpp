/// Defines elements and entities in relation to assembly instructions

#include "asm.hpp"

namespace ASM
{

/* Inst */

Inst::Operation::Operation()
{
    code = OPCODE::DAT;
    mod  = MOD::F;
}

Inst::Operand::Operand()
{
    admo  = ADMO::DIRECT;
    val   = 0;
}

Inst::Inst(Operation _OP, Operand _A, Operand _B)
{
    OP = _OP;
    A  = _A;
    B  = _B;
}
Inst::Inst() = default;
Inst::~Inst(){}

std::string Inst::toAsmCode()
{
    std::string asm_arg   = "";         // single asm argument
    std::string asm_code_ = "";         // full asm code string
    asm_code_.reserve(32);

    /* opcode */
    switch (OP.code)
    {
        case OPCODE::DAT: asm_arg = "dat"; break;
        case OPCODE::MOV: asm_arg = "mov"; break;
        case OPCODE::SEQ: asm_arg = "seq"; break;
        case OPCODE::SNE: asm_arg = "sne"; break;
        case OPCODE::SLT: asm_arg = "slt"; break;
        case OPCODE::ADD: asm_arg = "add"; break;
        case OPCODE::SUB: asm_arg = "sub"; break;
        case OPCODE::MUL: asm_arg = "mul"; break;
        case OPCODE::DIV: asm_arg = "div"; break;
        case OPCODE::MOD: asm_arg = "mod"; break;
        case OPCODE::JMP: asm_arg = "jmp"; break;
        case OPCODE::JMZ: asm_arg = "jmz"; break;
        case OPCODE::JMN: asm_arg = "jmn"; break;
        case OPCODE::DJN: asm_arg = "djn"; break;
        case OPCODE::SPL: asm_arg = "spl"; break;
    } asm_code_.append(asm_arg);

    /* mod */
    asm_code_.append(".");
    switch (OP.mod)
    {
        case MOD::A:  asm_arg = "a";  break;
        case MOD::B:  asm_arg = "b";  break;
        case MOD::AB: asm_arg = "ab"; break;
        case MOD::BA: asm_arg = "ba"; break;
        case MOD::F:  asm_arg = "f";  break;
        case MOD::X:  asm_arg = "x";  break;
        case MOD::I:  asm_arg = "i";  break;
    } asm_code_.append(asm_arg);

    /* operands */
    Operand *opr = &A;
    int i = 0;
    while (i++ < 2)
    {
        /* admo */
        switch (opr->admo)
        {
            case ADMO::IMMEDIATE:  asm_arg = " #"; break;
            case ADMO::DIRECT:     asm_arg = " $"; break;
            case ADMO::INDIRECT_A: asm_arg = " *"; break;
            case ADMO::INDIRECT_B: asm_arg = " @"; break;
            case ADMO::PRE_DEC_A:  asm_arg = " {"; break;
            case ADMO::PRE_DEC_B:  asm_arg = " <"; break;
            case ADMO::POST_INC_A: asm_arg = " }"; break;
            case ADMO::POST_INC_B: asm_arg = " >"; break;
        } asm_code_.append(asm_arg);

        /* val */
        asm_code_.append(std::to_string(opr->val));
        
        if (i == 0) asm_code_.append(",");
        opr = &B; // swap A -> B
    }
    return asm_code_;
}

/* Warrior */
Warrior::Warrior(std::string _name, const int _length, int _max_warrior_len)
{
    if (!max_warrior_len) 
        max_warrior_len = _max_warrior_len; // set once

    uuid         = createUUID();
    name         = _name;
    length       = _length;
    core_index   = -1;              // core_index is late init
    instructions.reserve(max_warrior_len);
}
Warrior::~Warrior(){}

void Warrior::push(Inst _inst)
{
    if (instructions.size() != length) 
        instructions.push_back(_inst);
}

Inst  Warrior::operator[](int index) const { return instructions[index]; }
Inst &Warrior::operator[](int index)       { return instructions[index]; }

} // namespace ASM
