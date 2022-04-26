// memory array assembly simulator containts array of instruction objects and decoding functions

#include "mars.hpp"

namespace OS
{
/* ControlUnit */
ControlUnit::Register::Register(int _index, Inst *_inst)
{
    index = _index;
    OP    = &_inst->OP;
    A     = &_inst->A; 
    B     = &_inst->B;
    event = Event::NOOP; // event applied to the address
}
ControlUnit::Register::Register() = default;

ControlUnit::ControlUnit(int _pc, RAM_T<Inst> *RAM)
{
    EXE    = Register(_pc, (*RAM)[_pc]);
    SRC    = Register();  // EXE A
    DEST   = Register();  // EXE B

    post_A = nullptr;
    post_B = nullptr;

    TYPE.code = OPCODE_TYPE::READWRITE;
    TYPE.mod  = MOD_TYPE::SINGLE;
}
ControlUnit::ControlUnit() = default;

/* MARS */
MARS::MARS(WarriorList *warriors, int _seperation)
{
    min_seperation = _seperation;

    // populate RAM with (dat #0, #0) asm instructions
    RAM = RAM_T<Inst>(core_size);

    // place warriors in core at random positions
    for (int i = 0; i < warriors->size(); i++)
    {
        Warrior warrior_i = *(*warriors)[i].get();
        uint32_t rnd_pos  = randomInt(core_size);

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
    ADMO _admo  = (exe_select == OPR::A) ? _CTRL->EXE.A->admo  // SRC
                                         : _CTRL->EXE.B->admo; // DEST
    OPR indirect;
    int main_i  = _CTRL->EXE.index; // executing instruction index
    
    // process addressing mode
    int pre_dec = 0;                // pre-decrement flag: set to -1 when applied
    switch (_admo)
    {
    case ADMO::IMMEDIATE:
        // relative address is 0, use stored value
        break;

    case ADMO::DIRECT:
        main_i += (*selectOperand(main_i, exe_select));
        break;
        
    /* Indirect */
    case ADMO::PRE_DEC_A:  pre_dec  = -1;
    case ADMO::INDIRECT_A:
    case ADMO::POST_INC_A: indirect = OPR::A; goto CALC_INDIRECT;

    case ADMO::PRE_DEC_B:  pre_dec  = -1;
    case ADMO::INDIRECT_B:
    case ADMO::POST_INC_B: indirect = OPR::B; goto CALC_INDIRECT;

    CALC_INDIRECT:
        main_i += (*selectOperand(main_i, indirect));       // indirect A|B
        (*selectOperand(main_i, indirect)) += pre_dec;      // apply pre-dec flag

        switch (_admo)
        {   /* post-inc ptr */
            case ADMO::POST_INC_A: _CTRL->post_A = selectOperand(main_i, indirect); break;
            case ADMO::POST_INC_B: _CTRL->post_B = selectOperand(main_i, indirect); break;
        }
        
        main_i += (*selectOperand(main_i, indirect));       // indirect
        break;
    }
    return ControlUnit::Register(main_i, RAM[main_i]);;
} // decodeAdmo

void MARS::decodeModifier(ControlUnit *_CTRL)
{
    OPCODE _code = _CTRL->EXE.OP->code;
    MOD    _mod  = _CTRL->EXE.OP->mod;

    // ignore modifier
    switch (_code)
    {
    case OPCODE::NOP: // ignored
    case OPCODE::DAT:
    case OPCODE::JMP:
    case OPCODE::SPL: return;

    case OPCODE::JMZ: // only change DEST operands on .B
    case OPCODE::JMN:
    case OPCODE::DJN: 
        if      (_mod == MOD::B) goto SWAP_DEST;
        else if (_mod == MOD::F) _CTRL->TYPE.mod = MOD_TYPE::DOUBLE;
        return;
    }

    // adjust operand value pointers base on mod type
    switch (_mod)
    {
    /* Single [default] */
    case MOD::A:  break; 
    case MOD::AB: goto SWAP_DEST;  // only dest
    case MOD::BA:                  // only src
    case MOD::B: // SRC A <-> B
            swapElements(_CTRL->SRC.A, _CTRL->SRC.B);
            goto SWAP_DEST;
        
    /* Double */ 
    case MOD::F: _CTRL->TYPE.mod  = MOD_TYPE::DOUBLE; break;
    case MOD::X: _CTRL->TYPE.mod  = MOD_TYPE::DOUBLE; goto SWAP_DEST;

    /* Full */
    case MOD::I:
        switch (_code)
        {
        case OPCODE::MOV:
        case OPCODE::SEQ:
        case OPCODE::SNE: _CTRL->TYPE.mod = MOD_TYPE::FULL;   break; // MOV, SEQ, SNE (only)
        default:          _CTRL->TYPE.mod = MOD_TYPE::DOUBLE; break; // else F
        } break;
    default: return;

        SWAP_DEST:
        switch (_mod)
        {
        case MOD::B:
        case MOD::AB:
        case MOD::X: // DEST A <-> B
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
    switch (CTRL_.EXE.OP->code)
    {
    /* Read/Write */
    case OPCODE::DAT:
    case OPCODE::MOV: CTRL_.TYPE.code = OPCODE_TYPE::READWRITE;   break;
    /* Comparision */
    case OPCODE::SEQ:
    case OPCODE::SLT:
    case OPCODE::SPL: CTRL_.TYPE.code = OPCODE_TYPE::COMPARISION; break;
    /* Arithmetic */
    case OPCODE::ADD:
    case OPCODE::SUB:
    case OPCODE::MUL:
    case OPCODE::DIV:
    case OPCODE::MOD: CTRL_.TYPE.code = OPCODE_TYPE::ARITHMETIC;  break;
    /* Jump */
    case OPCODE::JMP:
    case OPCODE::JMZ:
    case OPCODE::JMN:
    case OPCODE::DJN: CTRL_.TYPE.code = OPCODE_TYPE::JUMP;        break;
    }
    CTRL_.EXE.event = Event::EXECUTE;

    #ifdef MARS_DEBUG
    printf("\nMARS::generateCTRL:\t (EXE)\t |%s| \t Index[%d] \n\t\t\t (SRC)\t '%s' \n\t\t\t (DEST)\t '%s' \n",
            CTRL_.EXE.inst->toAsmCode().c_str(), CTRL_.EXE.index,
            CTRL_.SRC.inst->toAsmCode().c_str(), CTRL_.DEST.inst->toAsmCode().c_str());
    #endif

    return CTRL_;
} // nextCommit

Inst &MARS::operator[](int index) const { return *RAM[index]; }
Inst &MARS::operator[](int index)       { return *RAM[index]; }
    
} // namespace OS
