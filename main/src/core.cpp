/// Pushes fetch/decode/execute cycles to MARS and selects warrior processes from the scheduler

#include "core.hpp"

/// Operating System handles: fetch/decode/execute cycle, memory simulator, and warrior processes
namespace OS {

// Report::Report(ControlUnit _CTRL, PCB _process)
// {
//     CTRL    = _CTRL;
//     process = _process;
// }
// Report::Report() = default;

Core::Core(ASM::WarriorList *_warriors, int _seperation, int _cycles, int _processes) 
{
    mars      = MARS(_warriors, _seperation);    // must be first (warrior needs core_index)
    scheduler = Scheduler(_warriors, _cycles, _processes);
}

int Core::nextFDECycle()
{
    /* Fetch */
    int IR_PC;                           // instruction register program counter
    PRCS_EXE = scheduler.nextProcess();  // fetch next process

    // victory
    if (PRCS_EXE.getStatus() == Status::EXIT)    return 1;
    // draw
    if (PRCS_EXE.getStatus() == Status::HAULTED) return -1;

    /* Decode */
    PRCS_EXE >> IR_PC;
    CTRL     = mars.generateCTRL(IR_PC);  // generate a control unit

    #ifdef CORE_DEBUG
    static int cycle;
    printf("\nCore::nextFDECycle:\t Cycle:[%d]\t Warrior:[%d] Index:[%d] Inst:'%s' \n",
        cycle++, PRCS_EXE.getParentID(), CTRL.EXE.index, CTRL.EXE.inst->toAsmCode().c_str());
    #endif

    /* Execute */
    int jump_val = CTRL.EXE.index +1; // value to apply to program counter (next instruction) 
    switch (CTRL.TYPE.code)
    {
    case ASM::OPCODE_TYPE::READWRITE:
        execute_ReadWrite();
        break;
    case ASM::OPCODE_TYPE::COMPARISION:
        execute_Compare(jump_val);
        break;
    case ASM::OPCODE_TYPE::ARITHMETIC:
        execute_Arithmetic();
        break;
    case ASM::OPCODE_TYPE::JUMP:
        execute_Jump(jump_val);
        break;
    }
    // check if killed
    if (PRCS_EXE.getStatus() != Status::TERMINATED)
    {
        scheduler.pushJump(PRCS_EXE, jump_val);
    }
    
    applyPostIncrement(CTRL.post_A);  // apply post-increment
    applyPostIncrement(CTRL.post_B);

    return 0;
}

void Core::execute_ReadWrite()
{
    ControlUnit::Register &_SRC  = CTRL.SRC,
                          &_DEST = CTRL.DEST;
    switch (CTRL.EXE.inst->OP.code)
    {
    case ASM::OPCODE::DAT:

        scheduler.killBackProcess(PRCS_EXE);
        CTRL.SRC.event  = Event::NOOP;
        CTRL.DEST.event = Event::NOOP;
        break;
        
    case ASM::OPCODE::MOV:

        switch (CTRL.TYPE.mod)
        {
        case ASM::MOD_TYPE::DOUBLE: *_DEST.B.ptr = _SRC.B.val; // doubles bleed into singles
        case ASM::MOD_TYPE::SINGLE: *_DEST.A.ptr = _SRC.A.val;        break;
        case ASM::MOD_TYPE::FULL:   *CTRL.DEST.inst = *CTRL.SRC.inst; break;
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
    case ASM::MOD_TYPE::DOUBLE: compare_eq  = _SRC.B.val - _DEST.B.val == 0;
    case ASM::MOD_TYPE::SINGLE: compare_eq &= _SRC.A.val - _DEST.A.val == 0; break;

    case ASM::MOD_TYPE::FULL:
        ASM::Inst &src_i  = *CTRL.SRC.inst,
                  &dest_i = *CTRL.DEST.inst;
        // entire instruction comparison
        compare_eq =    src_i.OP.code == dest_i.OP.code  // opcode
                     && src_i.OP.mod  == dest_i.OP.mod   // modifier
                     && src_i.A.admo  == dest_i.A.admo   // A admo
                     && src_i.B.admo  == dest_i.B.admo   // B admo
                     && src_i.A.val   == dest_i.A.val    // A val
                     && src_i.B.val   == dest_i.B.val;   // A val
    }

    switch (CTRL.EXE.inst->OP.code)
    {
    case ASM::OPCODE::SEQ: if(compare_eq)          break;   else return; // return skips jump++
    case ASM::OPCODE::SNE: if(compare_eq == false) break;   else return; 

    case ASM::OPCODE::SLT:
        switch (CTRL.TYPE.mod)
        {
        case ASM::MOD_TYPE::DOUBLE: if (_SRC.B.val < _DEST.B.val) /* bleed */; else return;
        case ASM::MOD_TYPE::SINGLE: if (_SRC.A.val < _DEST.A.val) break;       else return;
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
    switch (CTRL.EXE.inst->OP.code)
    {
    case ASM::OPCODE::ADD: operator_char = '+'; break;
    case ASM::OPCODE::SUB: operator_char = '-'; break;
    case ASM::OPCODE::MUL: operator_char = '*'; break;
    case ASM::OPCODE::DIV: operator_char = '/'; break;
    case ASM::OPCODE::MOD: operator_char = '%'; break;
    }
    
    // check for division by zero
    if (operator_char == '/' || operator_char == '%')
    {
        bool zero_div = false; // true if division by zero
        switch (CTRL.TYPE.mod)
        {
        case ASM::MOD_TYPE::DOUBLE: zero_div = !(_SRC.B.val && _DEST.B.val); // NOT 0
        case ASM::MOD_TYPE::SINGLE: zero_div = !zero_div && !(_SRC.A.val && _DEST.A.val); break;
        }

        if (zero_div) // kill process
        {
            scheduler.killBackProcess(PRCS_EXE);
            CTRL.SRC.event = Event::NOOP;
            CTRL.SRC.event = Event::NOOP;
            return;
        }
    }
    
    switch (CTRL.TYPE.mod)
    {
    case ASM::MOD_TYPE::DOUBLE: // bleeds into single
        *_DEST.B.ptr = arithOpFilter(*_DEST.B.ptr, _SRC.B.val, operator_char);

    case ASM::MOD_TYPE::SINGLE: 
        *_DEST.A.ptr = arithOpFilter(*_DEST.A.ptr, _SRC.A.val, operator_char);
    }
}

void Core::execute_Jump(int &jump_val)
{
    ASM::OPCODE code = CTRL.EXE.inst->OP.code;
    ControlUnit::Register &_SRC  = CTRL.SRC,
                          &_DEST = CTRL.DEST;
    
    CTRL.SRC.event  = Event::READ;
    CTRL.DEST.event = (code == ASM::OPCODE::JMP) ? Event::NOOP 
                                                 : Event::READ; // JMZ, JMN, DJN

    // comparison boolean: true if target is zero
    bool compare_zero = true;
    if (code == ASM::OPCODE::JMZ || code == ASM::OPCODE::JMN)
    {
        switch (CTRL.TYPE.mod)
        {
        case ASM::MOD_TYPE::DOUBLE: compare_zero  = _DEST.B.val == 0;
        case ASM::MOD_TYPE::SINGLE: compare_zero &= _DEST.A.val == 0; break;
        }
    }
    /* code */
    switch (code)
    {
    case ASM::OPCODE::JMP: break; // break to set jump value
    case ASM::OPCODE::JMZ: if ( compare_zero) break;   else return;
    case ASM::OPCODE::JMN: if (!compare_zero) break;   else return;
    case ASM::OPCODE::DJN:
        switch (CTRL.TYPE.mod)
        {
        case ASM::MOD_TYPE::DOUBLE:
            --(*_DEST.B.ptr); /* pre-dec */ 
            compare_zero  = _DEST.B.val == 0;

        case ASM::MOD_TYPE::SINGLE:
            --(*_DEST.A.ptr);
            compare_zero |= _DEST.A.val == 0;

            if(compare_zero) return; // all compared values are zero, skip jump
        } break;
    
    case ASM::OPCODE::SPL:
        scheduler.addProcess(PRCS_EXE.getParentID(), CTRL.SRC.index); 
        return;
    }
    jump_val = _SRC.index; // only reached by break statement
}

} // namespace OS
