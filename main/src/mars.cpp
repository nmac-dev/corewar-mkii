// memory array assembly simulator containts array of instruction objects and decoding functions

#include "mars.hpp"

namespace OS
{
/* ControlUnit */
ControlUnit::Operand::Operand(int *_ptr)
{
    if (_ptr != 0 || _ptr != nullptr)
    {
        ptr = _ptr;
        val = *ptr;
    }
    else val = 0;
}
ControlUnit::Operand::Operand() = default;

ControlUnit::Register::Register(int _index, ASM::Inst *_inst)
{
    index = _index;
    inst  = _inst;
    A     = Operand(&inst->A.val); 
    B     = Operand(&inst->B.val);
    event = Event::NOOP; // event applied to the address
}
ControlUnit::Register::Register() = default;

ControlUnit::ControlUnit(int _pc, RAM_T<ASM::Inst> *RAM)
{
    EXE      = Register(_pc, (*RAM)[_pc]);
    SRC      = Register();  // EXE A
    DEST     = Register();  // EXE B

    post_A   = nullptr;
    post_B   = nullptr;

    TYPE.code = ASM::OPCODE_TYPE::READWRITE;
    TYPE.mod  = ASM::MOD_TYPE::SINGLE;
}
ControlUnit::ControlUnit() = default;

/* MARS */
MARS::MARS(ASM::WarriorList *warriors, int _seperation)
{
    min_seperation = _seperation;

    // populate RAM with (dat #0, #0) asm instructions
    RAM = RAM_T<ASM::Inst>(core_size);

    // place warriors in core at random positions
    for (int i = 0; i < warriors->size(); i++)
    {
        ASM::Warrior warrior_i = *(*warriors)[i].get();
        uint32_t rnd_pos        = randomInt(core_size);

        // validate position meets minimum seperation requirements
        for (int k = 0; k < i; k++)
        {
            // get upper & lower limits for seperation boundry
            int upper   = RAM.loopIndex(rnd_pos + min_seperation),
                lower   = RAM.loopIndex(rnd_pos - min_seperation);
            int w_index = warrior_i.getCoreIndex();

            if (w_index > lower && w_index < upper)
            {
                // failed; generate new position, restart loop
                rnd_pos = randomInt(core_size);
                k = 0;
            }
        }
        (*warriors)[i].get()->setCoreIndex(rnd_pos);

        // add each warrior instruction into the core
        for (int j = 0; j < warrior_i.len(); j++)
        {
            *RAM[rnd_pos++] = warrior_i[j];
        }

        #ifdef MARS_DEBUG
        if (i == 0) printf("\nMARS::MARS: \n");
        printf("\tAdded |%d| instructions @ index [%d] \t...from: [%d] '%s'\n",
                warrior_i.len(), (*warriors)[i].get()->getCoreIndex(),
                warrior_i.getUUID(), warrior_i.getName().c_str());
        #endif
    }
}
MARS::MARS() = default;

uint32_t MARS::randomInt(uint32_t maxRange)
{
    static uint32_t repeated_flag; // stores first seed, flag triggers on seed repetition
    static uint32_t seed;          // seed is random and mutates on each function call

    // generate a new seed on the first function call, or when the seed has repeated it's sequance
    if (seed == repeated_flag)
    {
        // large base value ensures a long random sequance before repetition
        seed  = 0xF2A40816 | repeated_flag;

        // clock() is unreliable, which provides a base for randomness
        seed |= (uint32_t) clock() << seed;
        repeated_flag = seed;
    }
    // xorshift algorithm
    seed ^= seed << 13;
    seed ^= seed >> 17;
    seed ^= seed << 5;

    return seed % maxRange;
}

ControlUnit::Register MARS::decodeAdmo(ControlUnit *_CTRL, OPR const exe_select)
{
    ASM::ADMO _admo = (exe_select == OPR::A) ? _CTRL->EXE.inst->A.admo  // SRC
                                             : _CTRL->EXE.inst->B.admo; // DEST
    OPR indirect;
    int main_i  = _CTRL->EXE.index; // executing instruction index
    
    // process addressing mode
    int pre_dec = 0;                // pre-decrement flag: set to -1 when applied
    switch (_admo)
    {
    case ASM::ADMO::IMMEDIATE:
        // relative address is 0, use stored value
        break;

    case ASM::ADMO::DIRECT:
        main_i += (*selectOperand(main_i, exe_select));
        break;
        
    /* Indirect */
    case ASM::ADMO::PRE_DEC_A:  pre_dec = -1;
    case ASM::ADMO::INDIRECT_A:
    case ASM::ADMO::POST_INC_A: indirect = OPR::A; goto CALC_INDIRECT;

    case ASM::ADMO::PRE_DEC_B:  pre_dec = -1;
    case ASM::ADMO::INDIRECT_B:
    case ASM::ADMO::POST_INC_B: indirect = OPR::B; goto CALC_INDIRECT;

    CALC_INDIRECT:
        main_i += (*selectOperand(main_i, indirect));       // indirect A|B
        (*selectOperand(main_i, indirect)) += pre_dec;      // apply pre-dec flag

        switch (_admo)
        {   /* post-inc ptr */
            case ASM::ADMO::POST_INC_A: _CTRL->post_A = selectOperand(main_i, indirect); break;
            case ASM::ADMO::POST_INC_B: _CTRL->post_B = selectOperand(main_i, indirect); break;
        }
        
        main_i += (*selectOperand(main_i, indirect));       // indirect
        break;
    }
    return ControlUnit::Register(main_i, RAM[main_i]);;
} // decodeAdmo

void MARS::decodeModifier(ControlUnit *_CTRL)
{

    ASM::OPCODE _code = _CTRL->EXE.inst->OP.code;
    ASM::MOD    _mod  = _CTRL->EXE.inst->OP.mod;

    // ignore modifier
    switch (_code)
    {
    case ASM::OPCODE::DAT: // ignored
    case ASM::OPCODE::JMP:
    case ASM::OPCODE::SPL: return;

    case ASM::OPCODE::JMZ: // only change DEST operands on .B
    case ASM::OPCODE::JMN:
    case ASM::OPCODE::DJN: 
        if      (_mod == ASM::MOD::B) goto SWAP_DEST;
        else if (_mod == ASM::MOD::F) _CTRL->TYPE.mod = ASM::MOD_TYPE::DOUBLE;
        return;
    }

    // adjust operand value pointers base on mod type
    switch (_mod)
    {
    /* Single [default] */
    case ASM::MOD::A:  break; 
    case ASM::MOD::AB: goto SWAP_DEST;  // only dest
    case ASM::MOD::BA:                  // only src
    case ASM::MOD::B: // SRC A <-> B
            swapElements(_CTRL->SRC.A, _CTRL->SRC.B);
            goto SWAP_DEST;
        
    /* Double */ 
    case ASM::MOD::F: _CTRL->TYPE.mod  = ASM::MOD_TYPE::DOUBLE; break;
    case ASM::MOD::X: _CTRL->TYPE.mod  = ASM::MOD_TYPE::DOUBLE; goto SWAP_DEST;

    /* Full */
    case ASM::MOD::I:
        switch (_code)
        {
        case ASM::OPCODE::MOV:
        case ASM::OPCODE::SEQ:
        case ASM::OPCODE::SNE: _CTRL->TYPE.mod = ASM::MOD_TYPE::FULL;   break; // MOV, SEQ, SNE (only)
        default:               _CTRL->TYPE.mod = ASM::MOD_TYPE::DOUBLE; break; // else F
        } break;
    default: return;

        SWAP_DEST:
        switch (_mod)
        {
        case ASM::MOD::B:
        case ASM::MOD::AB:
        case ASM::MOD::X: // DEST A <-> B
            swapElements(_CTRL->DEST.A, _CTRL->DEST.B); // PTR
        } return;
    }
} // decodeModifier

ControlUnit MARS::generateCTRL(int const _pc)
{
    ControlUnit CTRL_ = ControlUnit(_pc, &RAM);

    // decode addressing modes, get SRC (A) and DEST (B) registers
    CTRL_.SRC  = decodeAdmo(&CTRL_, OPR::A);
    CTRL_.DEST = decodeAdmo(&CTRL_, OPR::B);
                 decodeModifier(&CTRL_);

    // determine opcode type
    switch (CTRL_.EXE.inst->OP.code)
    {
    /* Read/Write */
    case ASM::OPCODE::DAT:
    case ASM::OPCODE::MOV: CTRL_.TYPE.code = ASM::OPCODE_TYPE::READWRITE;   break;
    /* Comparision */
    case ASM::OPCODE::SEQ:
    case ASM::OPCODE::SLT:
    case ASM::OPCODE::SPL: CTRL_.TYPE.code = ASM::OPCODE_TYPE::COMPARISION; break;
    /* Arithmetic */
    case ASM::OPCODE::ADD:
    case ASM::OPCODE::SUB:
    case ASM::OPCODE::MUL:
    case ASM::OPCODE::DIV:
    case ASM::OPCODE::MOD: CTRL_.TYPE.code = ASM::OPCODE_TYPE::ARITHMETIC;  break;
    /* Jump */
    case ASM::OPCODE::JMP:
    case ASM::OPCODE::JMZ:
    case ASM::OPCODE::JMN:
    case ASM::OPCODE::DJN: CTRL_.TYPE.code = ASM::OPCODE_TYPE::JUMP;        break;
    }
    CTRL_.EXE.event = Event::EXECUTE;

    #ifdef MARS_DEBUG
    printf("\nMARS::generateCTRL:\t (EXE)\t |%s| \t Index[%d] \n\t\t\t (SRC)\t '%s' \n\t\t\t (DEST)\t '%s' \n",
            CTRL_.EXE.inst->toAsmCode().c_str(), CTRL_.EXE.index,
            CTRL_.SRC.inst->toAsmCode().c_str(), CTRL_.DEST.inst->toAsmCode().c_str());
    #endif

    return CTRL_;
} // nextCommit

ASM::Inst &MARS::operator[](int index) const { return *RAM[index]; }
ASM::Inst &MARS::operator[](int index)       { return *RAM[index]; }
    
} // namespace OS
