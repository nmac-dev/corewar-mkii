// memory array assembly simulator containts array of instruction objects and decoding functions

#include "OS/memory.hpp"

namespace OS
{
Memory::Memory(ProgramVec *_programs, int _min_seperation)
{
    ini_min_seperation = _min_seperation;

    // populate RAM with (dat #0, #0) asm instructions
    RAM = C_RAM<Inst>(ram_size);

    // place programs in core at random positions
    for (int i = 0; i < _programs->size(); i++)
    {
        Program program_i = *(*_programs)[i].get();
        uint32_t rnd_pos  = random_int(ram_size);

        // validate position meets minimum seperation requirements
        for (int k = 0; k < i; k++)
        {
            // get upper & lower limits for seperation boundry
            int upper   = RAM.loop_index(rnd_pos + ini_min_seperation),
                lower   = RAM.loop_index(rnd_pos - ini_min_seperation);
            int w_index = program_i.address();

            if (w_index > lower && w_index < upper)
            {
                // failed; generate new position, restart loop
                rnd_pos = random_int(ram_size);
                k = 0;
            }
        }
        (*_programs)[i].get()->set_address(rnd_pos);

        // add each program instruction into the core
        for (int j = 0; j < program_i.len(); j++)
        {
            *RAM[rnd_pos++] = program_i[j];
        }

        #ifdef MEMORY_DEBUG
        if (i == 0) printf("\n Memory::Memory: \n");
        printf("\tAdded |%d| instructions @ address [%d] \t...from: [%d] '%s'\n",
                program_i.len(), (*_programs)[i].get()->address(),
                program_i.uuid(), program_i.name().c_str());
        #endif
    }
}
Memory::Memory() = default;

uint32_t Memory::random_int(uint32_t max_range)
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

    return seed % max_range;
}

Register Memory::decode_admo(ControlUnit *_ctrl, InstField const exe_select)
{
    Admo _admo = (exe_select == InstField::A) ? _ctrl->EXE.A->admo  // SRC
                                              : _ctrl->EXE.B->admo; // DEST
    InstField indirect_select;
    int main_i  = _ctrl->EXE.address;

    bool indirect_type = false;
    switch (_admo)
    {
        case Admo::IMMEDIATE:
        {
            // relative address is 0
            break;
        }
        case Admo::DIRECT:
        {
            main_i += (*fetch_operand(main_i, exe_select));
            break;
        }
        /* Indirect */
        case Admo::PRE_DEC_A:
        case Admo::POST_INC_A:
        case Admo::INDIRECT_A:
        {
            indirect_select = InstField::A;
            indirect_type = true;
            break;
        }
        case Admo::PRE_DEC_B:
        case Admo::POST_INC_B:
        case Admo::INDIRECT_B:
        {
            indirect_select = InstField::B;
            indirect_type = true;
            break;
        }
        default: break;
    }

    if (indirect_type)
    {
        main_i += (*fetch_operand(main_i, exe_select));             // direct

        if (_admo == Admo::PRE_DEC_A || _admo == Admo::PRE_DEC_B)
        {
            (*fetch_operand(main_i, indirect_select)) += -1;
        }
        if (_admo == Admo::POST_INC_A || _admo == Admo::POST_INC_B)
        {
            int *post_inc = fetch_operand(main_i, indirect_select);
            if(exe_select == InstField::A)
            {
                 _ctrl->post_A = post_inc;
            }
            else _ctrl->post_B = post_inc;
        }
        main_i += (*fetch_operand(main_i, indirect_select));        // indirect
    }
    return Register(main_i, RAM[main_i]);;
} /* decode_admo() */

void Memory::decode_modifier(ControlUnit *_ctrl)
{
    Opcode _code = _ctrl->EXE.OP->code;
    Modifier    _mod  = _ctrl->EXE.OP->mod;

    // allows -> ModifierType::FULL
    bool full_type = false;
    // swap flags
    bool swap_src  = false,
         swap_dest = false;

    /* [code] Conditionals */
    switch (_code)
    {
        case Opcode::NOP:
        case Opcode::DAT:
        case Opcode::JMP:
        case Opcode::SPL:
        {   
            /* ignore modifier */
            return;
        }
        case Opcode::MOV:
        case Opcode::SEQ:
        case Opcode::SNE:
        {
            full_type = true;
            break;
        }
        case Opcode::JMZ: // only change DEST operands on .B
        case Opcode::JMN:
        case Opcode::DJN:
        {
            if (_mod == Modifier::B)
            {
                swap_dest = true;
                _mod = Modifier::A;
            }
            else if (_mod == Modifier::F)
            {
                _ctrl->TYPE.mod = ModifierType::DOUBLE;
                return;
            }
            break;
        }
        default: break;
    }

    /* Apply Modifiers */
    switch (_mod)
    {
        /* Single [default] */
        case Modifier::A:  break; 
        case Modifier::AB:
        {
            swap_dest = true;
            break;
        }
        case Modifier::BA:
        {
            swap_src = true;
            break;
        }
        case Modifier::B:
        {
            swap_src = swap_dest = true;
            break;
        }
        /* Double */ 
        case Modifier::F:
        {
            _ctrl->TYPE.mod = ModifierType::DOUBLE; 
            break;
        }
        case Modifier::X:
        {
            swap_dest = true;
            _ctrl->TYPE.mod = ModifierType::DOUBLE; 
            break;
        }
        /* Full */ 
        case Modifier::I:
        {
            if (full_type)
            {
                 _ctrl->TYPE.mod = ModifierType::FULL;
            }
            else _ctrl->TYPE.mod = ModifierType::DOUBLE; // else Modifier::F
            break;
        }
        default: break;
    }

    if (swap_src)                   // SRC A <-> B
    {
        _ctrl->SRC.swap_operands();
    }
    if (swap_dest)                  // DEST A <-> B
    {
        _ctrl->DEST.swap_operands();
    }
} /* decode_modifier() */

ControlUnit Memory::generate_ctrl(int const _pc)
{
    ControlUnit ctrl_ ( {_pc, RAM[_pc]} );

    // decode addressing modes, get SRC (A) and DEST (B) registers
    ctrl_.SRC  = decode_admo(&ctrl_, InstField::A);
    ctrl_.DEST = decode_admo(&ctrl_, InstField::B);
                 decode_modifier(&ctrl_);

    // determine opcode type
    switch (ctrl_.EXE.OP->code)
    {
        /* System */
        case Opcode::NOP:
        case Opcode::DAT:
        case Opcode::MOV:
        case Opcode::SPL:
        {
            ctrl_.TYPE.code = OpcodeType::SYSTEM;
            break;
        }
        /* Comparision */
        case Opcode::SEQ:
        case Opcode::SNE:
        case Opcode::SLT:
        {
            ctrl_.TYPE.code = OpcodeType::COMPARISION;
            break;
        }
        /* Arithmetic */
        case Opcode::ADD:
        case Opcode::SUB:
        case Opcode::MUL:
        case Opcode::DIV:
        case Opcode::MOD: 
        {
            ctrl_.TYPE.code = OpcodeType::ARITHMETIC;
            break;
        }
        /* Jump */
        case Opcode::JMP:
        case Opcode::JMZ:
        case Opcode::JMN:
        case Opcode::DJN: 
        {
            ctrl_.TYPE.code = OpcodeType::JUMP;
            break;
        }
        default: break;
    }

    #ifdef MEMORY_DEBUG
    printf("\n Memory::generate_ctrl:\t (EXE)  [%d]'%s' \n"
                          "\n\t\t\t (SRC)  [%d]'%s' \n"
                          "\n\t\t\t (DEST) [%d]'%s' \n",
        ctrl_.EXE.address,  RAM[ctrl_.EXE.address]->to_assembly().c_str(),
        ctrl_.SRC.address,  RAM[ctrl_.SRC.address]->to_assembly().c_str(),
        ctrl_.DEST.address, RAM[ctrl_.DEST.address]->to_assembly().c_str());
    #endif

    return ctrl_;
} /* generate_ctrl() */

Inst &Memory::operator[](int address) const { return *RAM[address]; }
Inst &Memory::operator[](int address)       { return *RAM[address]; }
    
} /* ::OS */
