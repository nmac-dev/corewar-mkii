/// Pushes fetch/decode/execute cycles to MARS and selects warrior processes from the scheduler

#include "core.hpp"

/// Operating System handles: fetch/decode/execute cycle, memory simulator, and warrior processes
namespace OS {

Core::Report::Report(MARS::CommitLog _commits_log, Scheduler::PrcsLog _process_log)
{
    commits_log = _commits_log;
    process_log = _process_log;
}
Core::Report::Report(){}

Core::Core(ASM::WarriorList &warriors, int _size, int _seperation, int _rounds, int _cycles) 
{
    simulator = MARS(warriors, _size, _seperation);     // must be first (warrior needs core_index)
    scheduler = Scheduler(warriors, _rounds, _cycles);
}

Core::Report Core::nextFDECycle()
{
    MARS::CommitLog    commit_log;              // commits MARS wants to push
    Scheduler::PrcsLog process_log;             // process executing the instruction

    /* Fetch */
    const int IR_PC = scheduler.peekPC();       // peek the next process program counter
    ASM::Inst i_register = simulator[IR_PC];    // fetch instruction    

    /* Decode */
    process_log = scheduler.nextRR(i_register.opcode);  // decode next process,     get log
    commit_log  = simulator.nextCommit(IR_PC);          // decode next instruction, get log

    /* Execute */
    switch (commit_log.commit_type)
    {
    case ASM::_OP_TYPE::READWRITE:
        pushReadWrite(commit_log);
        break;
    /// TDOD:
    // case ASM::_OP_TYPE::COMPARE:
    //     pushCompare(commit_log);
    //     break;
    // case ASM::_OP_TYPE::ARITHMETIC:
    //     pushArithmetic(commit_log);
    //     break;
    // case ASM::_OP_TYPE::JUMP:
    //     pushJump(commit_log);
    //     break;
    }

    return Core::Report(commit_log, process_log);
}

void Core::pushReadWrite(MARS::CommitLog &push_cl)
{
    switch (simulator[push_cl.ir_pc].opcode)
    {
    case ASM::_OP::DAT:
        // already applied to process
        break;
    case ASM::_OP::MOV:
        switch (push_cl.mod_type)
        {
        case ASM::_MOD_TYPE::SINGLE:
            push_cl.dest_A = push_cl.src_A;
            break;
        case ASM::_MOD_TYPE::DOUBLE:
            push_cl.dest_A = push_cl.src_A;
            push_cl.dest_B = push_cl.src_B;
            break;
        case ASM::_MOD_TYPE::FULL:
            simulator[push_cl.adr_dest] = simulator[push_cl.adr_src];
            break;
        }
        break;
    }
}

} //// namespace OS
