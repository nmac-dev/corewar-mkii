/// Represents a single process within a process control block
#pragma once

#include "asm.hpp"

/// Operating System handles: fetch/decode/execute cycle, memory simulator, and warrior processes
namespace OS {

/// Denotes the status of execution for a pcb
enum class Status
{
    ACTIVE,
    TERMINATED,
};

/// Process Control Block: represents a process with an unique ID, status, and program counter
class PCB
{
 private:
    int     p_id;               // unique ID: identifier for the process
    Status  status;             // current status
    int     program_counter;    // holds address of next instruction to be executed

    /// Creates a unique process identifier (starts from 1)
    inline static int createPID()
    {
        static int unique_pid;
        return   ++unique_pid;
    }

 /* Functions */
 public:
    /// Creates a process
    /// @param pc_initial initial program counter index 
    PCB(int pc_initial);
    PCB();
 
 /* Getters */
    inline int    getPID()    const { return p_id; }
    inline Status getStatus() const { return status; }

    /// [setter] Sets the program counters index to the r_value
    /// @param r_value PCBs updated program counter index
    void operator<<(int r_value);
    /// [getter] Places the program counter index into the r_value
    /// @param r_value Int to store the PCBs program counter index
    void operator>>(int &r_value) const;
    /// Increments PCB's program counter 
    void operator++();
    /// Decrements PCB's program counter 
    void operator--();
};

} //// namespace OS
