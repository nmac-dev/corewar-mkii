// memory array assembly simulator containts array of instruction objects and decoding functions

#include "mars.hpp"

namespace OS
{

MARS::CommitLog::CommitLog(int _ir_pc)
{
    ir_pc = _ir_pc;
    
    adr_src = 0,       adr_dest = 0;
    src_A   = nullptr, src_B    = nullptr;
    dest_A  = nullptr, dest_B   = nullptr;
    pi_a    = 0,       pi_b     = 0;
    pd_a    = 0,       pd_b     = 0;

    event_src   = Event::NOOP;
    event_dest  = Event::NOOP;
}
MARS::CommitLog::CommitLog(){}

MARS::MARS(ASM::WarriorList &warriors, int _size, int _seperation)
{
    core_size      = _size;
    min_seperation = _seperation;

    // populate core with (dat #0, #0) asm instructions
    RAM = RAM_T(new ASM::Inst[core_size]);

    // place warriors in core at random positions
    for (int i = 0; i < warriors.size(); i++)
    {
        ASM::Warrior *warrior_i = &warriors[i];
        uint32_t rnd_pos        = randomInt(core_size);

        // validate position meets minimum seperation requirements
        for (int k = 0; k < i; k++)
        {
            // get upper & lower limits for seperation boundry
            int upper   = loopIndex(rnd_pos + min_seperation),
                lower   = loopIndex(rnd_pos - min_seperation);
            int w_index = warrior_i->getCoreIndex();

            if (w_index > lower && w_index < upper)
            {
                // failed; generate new position, restart loop
                rnd_pos = randomInt(core_size);
                k = 0;
            }
        }
        warrior_i->setCoreIndex(rnd_pos);

        // add each warrior instruction into the core
        for (int j = 0; j < warrior_i->len(); j++)
        {
            RAM[rnd_pos++] = (*warrior_i)[j];
        }

        #ifdef MARS_DEBUG
            if (i == 0) printf("\nMARS::MARS: \n");
            printf("\tAdded |%d| instructions @ index [%d] \t...from: [%d] '%s'\n",
                    warrior_i->len(), warrior_i->getCoreIndex(),
                    warrior_i->getUUID(), warrior_i->getName().c_str());
        #endif
    }
}
MARS::MARS(){}

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

void MARS::decodeAdmo(CommitLog *cl, int op_select)
{
    ASM::_AM ir_admo;           // addressing mode
    int      ir_operand;        // operand

    int *adr, **src, **dest;    // commit log pointer adr|src|dest

    int ir_pc = cl->ir_pc;      // instruction register program counter
    // select A
    if (op_select == 0)
    {
        ir_admo     = RAM[ir_pc].admo_a;
        ir_operand  = RAM[ir_pc].operand_a;
        adr         = &cl->adr_src;
        src         = &cl->src_A;
        dest        = &cl->dest_A;
    }
    else // B
    {
        ir_admo     = RAM[ir_pc].admo_b;
        ir_operand  = RAM[ir_pc].operand_b;
        adr         = &cl->adr_dest;
        src         = &cl->src_B;
        dest        = &cl->dest_B;
    }
    
    // process addressing mode
    switch (ir_admo)
    {
        case ASM::_AM::IMMEDIATE: // address is 0 (stored value)
            *adr  = 0;
            *src  = selectOperand(ir_pc, op_select);
            *dest = *src;
            break;
        case ASM::_AM::DIRECT:    // direct adr = ir_operand 
            *adr  = ir_pc;
            *src  = selectOperand(ir_pc, op_select);
            *dest = selectOperand(ir_pc + (*adr), op_select);
            break;
        /* Indirect */
        case ASM::_AM::INDIRECT_A: // direct adr + dest value = indirect
        case ASM::_AM::PRE_DEC_A:
        case ASM::_AM::POST_INC_A:
            *adr  = ir_operand;
            *src  = &RAM[loopIndex(ir_pc + (*adr))].operand_a; // arg pointer: ignore admo
            *dest = &RAM[loopIndex(ir_pc + (*adr) + (*(*src)))].operand_a;
            goto INDIRECT_PRE_POST;
        case ASM::_AM::INDIRECT_B:
        case ASM::_AM::PRE_DEC_B:
        case ASM::_AM::POST_INC_B:
            *adr  = ir_operand;
            *src  = &RAM[loopIndex(ir_pc + (*adr))].operand_b; // arg pointer: ignore admo
            *dest = &RAM[loopIndex(ir_pc + (*adr) + (*(*src)))].operand_b;
            goto INDIRECT_PRE_POST;
        /* apply dec/inc */
        INDIRECT_PRE_POST:
        switch (ir_admo)
        {
            /* pre/dec */
            case ASM::_AM::PRE_DEC_A:
                cl->pd_a = -1;
                break;
            case ASM::_AM::PRE_DEC_B:
                cl->pd_a = -1;
                break;
            /* post/inc */
            case ASM::_AM::POST_INC_A:
                cl->pi_a = 1; // pointer admo: mark A for post-increment
                break;
            case ASM::_AM::POST_INC_B:
                cl->pi_b = 1; // pointer admo: mark B for post-increment
                break;
        }
    }
} //// decodeAdmo()

void MARS::decodeModifier(MARS::CommitLog *cl)
{
    ASM::_MOD mod = RAM[cl->ir_pc].modifier;
    
    int *swap_ptr;
    // determine mod type
    switch (mod)
    {
    case ASM::_MOD::A:
    case ASM::_MOD::B:
    case ASM::_MOD::AB:
    case ASM::_MOD::BA:
        cl->mod_type = ASM::_MOD_TYPE::SINGLE;
        goto SWAP_OPERANDS;
    case ASM::_MOD::F:
    case ASM::_MOD::X:
        cl->mod_type = ASM::_MOD_TYPE::DOUBLE;
        goto SWAP_OPERANDS;
    case ASM::_MOD::I:
        cl->mod_type = ASM::_MOD_TYPE::FULL;
        break;

    // swap destination operand pointers
    SWAP_OPERANDS:
        switch (mod)
        {
        // src  A <-> B
        // dest A <-> B
        case ASM::_MOD::B:
            // src
            swap_ptr   = cl->src_A;
            cl->src_A  = cl->src_B;
            cl->src_B  = swap_ptr;
            // dest
            swap_ptr   = cl->dest_A;
            cl->dest_A = cl->dest_B;
            cl->dest_B = swap_ptr;
            break;
        // src  A <-> B
        case ASM::_MOD::BA:
            swap_ptr  = cl->src_A;
            cl->src_A = cl->src_B;
            cl->src_B = swap_ptr;
            break;
        // dest A <-> B
        case ASM::_MOD::AB:
        case ASM::_MOD::X:
            swap_ptr   = cl->dest_A;
            cl->dest_A = cl->dest_B;
            cl->dest_B = swap_ptr;
            break;
        }
    }
}

MARS::CommitLog MARS::nextCommit(int const ir_pc)
{
    CommitLog cl_ = CommitLog(ir_pc);   // set commit location
    int A = 0,  // select A
        B = 1;  // B

    decodeAdmo(&cl_, A);
    decodeAdmo(&cl_, B);

    // determine commit type
    switch (RAM[ir_pc].opcode)
    {
    /* Read/Write */
    case ASM::_OP::DAT:
    case ASM::_OP::MOV:
        cl_.commit_type = ASM::_OP_TYPE::READWRITE;
        break;
    /* Comparision */
    case ASM::_OP::CMP:
    case ASM::_OP::SLT:
    case ASM::_OP::SPL:
        cl_.commit_type = ASM::_OP_TYPE::COMPARE;
        break;
    /* Arithmetic */
    case ASM::_OP::ADD:
    case ASM::_OP::SUB:
    case ASM::_OP::MUL:
    case ASM::_OP::DIV:
    case ASM::_OP::MOD:
        cl_.commit_type = ASM::_OP_TYPE::ARITHMETIC;
        break;
    /* Jump */
    case ASM::_OP::JMP:
    case ASM::_OP::JMZ:
    case ASM::_OP::JMN:
    case ASM::_OP::DJN:
        cl_.commit_type = ASM::_OP_TYPE::JUMP;
        break;
    }
    decodeModifier(&cl_);

    return cl_;
}

ASM::Inst &MARS::operator[](int index) const { return RAM[index]; }
ASM::Inst &MARS::operator[](int index)       { return RAM[index]; }
    
} //// namespace OS
