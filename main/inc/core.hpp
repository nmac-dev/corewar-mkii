/// Executes fetch/decode/execute cycles to MARS and selects warrior processes from the scheduler
#pragma once

// #define CORE_DEBUG
// #define CORE_DEBUG_CODES

#include "mars.hpp"
#include "scheduler.hpp"

/// Operating System handles: fetch/decode/execute cycle, memory simulator, and warrior processes
namespace OS {

/// Report returned from the OS after each FDE cycle
struct Report
{
    /// Logs the address and event for an instruction in memory
    struct Log {
        int   address; // location of instruction in memory
        Event event; // instruction event
    };

    int warrior_ID,     // executing warrior UUID
        next_pc;        // next program counter to be executed
    Status status;      // m_status of the execution

    Log exe,            // executing instruction    (EXE)
        src,            // EXE: source instruction
        dest;           // EXE: destination instruction

    /// Generates a report from the OS detailing its last FDE cycle
    /// @param _PROCESS executing process with parent ID
    /// @param _CTRL control block containing registers for the executing, source, and destination
    Report(PCB &_PROCESS, ControlUnit &_CTRL);
    Report();
};

/// Handles the fetch/decode/execute cycle, memory array of assembly instructions, and warrior processes
class Core
{
 private:
    MARS      *os_memory;  // memory array simulator
    Scheduler *os_sched;   // process scheduler (sched) for warriors

    ControlUnit ctrl;       // Control Unit from MARS, used in executiom
    PCB         exe_process;    // process executing the instruction

 public:
    /// Creates a core to fetch/decode/execute and manage a memory array simulator
    Core(MARS *_memory, Scheduler *_sched);
    Core();

 /* Functions */
    /// run the next fetch/decode/execute cycle, then returns an operating system report
    Report run_fde_cycle();
    
 /* Execute */
 private:
    /// Executes a (NOP, DAT, MOV)
    void execute_system();
    /// Executes a (CMP, SLT, SPL)
    void execute_compare();
    /// Executes a (ADD, SUB, MUL, DIV, MOD)
    void execute_arithmetic();
    /// Executes a (JMP, JMZ, JMN, DJN)
    void execute_jump();

    /// Applies an arithmetic operation to the left value
    /// @param operator_char char represnetation of the arithmetic operator
    /// @param l_val modifiable left value used as the base
    /// @param r_val right value to apply to l_val
    inline void apply_arithmatic(int &l_val, int r_val, char _operator)
    {
        switch (_operator)
        {
            case '+': l_val = l_val + r_val; break;
            case '-': l_val = l_val - r_val; break;
            case '*': l_val = l_val * r_val; break;
            case '/': l_val = l_val / r_val; break;
            case '%': l_val = l_val % r_val; break;
            default: break;
        }
    }
}; /* Core */

} /* ::OS */
