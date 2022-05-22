/// Executes fetch/decode/execute cycles to Memory and selects program processes from the scheduler
#pragma once

// #define CPU_DEBUG
// #define CPU_DEBUG_CODES

#include "assembly.hpp"
#include "memory.hpp"
#include "scheduler.hpp"
#include "report.hpp"

/// Operating System handles: fetch/decode/execute cycle, memory simulator, and program processes
namespace OS 
{
/// Handles the fetch/decode/execute cycle, memory array of assembly instructions, and program processes
class CPU
{
 private:
    Memory    *os_memory;       // memory array simulator
    Scheduler *os_sched;        // process scheduler (sched) for programs

    ControlUnit ctrl;           // Control Unit from Memory, used in executiom
    PCB         exe_process;    // process executing the instruction

 public:
    /// Creates a core to fetch/decode/execute and manage a memory array simulator
    CPU(Memory *_memory, Scheduler *_sched);
    CPU();

 /* Execute */

    /// run the next fetch/decode/execute cycle, then returns an operating system report
    Report const run_fde_cycle();

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
}; /* CPU */

} /* ::OS */
