/// Pushes fetch/decode/execute cycles to Memory and selects program processes from the scheduler

#include "OS/cpu.hpp"

/// Operating System handles: fetch/decode/execute cycle, memory, and processes
namespace OS
{
CPU::CPU(Memory *_memory, Scheduler *_sched)
{
    os_memory = _memory;
    os_sched  = _sched;
}
CPU::CPU() = default;

Report CPU::run_fde_cycle()
{
 /* Fetch */
    int exe_pc;                               // instruction register program counter
    exe_process = os_sched->fetch_next();     // fetch next process

 /* Decode */
    exe_process >> exe_pc;
    ctrl        = os_memory->generate_ctrl(exe_pc); // generate control unit

    #ifdef CPU_DEBUG
    printf("\nCore::run_fde_cycle:\t Cycle:[%d]\t Program:[%d] Index:[%d] Inst:'%s' \n",
        os_sched->cycles(), exe_process.parent_id(), 
        ctrl.EXE.address, (*os_memory)[ctrl.EXE.address].to_assembly().c_str());
    #endif

 /* Execute */
    if (exe_process.status() < Status::HAULTED)
    {
        exe_process << ctrl.EXE.address +1;   // set program counter

        ctrl.EXE.event = Event::EXECUTE;
        switch (ctrl.TYPE.code)
        {
            case OpcodeType::SYSTEM:
            {
                execute_system();
                break;
            }
            case OpcodeType::COMPARISION:
            {
                execute_compare();
                break;
            }
            case OpcodeType::ARITHMETIC:
            {
                execute_arithmetic();
                break;
            }
            case OpcodeType::JUMP:
            {
                execute_jump();
                break;
            }
            default:
            {
                #ifdef CPU_DEBUG_CODES
                printf("ERROR! CPU:: FDE cycle: undefined 'OPCODE TYPE'");
                #endif
                break;
            }
        }
        if (ctrl.EXE.event == Event::ILLEGAL)
            os_sched->kill_process(&exe_process);
    }
    os_sched->return_process(&exe_process);   

    // SPL request new process
    if(exe_process.status() == Status::NEW)
    {
        os_sched->add_process(exe_process.parent_id(), ctrl.SRC.address);
    }

    os_memory->encode_ctrl(&ctrl);
    os_memory->apply_post_inc(ctrl);

    return Report(exe_process, ctrl);
} /* nextFDEcycle() */

void CPU::execute_system()
{
    Opcode       code_  = ctrl.EXE.OP->code;
    ModifierType mod_t  = ctrl.TYPE.mod;
    Register     &SRC_  = ctrl.SRC,
                 &DEST_ = ctrl.DEST;

    switch (code_)
    {
        case Opcode::NOP:
        {
            ctrl.EXE.event  = Event::NOOP;
            SRC_.event      = Event::NOOP;
            DEST_.event     = Event::NOOP;
            break;
        }
        case Opcode::DAT:
        {
            ctrl.EXE.event  = Event::ILLEGAL;
            SRC_.event  = Event::NOOP;
            DEST_.event = Event::NOOP;
            break;
        }
        case Opcode::MOV:
        {
            switch (mod_t)
            {
                case ModifierType::DOUBLE:
                {
                    DEST_.B->val = SRC_.B->val; 
                    /* ->::SINGLE */
                }
                case ModifierType::SINGLE:
                {
                    DEST_.A->val = SRC_.A->val;
                    break;
                }
                case ModifierType::FULL:
                {
                    *ctrl.DEST.OP = *ctrl.SRC.OP;
                    *ctrl.DEST.A  = *ctrl.SRC.A;
                    *ctrl.DEST.B  = *ctrl.SRC.B;
                    break;
                }
            } /* switch() */
            SRC_.event  = Event::READ;
            DEST_.event = Event::WRITE;
            break;
        }
        case Opcode::SPL:
        {
            exe_process.set_status(Status::NEW); // new process after return
            break;
        }
        default:
        {
            #ifdef CPU_DEBUG_CODES
            printf("ERROR! CPU:: 'execute SYSTEM' opcode not found");
            #endif
            break;
        }
    } /* switch() */
} /* ::execute_system() */

void CPU::execute_compare()
{
    Opcode       code_  = ctrl.EXE.OP->code;
    ModifierType mod_t  = ctrl.TYPE.mod;
    Register     &SRC_  = ctrl.SRC,
                 &DEST_ = ctrl.DEST;

    SRC_.event  = Event::READ;
    DEST_.event = Event::READ;

    // comparison flag (assume true)
    bool compare_flag = true;

    // SEQ, SNE
    if (code_ != Opcode::SLT)
    {    
        if (mod_t == ModifierType::FULL)
        {
            // entire instruction comparison
            compare_flag =  SRC_.OP->code == DEST_.OP->code     // [code].<mod>
                         && SRC_.OP->mod  == DEST_.OP->mod      //
                         && SRC_.A->admo  == DEST_.A->admo      // <admo>[A],
                         && SRC_.A->val   == DEST_.A->val       //
                         && SRC_.B->admo  == DEST_.B->admo      // <admo>[B]
                         && SRC_.B->val   == DEST_.B->val;      //
        }
        else
        {
            if (mod_t == ModifierType::DOUBLE)
            {
                compare_flag  = SRC_.B->val - DEST_.B->val == 0;
            }
            compare_flag &= SRC_.A->val - DEST_.A->val == 0; 
        }
    }

    /* OPCODES */
    bool skip_next = true;
    switch (code_)
    {
        case Opcode::SEQ:
        {
            skip_next = compare_flag;
            break;
        }
        case Opcode::SNE:
        {
            skip_next = !compare_flag;
            break;
        }
        case Opcode::SLT:
        {
            if(mod_t == ModifierType::DOUBLE)
            {
                skip_next = SRC_.B->val < DEST_.B->val;
            }
            skip_next    &= SRC_.A->val < DEST_.A->val;
            break;
        }
        default:
        {
            #ifdef CPU_DEBUG_CODES
            printf("ERROR! CPU:: 'execute COMPARISION' opcode not found"); 
            #endif
            break;
        }
    } /* switch() */

    if (skip_next)
    {
        exe_process << ctrl.EXE.address + 2;
    }
} /* execute_compare() */

void CPU::execute_arithmetic()
{
    Opcode       code_  = ctrl.EXE.OP->code;
    ModifierType mod_t  = ctrl.TYPE.mod;
    Register     &SRC_  = ctrl.SRC,
                 &DEST_ = ctrl.DEST;

    SRC_.event  = Event::READ;
    DEST_.event = Event::WRITE;

    char operator_char;
    switch (code_)
    {
        case Opcode::ADD: operator_char = '+'; break;
        case Opcode::SUB: operator_char = '-'; break;
        case Opcode::MUL: operator_char = '*'; break;
        case Opcode::DIV: operator_char = '/'; break;
        case Opcode::MOD: operator_char = '%'; break;

        default:
        {
            #ifdef CPU_DEBUG_CODES
            printf("ERROR! CPU:: 'execute ARITHMETIC' opcode not found"); 
            #endif
            return;
        }
    }

    // check for division by zero
    if (code_ == Opcode::DIV || code_ == Opcode::MOD)
    {
        bool zero_div = false; // true if division by zero
        
        if(mod_t == ModifierType::DOUBLE)
        {
            zero_div = !(SRC_.B->val && DEST_.B->val);
        }
        zero_div = !zero_div && !(SRC_.A->val && DEST_.A->val);

        if (zero_div) // kill process
        {
            ctrl.EXE.event = Event::ILLEGAL;
            SRC_.event     = Event::NOOP;
            DEST_.event    = Event::NOOP;
            return;
        }
    }

    if(mod_t == ModifierType::DOUBLE)
    {
        apply_arithmatic(DEST_.B->val, SRC_.B->val, operator_char);
    }
    apply_arithmatic(DEST_.A->val, SRC_.A->val, operator_char);
} /* execute_arithmetic() */

void CPU::execute_jump()
{
    Opcode       code_  = ctrl.EXE.OP->code;
    ModifierType mod_t  = ctrl.TYPE.mod;
    Register     &SRC_  = ctrl.SRC,
                 &DEST_ = ctrl.DEST;

    SRC_.event  = Event::READ;
    DEST_.event = Event::READ;

    // comparison boolean: true if target is zero
    bool compare_zero = true;
    if (code_ == Opcode::JMZ || code_ == Opcode::JMN)
    {
        if (mod_t == ModifierType::DOUBLE)
        {
            compare_zero  = DEST_.B->val == 0;
        }
        compare_zero &= DEST_.A->val == 0;
    }

    bool set_jump = false;
    switch (code_)
    {
        case Opcode::JMP:
        {
            set_jump    = true;
            DEST_.event = Event::NOOP;
            break;
        }
        case Opcode::JMZ:
        {
            set_jump = compare_zero;
            break;
        }
        case Opcode::JMN:
        { 
            set_jump = !compare_zero;
            break;
        }
        case Opcode::DJN:
        {
            compare_zero = false;
            if (mod_t == ModifierType::DOUBLE)
            {
                DEST_.B->val += -1;
                compare_zero  = DEST_.B->val == 0;
            }
            DEST_.A->val += -1;
            compare_zero |= DEST_.A->val == 0;

            set_jump = !compare_zero;
            break;
        }
        default:
        {
            #ifdef CPU_DEBUG_CODES
            printf("ERROR! CPU:: 'execute JUMP' opcode not found"); 
            #endif
            break;
        }
    }
    
    if (set_jump)
    {
        exe_process << SRC_.address;
    }    
} /* execute_jump() */

} /* ::OS */
