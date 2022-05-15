// Handles warrior processes in a round robin system, ensures one process each per cycle
#pragma once

//  #define SCHEDULER_DEBUG
//  #define SCHEDULER_DEBUG_ADD_PCB
//  #define SCHEDULER_DEBUG_KILL_PCB

#include <unordered_map>
#include "assembly.hpp"
#include "pcb.hpp"
#include "template/queue.hpp"
#include "template/round_robin.hpp"

/// Operating System handles: fetch/decode/execute cycle, memory simulator, and warrior processes
namespace OS {

namespace /* {anonymous} */
{
    using PrcsQueue = Queue<PCB>;                           // Queue of scheduled processes
    using Schedules = std::unordered_map<UUID, PrcsQueue>;  // Hashtable of schedules
}

/// Manages processes using a queue of PCBs for each warrior
class Scheduler
{
 private:
    int ini_max_cycles,         // max number of cycles before the round has been concluded
        ini_max_processes;      // max number of processes a single warrior can create
    int m_cycles;               // cycles executed

    Schedules schedules_tbl;    // hosts a queue of processes for each warrior
    RoundRobin<UUID> RR;        // A Round Robin System which manages its position rotation

 public:
    /// Create a process scheduler using a PCB queue for each warrior
    /// @param _warriors collection of all the warriors
    /// @param _cycles max number of cycles before the round has been concluded
    /// @param _processes max number of processes a single warrior can create
    Scheduler(ASM::WarriorVec *_warriors,int _max_cycles, int _max_processes);
    Scheduler();

 /* Fetch */

    /// Creates a new process and sets the initial program counter
    /// @param _parent parent of the process
    /// @param _pc_initial program counter's initial address, where the first instruction is read
    void add_process(UUID _parent, int _pc_initial);

    /// Kills the process
    /// @param _process process to kill
    void kill_process(PCB *_process);

    /// Returns the next process from the queue
    PCB fetch_next();

    /// Returns the process to the queue
    /// @param _process existing process to return
    void return_process(PCB *_process);

 /* Utility */

    /// Returns max number of cycles before the round has been concluded
    inline int const max_cycles() const { return ini_max_cycles; }

    /// Returns max number of processes a single parent can create
    inline int const max_processes() const { return ini_max_processes; }

    /// Returns cycles executed
    inline int const cycles() const { return m_cycles; }

    /// Returns number of active queues
    inline int const active() { return RR.len(); }

    /// Returns the total number of processes
    inline int const processes() 
    {  
        int total_ = 0;
        
        for (int i = 0; i < active(); i++)
        {
            total_ += schedules_tbl[RR.next()].size();
        }
        return total_;
    }

    /// Returns the number of processes for a specific UUID
    /// @param _uuid parents's ID
    inline int const processes(UUID _uuid)
    { 
        int processes_ = 0;
        if (schedules_tbl.count(_uuid))
        {
            processes_ = schedules_tbl[_uuid].size();
        }
        return processes_;
    }
};/* Scheduler */

} /* ::OS */
