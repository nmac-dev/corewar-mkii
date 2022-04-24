/// Executes fetch/decode/execute cycles to MARS and selects warrior processes from the scheduler
#pragma once

//#define CORE_DEBUG

#include "mars.hpp"
#include "scheduler.hpp"

/// Operating System handles: fetch/decode/execute cycle, memory simulator, and warrior processes
namespace OS {

/// Report returned from the OS after each FDE cycle
struct Report
{
    /// Logs the index and event for an instruction in memory
    struct Log {
        int   index; // location of instruction in memory
        Event event; // instruction event
        Log(ControlUnit::Register &_reg);
        Log();
    };

    int warrior_ID;     // executing warrior UUID
    Status prcs_status; // status of the executing process
    int pcbs;           // number of pcbs for the current warrior
    int cycles;         // number of cycles executed

    Log exe,            // executing instruction
        src,            // exe: source instruction
        dest;           // exe: destination instruction

    /// Generates a report from the OS detailing its last FDE cycle
    /// @param _prcs executing process with parent ID
    /// @param _CTRL control block containing registers for the executing, source, and destination
    /// @param _pcb  number of pcbs for the current warrior
    Report(PCB &_prcs, ControlUnit &_CTRL, int _pcbs, int _cycles);
    Report();
};

/// Handles the fetch/decode/execute cycle, memory array of assembly instructions, and warrior processes
class Core
{
 private:
    MARS      mars;        // memory array simulator
    Scheduler scheduler;   // manages warrior processes

    ControlUnit CTRL;      // Control Unit from MARS, used in executiom
    PCB         PRCS;      // process executing the instruction

 public:
    /// Creates a core to fetch/decode/execute and manage a memory array simulator
    Core(ASM::WarriorList *warriors, int _seperation, int _cycles, int _processes);

 /* Functions */
    /// Returns the size of the memory array
    inline int size() const { return mars.size(); }
    /// Returns the number of processes
    inline int processes()  { return scheduler.totalPCBs();  }

    /// run the next fetch/decode/execute cycle, then returns an operating system report
    Report nextFDECycle();
    
 /* Execute */
 private:
   /// Executes a (DAT, MOV)
   void execute_ReadWrite();
   /// Executes a (CMP, SLT, SPL)
   void execute_Compare(int &jump_val);
   /// Executes a (ADD, SUB, MUL, DIV, MOD)
   void execute_Arithmetic();
   /// Executes a (JMP, JMZ, JMN, DJN)
   void execute_Jump(int &jump_val);

   /// Arithmatic operation filter: applies the operator to both values and returns the result
   /// @param operator_char char represnetation of the operation to perform
   /// @param l_val left value  used as the base
   /// @param r_val right value to perform operation on
   inline int arithOpFilter(int l_val, int r_val, char operator_char)
   {
      int ret_ = 0;
      switch (operator_char)
      {
      case '+': ret_ = l_val + r_val; break;
      case '-': ret_ = l_val - r_val; break;
      case '*': ret_ = l_val * r_val; break;
      case '/': ret_ = l_val / r_val; break;
      case '%': ret_ = l_val % r_val; break;
      default: break;
      }
      return ret_;
   }

   /// Applies the post-increment
   /// @param _post pointer to be incremented
   inline void applyPostIncrement(int *_post)
   {
       if (_post != 0 && _post != nullptr && _post != NULL) (*_post)++;
   }
};

} // namespace OS
