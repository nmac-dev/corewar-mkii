// memory array assembly simulator containts array of instruction objects and decoding functions

#include "mars.hpp"

namespace OS
{

MARS::CommitLog::CommitLog(int _ir_pc)
{
    ir_pc       = _ir_pc;
    opcode      = ASM::_OP::DAT;
    adr_A       = 0;
    adr_B       = 0;
    src_A       = nullptr;
    src_B       = nullptr;
    dest_A      = nullptr;
    dest_B      = nullptr;
    pip_a       = 0;
    pip_b       = 0;
    event_src   = Event::NOOP;
    event_dest  = Event::NOOP;
}
MARS::CommitLog::CommitLog(){}

MARS::MARS(ASM::WarriorList &warriors, int _size, int _seperation)
{
    core_size      = _size;
    min_seperation = _seperation;

    // populate core with (dat #0, #0) asm instructions
    RAM = new ASM::Inst[core_size];

    // place warriors in core at random positions
    for (int i = 0; i < warriors.size(); i++)
    {
        ASM::Warrior &warrior_i = warriors[i];
        uint32_t rnd_pos = randomInt(core_size);

        // validate position meets minimum seperation requirements
        for (int k = 0; k < i; k++)
        {
            // get upper & lower limits for seperation boundry
            int upper   = loopIndex(rnd_pos + min_seperation),
                lower   = loopIndex(rnd_pos - min_seperation);
            int w_index = warrior_i.getCoreIndex();

            if (w_index > lower && w_index < upper)
            {
                // failed; generate new position, restart loop
                rnd_pos = randomInt(core_size);
                k = 0;
            }
        }

        // add each warrior instruction into the core
        for (int j = 0; j < warrior_i.len(); j++)
        {
            RAM[rnd_pos++] = warrior_i[j];
        }
        warrior_i.setCoreIndex(rnd_pos);

        #ifdef MARS_DEBUG
            if (i == 0) printf("\nMARS::MARS: \n");
            printf("\tAdded '%s' at index [%d] with |%d| instructions\n",
                    warrior_i.getName().c_str(), warrior_i.getCoreIndex(), warrior_i.len());
        #endif
    }
}
MARS::MARS(){}
MARS::~MARS() { delete[] RAM; }

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
        adr         = &cl->adr_A;
        src         = &cl->src_A;
        dest        = &cl->dest_A;
    }
    else // B
    {
        ir_admo     = RAM[ir_pc].admo_b;
        ir_operand  = RAM[ir_pc].operand_b;
        adr         = &cl->adr_B;
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
            *adr  = ir_operand;
            *src  = selectOperand(ir_pc, op_select);
            *dest = selectOperand(ir_pc + ir_operand, op_select);
            break;
        /* Indirect */
        case ASM::_AM::INDIRECT_A: // direct adr + dest value = indirect
        case ASM::_AM::PRE_DEC_A:
        case ASM::_AM::POST_INC_A:
            *adr  = ir_operand;
            *src  = &RAM[loopIndex(ir_pc + ir_operand)].operand_a; // arg pointer: ignore admo
            *dest = &RAM[loopIndex(ir_pc + ir_operand + (*(*src)))].operand_a;
            goto INDIRECT_PRE_POST;
        case ASM::_AM::INDIRECT_B:
        case ASM::_AM::PRE_DEC_B:
        case ASM::_AM::POST_INC_B:
            *adr  = ir_operand;
            *src  = &RAM[loopIndex(ir_pc + ir_operand)].operand_b; // arg pointer: ignore admo
            *dest = &RAM[loopIndex(ir_pc + ir_operand + (*(*src)))].operand_b;
            goto INDIRECT_PRE_POST;
        /* apply dec/inc */
        INDIRECT_PRE_POST:
        switch (ir_admo)
        {
            /* pre/dec */
            case ASM::_AM::PRE_DEC_A:
                cl->pdp_a = -1;
                break;
            case ASM::_AM::PRE_DEC_B:
                cl->pdp_a = -1;
                break;
            /* post/inc */
            case ASM::_AM::POST_INC_A:
                cl->pip_a = 1; // pointer admo: mark A for post-increment
                break;
            case ASM::_AM::POST_INC_B:
                cl->pip_b = 1; // pointer admo: mark B for post-increment
                break;
        }
    }
} //// decodeAdmo()

MARS::CommitLog MARS::nextCommit(int ir_pc)
{
    CommitLog cl_ = CommitLog(ir_pc);   // set commit location
    int A = 0,  // select A
        B = 1;  // B

    decodeAdmo(&cl_, A); // decode instruction register's operand A addressing mode
    decodeAdmo(&cl_, B); // B

    return cl_;
}

ASM::Inst &MARS::operator[](int index) const
{
    return RAM[index];
}
ASM::Inst &MARS::operator[](int index)
{
    return RAM[index];
}
    
} //// namespace OS
