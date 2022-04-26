/// Pushes fetch/decode/execute cycles to MARS and selects warrior processes from the scheduler

#include "core.hpp"

/// Operating System handles: fetch/decode/execute cycle, memory simulator, and warrior processes
namespace OS {

/* OS::Report */

 Report::Report(PCB& _prcs, ControlUnit& _CTRL, int _pcbs, int _cycles)
 {
     warrior_ID  = _prcs.getParentID();
     prcs_status = _prcs.getStatus();
     pcbs        = _pcbs;
     cycles      = _cycles;

     exe  = Log(_CTRL.EXE);
     src  = Log(_CTRL.SRC);
     dest = Log(_CTRL.DEST);
 }
 Report::Report() = default;

 Report::Log::Log(ControlUnit::Register &_reg)
 {
     index = _reg.index;
     event = _reg.event;
 }
 Report::Log::Log() = default;

 /* Core */

Core::Core(WarriorList *_warriors, int _seperation, int _cycles, int _processes) 
{
    mars      = MARS(_warriors, _seperation);   // must be first (warrior needs core_index)
    scheduler = Scheduler(_warriors, _cycles, _processes);
}

Report Core::nextFDECycle()
{
    /* Fetch */
    int IR_PC;                       // instruction register program counter
    PRCS = scheduler.nextProcess();  // fetch next process

    /* Decode */
    PRCS >> IR_PC;
    CTRL = mars.generateCTRL(IR_PC); // generate a control unit

    #ifdef CORE_DEBUG
    printf("\nCore::nextFDECycle:\t Cycle:[%d]\t Warrior:[%d] Index:[%d] Inst:'%s' \n",
        scheduler.cycle(), PRCS.getParentID(), CTRL.EXE.index, CTRL.EXE.to->smCode().c_str());
    #endif

    // check for victory or draw
    if (PRCS.getStatus() != Status::EXIT && PRCS.getStatus() != Status::HAULTED)
    {
        /* Execute */
        int jump_val = CTRL.EXE.index +1; // value to apply to program counter (next instruction) 
        switch (CTRL.TYPE.code)
        {
        case OPCODE_TYPE::READWRITE:
            execute_ReadWrite();
            break;
        case OPCODE_TYPE::COMPARISION:
            execute_Compare(jump_val);
            break;
        case OPCODE_TYPE::ARITHMETIC:
            execute_Arithmetic();
            break;
        case OPCODE_TYPE::JUMP:
            execute_Jump(jump_val);
            break;
        }
        // check if killed
        if (PRCS.getStatus() != Status::TERMINATED)
        {
            scheduler.pushJump(PRCS, jump_val);
        }

        applyPostIncrement(CTRL.post_A);  // apply post-increment
        applyPostIncrement(CTRL.post_B);
    }
    return Report(PRCS, CTRL, scheduler.warriorPCBs(PRCS.getParentID()), scheduler.cycles());
}

void Core::execute_ReadWrite()
{
    ControlUnit::Register &_SRC  = CTRL.SRC,
                          &_DEST = CTRL.DEST;
    switch (CTRL.EXE.OP->code)
    {
    case OPCODE::NOP: return;

    case OPCODE::DAT:

        scheduler.killBackProcess(PRCS);
        CTRL.SRC.event  = Event::NOOP;
        CTRL.DEST.event = Event::NOOP;
        break;

    case OPCODE::MOV:

        switch (CTRL.TYPE.mod)
        {
        case MOD_TYPE::DOUBLE: _DEST.B->val = _SRC.B->val; // doubles bleed into singles
        case MOD_TYPE::SINGLE: _DEST.A->val = _SRC.A->val; break;
        case MOD_TYPE::FULL:   
            *CTRL.DEST.OP = *CTRL.SRC.OP; 
            *CTRL.DEST.A  = *CTRL.SRC.A; 
            *CTRL.DEST.B  = *CTRL.SRC.B; 
            break;
        }
        CTRL.SRC.event  = Event::READ;
        CTRL.DEST.event = Event::WRITE;
    }
}

void Core::execute_Compare(int &jump_val)
{
    ControlUnit::Register &_SRC  = CTRL.SRC,
                          &_DEST = CTRL.DEST;

    CTRL.SRC.event  = Event::READ;
    CTRL.DEST.event = Event::READ;

    // comparison boolean
    bool compare_eq = true;
    switch (CTRL.TYPE.mod)
    {
    case MOD_TYPE::DOUBLE: compare_eq  = _SRC.B->val - _DEST.B->val == 0;
    case MOD_TYPE::SINGLE: compare_eq &= _SRC.A->val - _DEST.A->val == 0; break;

    case MOD_TYPE::FULL:
        ControlUnit::Register &src_i  = CTRL.SRC,
                              &dest_i = CTRL.DEST;
        // entire instruction comparison
        compare_eq =   src_i.OP->code == dest_i.OP->code    // opcode
                    && src_i.OP->mod  == dest_i.OP->mod     // modifier
                    && src_i.A->admo  == dest_i.A->admo     // A admo
                    && src_i.B->admo  == dest_i.B->admo     // B admo
                    && src_i.A->val   == dest_i.A->val      // A val
                    && src_i.B->val   == dest_i.B->val;     // A val
    }

    switch (CTRL.EXE.OP->code)
    {
    case OPCODE::SEQ: if(compare_eq)          break;   else return; // return skips jump++
    case OPCODE::SNE: if(compare_eq == false) break;   else return;

    case OPCODE::SLT:
        switch (CTRL.TYPE.mod)
        {
        case MOD_TYPE::DOUBLE: if (_SRC.B->val < _DEST.B->val) /* bleed */; else return;
        case MOD_TYPE::SINGLE: if (_SRC.A->val < _DEST.A->val) break;       else return;
        }
    }
    jump_val++;
}

void Core::execute_Arithmetic()
{
    ControlUnit::Register &_SRC  = CTRL.SRC,
                          &_DEST = CTRL.DEST;

    CTRL.SRC.event  = Event::READ;
    CTRL.DEST.event = Event::WRITE;

    // select operator argument
    char operator_char;
    switch (CTRL.EXE.OP->code)
    {
    case OPCODE::ADD: operator_char = '+'; break;
    case OPCODE::SUB: operator_char = '-'; break;
    case OPCODE::MUL: operator_char = '*'; break;
    case OPCODE::DIV: operator_char = '/'; break;
    case OPCODE::MOD: operator_char = '%'; break;
    }

    // check for division by zero
    if (operator_char == '/' || operator_char == '%')
    {
        bool zero_div = false; // true if division by zero
        switch (CTRL.TYPE.mod)
        {
        case MOD_TYPE::DOUBLE: zero_div = !(_SRC.B->val && _DEST.B->val); // NOT 0
        case MOD_TYPE::SINGLE: zero_div = !zero_div && !(_SRC.A->val && _DEST.A->val); break;
        }

        if (zero_div) // kill process
        {
            scheduler.killBackProcess(PRCS);
            CTRL.SRC.event = Event::NOOP;
            CTRL.SRC.event = Event::NOOP;
            return;
        }
    }

    switch (CTRL.TYPE.mod)
    {
    case MOD_TYPE::DOUBLE: // bleeds into single
        _DEST.B->val = arithOpFilter(_DEST.B->val, _SRC.B->val, operator_char);

    case MOD_TYPE::SINGLE:
        _DEST.A->val = arithOpFilter(_DEST.A->val, _SRC.A->val, operator_char);
    }
}

void Core::execute_Jump(int &jump_val)
{
    OPCODE code = CTRL.EXE.OP->code;
    ControlUnit::Register &_SRC  = CTRL.SRC,
                          &_DEST = CTRL.DEST;

    CTRL.SRC.event  = Event::READ;
    CTRL.DEST.event = (code == OPCODE::JMP) ? Event::NOOP
                                                 : Event::READ; // JMZ, JMN, DJN

// comparison boolean: true if target is zero
    bool compare_zero = true;
    if (code == OPCODE::JMZ || code == OPCODE::JMN)
    {
        switch (CTRL.TYPE.mod)
        {
        case MOD_TYPE::DOUBLE: compare_zero  = _DEST.B->val == 0;
        case MOD_TYPE::SINGLE: compare_zero &= _DEST.A->val == 0; break;
        }
    }
    /* code */
    switch (code)
    {
    case OPCODE::JMP: break; // break to set jump value
    case OPCODE::JMZ: if ( compare_zero) break;   else return;
    case OPCODE::JMN: if (!compare_zero) break;   else return;
    case OPCODE::DJN:
        switch (CTRL.TYPE.mod)
        {
        case MOD_TYPE::DOUBLE:
            --(_DEST.B->val); /* pre-dec */
            compare_zero  = _DEST.B->val == 0;

        case MOD_TYPE::SINGLE:
            --(_DEST.A->val);
            compare_zero |= _DEST.A->val == 0;

            if(compare_zero) return; // all compared values are zero, skip jump
        } break;

    case OPCODE::SPL:
        scheduler.addProcess(PRCS.getParentID(), CTRL.SRC.index);
        return;
    }
    jump_val = _SRC.index; // only reached by break statement
}

} // namespace OS
