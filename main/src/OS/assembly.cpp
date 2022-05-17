/// Defines elements and entities in relation to assembly instructions

#include "OS/assembly.hpp"

namespace Asm
{
Inst::Inst(Operation _OP, Operand _A, Operand _B)
{
    OP = _OP;
    A  = _A;
    B  = _B;
}
Inst::Inst()
{
    (*this) = { {Opcode::DAT, Modifier::F}, {Admo::IMMEDIATE, 0}, {Admo::IMMEDIATE, 0} };
}

Modifier Inst::find_default_mod(Opcode _code, Admo _A, Admo _B)
{
    switch (_code)
    {
    // [dat]
    case Opcode::DAT: 
        return Modifier::F;
    // [mov, seq, sne]
    case Opcode::MOV:
    case Opcode::SEQ:
    case Opcode::SNE:
    // [add, sub, mul, div, mod]
    case Opcode::ADD:
    case Opcode::SUB:
    case Opcode::MUL:
    case Opcode::DIV:
    case Opcode::MOD:
        // A == '#'
        if (_A == Admo::IMMEDIATE)
        {
            return Modifier::AB;
        }
        // A != ['#','*'] && B == '#' 
        else if (_A != Admo::IMMEDIATE && _B == Admo::IMMEDIATE)
        {
            return Modifier::B;
        }
        else
        {   // default: [mov, seq, sne]
            if (_code <= Opcode::SNE)
            {
                return Modifier::I;
            }
            // default: [add, sub, mul, div, mod]
            return Modifier::F;
        }
    // [slt]
    case Opcode::SLT:
        // A == '#'
        if (_A == Admo::IMMEDIATE)
        {
            return Modifier::AB;
        }
    // [nop, jmp, jmz, jmn, djn, spl]
    case Opcode::NOP:
    case Opcode::JMP:
    case Opcode::JMZ:
    case Opcode::JMN:
    case Opcode::DJN:
    case Opcode::SPL:
        default: return Modifier::B;
    }
} /* ::getDefaultModifier() */

std::string Inst::to_assembly()
{
    int constexpr avg_inst_size = 16;           // average asm code inst size
    std::string code_ = "";                     // full asm code string
                code_.reserve(avg_inst_size);

    std::string asm_arg   = "";                 // single asm argument
    /* opcode */
    switch (OP.code)
    {
        case Opcode::NOP: asm_arg = "nop"; break;
        case Opcode::DAT: asm_arg = "dat"; break;
        case Opcode::MOV: asm_arg = "mov"; break;
        case Opcode::SEQ: asm_arg = "seq"; break;
        case Opcode::SNE: asm_arg = "sne"; break;
        case Opcode::SLT: asm_arg = "slt"; break;
        case Opcode::ADD: asm_arg = "add"; break;
        case Opcode::SUB: asm_arg = "sub"; break;
        case Opcode::MUL: asm_arg = "mul"; break;
        case Opcode::DIV: asm_arg = "div"; break;
        case Opcode::MOD: asm_arg = "mod"; break;
        case Opcode::JMP: asm_arg = "jmp"; break;
        case Opcode::JMZ: asm_arg = "jmz"; break;
        case Opcode::JMN: asm_arg = "jmn"; break;
        case Opcode::DJN: asm_arg = "djn"; break;
        case Opcode::SPL: asm_arg = "spl"; break;
    } code_.append(asm_arg);

    /* mod */;
    switch (OP.mod)
    {
        case Modifier::A:  asm_arg = ".a";  break;
        case Modifier::B:  asm_arg = ".b";  break;
        case Modifier::AB: asm_arg = ".ab"; break;
        case Modifier::BA: asm_arg = ".ba"; break;
        case Modifier::F:  asm_arg = ".f";  break;
        case Modifier::X:  asm_arg = ".x";  break;
        case Modifier::I:  asm_arg = ".i";  break;
    } code_.append(asm_arg);

    /* operands */
    Operand *opr = &A;
    int i = 0;
    while (i++ < 2)
    {
        /* admo */
        switch (opr->admo)
        {
            case Admo::IMMEDIATE:  asm_arg = " #"; break;
            case Admo::DIRECT:     asm_arg = " $"; break;
            case Admo::INDIRECT_A: asm_arg = " *"; break;
            case Admo::INDIRECT_B: asm_arg = " @"; break;
            case Admo::PRE_DEC_A:  asm_arg = " {"; break;
            case Admo::PRE_DEC_B:  asm_arg = " <"; break;
            case Admo::POST_INC_A: asm_arg = " }"; break;
            case Admo::POST_INC_B: asm_arg = " >"; break;
        } code_.append(asm_arg);

        /* val */
        code_.append(std::to_string(opr->val));
        
        if (i == 1) code_.append(",");
        opr = &B; // swap A -> B
    }
    return code_;
} /* ::toAsmCode() */

Program::Program(std::string _name, const int _length)
{
    m_uuid         = create_uuid();
    m_name         = _name;
    m_length       = _length;
    m_address      = -1;                    // m_address (late init)
    m_insts.reserve(m_length);
}

void Program::push(Inst _inst)
{
    if (m_insts.size() != m_length) 
        m_insts.push_back(_inst);
}

Inst  Program::operator[](int address) const { return m_insts[address]; }
Inst &Program::operator[](int address)       { return m_insts[address]; }

} /* ::Asm */
