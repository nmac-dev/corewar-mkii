// Handles warrior processes in a round robin system, ensures one process each per cycle
#pragma once

// #define SCHEDULER_DEBUG
// #define SCHEDULER_DEBUG_ADD_PCB
// #define SCHEDULER_DEBUG_KILL_PCB

#ifdef SCHEDULER_DEBUG
#define SCHEDULER_DEBUG_ADD_PCB
#define SCHEDULER_DEBUG_KILL_PCB
#endif

#include <unordered_map>
#include "asm.hpp"
#include "pcb.hpp"
#include "template/queue.hpp"
#include "template/round_robin.hpp"

/// Operating System handles: fetch/decode/execute cycle, memory simulator, and warrior processes
namespace OS {

namespace
{
using PrcsQueue = Queue<PCB>;                           // Queue of scheduled processes
using Schedules = std::unordered_map<int, PrcsQueue>;   // Hashtable of schedules
}

/// Manages processes using a queue of PCBs for each warrior
class Scheduler
{
 private:
    int max_cycles,             // max number of cycles before the round has been concluded
        max_processes;          // max number of processes a single warrior can create
    int cycles_counter;         // cycles executed

    Schedules schedules_tbl;    // hosts a queue of processes for each warrior
    RR_T<int> RR;               // A Round Robin System which manages its position rotation
    
 public:
    /// Create a process scheduler using a PCB queue for each warrior
    /// @param warriors collection of all the warriors
    /// @param _cycles max number of cycles before the round has been concluded
    /// @param _processes max number of processes a single warrior can create
    Scheduler(ASM::WarriorList *warriors,int _cycles, int _processes);
    Scheduler();

 /* Functions */
    /// Creates a new PCB for a process and sets the program counter to the input core index
    /// @param parent_ID owner of the process
    /// @param pc_initial program counter's initial index, where the first instruction is read
    void addProcess(int parent_ID, int pc_initial);

    /// Kills the last process added to the warriors pcb queue
    /// @param _prcs process to kill
    void killBackProcess(PCB &_prcs);

    /// Adjusts the program counter (relative) of the previous process in the round robin 
    /// @param _prcs process used to validate adjustment
    /// @param value added onto the program counter
    void pushJump(PCB const &_prcs, int value);

    /// Returns a report of the current process in the round robin sequance, then move to the next RR
    /// @param opcode evaluates for DAT opcode
    PCB nextProcess();

    /// Returns the current cycle number
    inline int cycles() const { return cycles_counter; }

    /// Returns the total number of warriors
    inline int totalWarriors()  const { return schedules_tbl.size(); }
    /// Returns the number of a warrior's pcbs
    /// @param _UUID warrior's UUID
    inline int warriorPCBs(int _UUID)
    { 
        int pcbs = 0;
        if (schedules_tbl.count(_UUID))
        {
            pcbs = schedules_tbl[_UUID].size();
        }
        return pcbs;
    }
    /// Returns the total number of processes
    inline int totalPCBs()
    { 
        int start_id = RR.i();
        int total_ = 0;
        for (int i = 0; i < RR.len(); i++)
        {
            int UUID = RR.i();
            if(schedules_tbl.count(UUID))
            {
                total_ += schedules_tbl[UUID].size();
            }
        }
        // ensure RR position is retained
        while (RR.i() != start_id) RR.next();

        return total_;
    }
};

} // namespace OS
