// Handles program processes in a round robin system, ensures one process each per cycle
#pragma once

//  #define SCHEDULER_DEBUG
//  #define SCHEDULER_DEBUG_ADD_PCB
//  #define SCHEDULER_DEBUG_KILL_PCB

#include <unordered_map>
#include "assembly.hpp"
#include "pcb.hpp"
#include "template/queue.hpp"
#include "template/round_robin.hpp"

/// Operating System handles: fetch/decode/execute cycle, memory simulator, and program processes
namespace OS {

namespace /* {anonymous} */
{
    using PrcsQueue = Queue<PCB>;                           // Queue of scheduled processes
    using Schedules = std::unordered_map<UUID, PrcsQueue>;  // Hashtable of schedules
}

/// Manages processes using a queue of PCBs for each program
class Scheduler
{
 private:
    int ini_max_cycles,         // max number of cycles before the round has been concluded
        ini_max_processes;      // max number of processes a single program can create
    int m_cycles;               // cycles executed
    int mutable m_total_prcs;   // total running processes

    Schedules schedules_tbl;    // hosts a queue of processes for each program
    RoundRobin<UUID> RR;        // A Round Robin System which manages its position rotation

 public:
    /// Create a process scheduler using a PCB queue for each program
    /// @param _programs collection of all the programs
    /// @param _cycles max number of cycles before the round has been concluded
    /// @param _processes max number of processes a single program can create
    Scheduler(Asm::ProgramVec *_programs, int _max_cycles, int _max_processes);
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
    inline int const &max_cycles() const { return ini_max_cycles; }

    /// Returns max number of processes a single parent can create
    inline int const &max_processes() const { return ini_max_processes; }

    /// Returns cycles executed
    inline int const &cycles()   const { return m_cycles; }

    /// Returns number of programs program
    inline int const &programs() const { return RR.len(); }

    /// Returns the total number of processes
    inline int const &processes() const
    {
        m_total_prcs = 0;
        for (int i = 0; i < programs(); i++)
        {
            m_total_prcs += schedules_tbl.at(RR.next()).size();
        }
        return m_total_prcs;
    }

    /// Returns the number of processes for a specific UUID
    /// @param _uuid parents's ID
    inline int const processes(UUID _uuid) const
    { 
        int processes_ = 0;
        if (schedules_tbl.count(_uuid))
        {
            processes_ = schedules_tbl.at(_uuid).size();
        }
        return processes_;
    }
};/* Scheduler */

} /* ::OS */
