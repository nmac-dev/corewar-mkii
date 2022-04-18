/// Pushes fetch/decode/execute cycles to MARS and selects warrior processes from the scheduler
#pragma once

#define CORE_DEBUG

#include "mars.hpp"
#include "scheduler.hpp"

/// Operating System handles: fetch/decode/execute cycle, memory simulator, and warrior processes
namespace OS {

/// Handles the fetch/decode/execute cycle, memory array of assembly instructions, and warrior processes
class Core
{
 private:
    MARS      simulator;    // memory array simulator
    Scheduler scheduler;    // manages warrior processes

 public:
    struct Report
    {
        MARS::CommitLog    commits_log; // log of commits to be made on the simulator
        Scheduler::PrcsLog process_log; // log from scheduler of current process details

        /// Copies both logs into the report
        /// @param _commit_log  commit  log from MARS
        /// @param _process_log process log from Scheduler
        Report(MARS::CommitLog _commits_log, Scheduler::PrcsLog _process_log);
        Report();
    };

 /* Functions */
    /// Creates a core to fetch/decode/execute and manage a memory array simulator
    Core(ASM::WarriorList &warriors, int size, int seperation, int n_rounds, int n_cycles);

    /// Returns the size of the memory array
    inline int size()      const { return simulator.size(); }
    /// Returns the number of processes
    inline int processes() const { return scheduler.processes();  }

    /// Commits the next fetch/decode/execute cycle, then returns an operating system report
    Report nextFDECycle();
    
 /* Execute */
 private:
   /// Pushes a (MOV) commit to the core
   /// @param _cl commit log to push  
   void pushReadWrite(MARS::CommitLog &push_cl);
   /// Pushes a (CMP, SLT, SPL) commit to the core
   /// @param _cl commit log to push  
   void pushCompare(MARS::CommitLog &push_cl);
   /// Pushes a (ADD, SUB, MUL, DIV, MOD) commit to the core
   /// @param _cl commit log to push  
   void pushArithmetic(MARS::CommitLog &push_cl);
   /// Pushes a (JMP, JMZ, JMN, DJN) commit to the core
   /// @param _cl commit log to push  
   void pushJump(MARS::CommitLog &push_cl);
};

} //// namespace OS
