//// Handles warrior processes in a round robin system, ensure one process each per cycle
#pragma once

#include <vector>
#include <queue>

#include "asm.hpp"

/// Warrior Operating System (WOS) hosts process scheduler functionality
namespace WOS {

struct PCB; // forward decleration

using WarriorPCBs = std::vector<std::queue<PCB>>;

/// Denotes the state of execution for a warriors process
enum class State {
    NEW,
    READY,
    RUNNING,
    WAITING,
    TERMINATED,
};

/// Process belonging to a Warrior
class Process
{
 private:
    ASM::Warrior *parent;
 public:
    Process(ASM::Warrior *parent);
};

/// Process Control Block
class PCB
{
 private:
    int     p_id;               // process ID: identifier for the PCB and process
    Process *process;           // process created by a warrior
    State   state;              // process state
    int     program_counter;    // holds address of next instruction to be executed

    /// Creates a unique process identifier (starts from 1)
    inline static int createPID()
    {
        static int unique_pid;
        return   ++unique_pid;
    }

 public:
    /// Creates a process control block for a warrior process
    /// @param new_process [process associated with the PCB 
    /// @param pc_initial   initial program counter position within the core
    PCB(Process *new_process, int pc_initial);

    /// Returns the process identifier
    inline int getPID() const { return p_id; } 
    /// Returns the processes parent UUID
    inline int getParentUUID() const;    
};


/// Manages processes using a queue of PCBs for each warrior
class Scheduler
{
 private:
    int n_warriors;  // number of warriors scheduled
    WarriorPCBs warrior_schedule; // Hosts a queue of PCBs for each warrior

 public:
    /// Create a process schedule using a PCB queue for each warrior
    /// @param warriors_list list of all the warriors loaded in the core
    Scheduler(std::vector<ASM::Warrior> &warriors_list);
    ~Scheduler();

    /// Creates a new PCB for a process and sets the program counter to the input core index
    /// @param parent owner of the process
    /// @param pc_index program counter index from which the first instruction is read
    void addProcess(ASM::Warrior &parent, int pc_index);
    /// Runs the next cycle of the round robin sequance where each queue head executes the processes 
    /// stored instruction, then adjusts the program counter, and finally removes terminated processes
    void executeCycle();
};

} /// namespace WOS
