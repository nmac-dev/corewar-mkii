//// Handles warrior processes in a round robin system, ensures one process each per cycle
#pragma once

// #define SCHEDULER_DEBUG

#include <unordered_map>
#include "p_queue.hpp"
#include "pcb.hpp"

/// Operating System handles: fetch/decode/execute cycle, memory simulator, and warrior processes
namespace OS {

namespace 
{
using PQ_T      = P_Queue<ASM::Warrior *, PCB>;         // PCB queue with warrior parent
using PrcsQueue = std::unique_ptr<PQ_T>;                // Queue of scheduled processes
using Schedules = std::unordered_map<int, PrcsQueue>;   // Hashtable of schedules
}

/// A Round Robin System which manages its position rotation 
class RRSystem  
{
 private:
    int pos;                // position of current round robin sequance
    int length;             // length of iterations before rotating to 0
    std::vector<int> uuids; // stores all active warrior's UUIDs
 public:

    /// Create a round robin system using warrior UUIDs
    /// @param _length of round robin system (before rotating to 0)
    RRSystem(ASM::WarriorList &warriors, int _length);
    RRSystem();

    /// Returns the current position
    inline int index() const { return pos; }
    /// Return the length of the round robin
    inline int len() const { return length; }
    /// Return the UUID at the current round robin position
    inline int UUID() const { return uuids[pos]; }

    /// Move to next round robin position
    inline void next() { if (length < ++pos) pos = 0; }
    /// Remove the input UUID from the round robin system and adjust the length
    inline void remove(int _uuid) 
    {
        for (int i = 0; i < length; i++)
            if (uuids[i] == _uuid)
            {
                while (i < length -1)   // remove UUID (shift remaining)
                {
                    uuids[i] = uuids[i +1];
                    i++; 
                }
                uuids.pop_back();
                length--; // trim sequance
            }
    }
};

/// Manages processes using a queue of PCBs for each warrior
class Scheduler
{
 private:
    int max_rounds,          // max number of rounds before the game is concluded
        max_cycles;          // max number of cycles before the round has been concluded
    int t_pcbs;              // total number of pcbs

    Schedules schedules_tbl; // hosts a queue of processes for each warrior
    RRSystem RR;
    
 public:
    /// Process log containing information on the current executing process
    struct PrcsLog
    {
        ASM::Warrior *warrior;  // warrior (parent) of the process
        PCB    process;         // current process
        int    n_pcbs;          // number of processes owned
        Status status;          // current process status
        bool   has_won;         // true if warrior has won the game

        /// Populates report with a warrior, its number of processes, and default values
        PrcsLog(ASM::Warrior *_warrior, PCB process, int _n_pcbs);
        PrcsLog();
    };

 /* Functions */
    /// Create a process scheduler using a PCB queue for each warrior
    /// @param warriors collection of all the warriors
    /// @param _rounds max number of rounds before the game is concluded
    /// @param _cycles max number of cycles before the round has been concluded
    Scheduler(ASM::WarriorList &warriors, int _rounds, int _cycles);
    Scheduler();

    /// Creates a new PCB for a process and sets the program counter to the input core index
    /// @param parent owner of the process
    /// @param pc_initial program counter's initial index, where the first instruction is read
    void addProcess(ASM::Warrior *parent, int pc_initial);
    
    /// Returns the program counter for the process at the front of the schedule
    int peekPC();

    /// Returns a report of the current process in the round robin sequance, then move to the next RR
    /// @param opcode evaluates for DAT opcode
    PrcsLog nextRR(ASM::_OP opcode);

    /// Returns the total number of processes
    inline int processes() const { return t_pcbs; }
};

} //// namespace OS
