/// Represents a single process within a process control block
#pragma once

#include "asm.hpp"

/// Operating System handles: fetch/decode/execute cycle, memory simulator, and warrior processes
namespace OS {

/// Denotes the status of execution for a pcb
enum class Status { ACTIVE, TERMINATED, HAULTED, EXIT };

/// Process Control Block: represents a process with an unique ID, status, and program counter
class PCB
{
 private:
    int     parent_ID;      // UUID of the process's parent
    int     pc;             // program counter: holds address of next instruction to be executed
    int     pid;            // unique identifier for the process
    Status  status;         // current execution status

    /// Creates a unique process identifier (starts from 1)
    inline static int createPID()
    {
        static int unique_pid;
        return   ++unique_pid;
    }

 /* Functions */
 public:
    /// Creates a process
    /// @param _parent_ID UUID of the process's parent
    /// @param pc_initial initial program counter index 
    PCB(int _parent_ID, int pc_initial);
    PCB();
 
 /* Getters */
    inline int    getParentID()       const { return parent_ID; }
    inline int    getPID()            const { return pid;       }
    inline Status getStatus()         const { return status;    }
    inline void   setStatus(Status _status) { status = _status; }

    /// [setter] Sets the program counters index to the r_value
    /// @param _set_pc PCBs updated program counter index
    void operator<<(int _set_pc);
    /// [getter] Places the program counter index into the r_value
    /// @param get_pc_ Int to store the PCBs program counter index
    void operator>>(int &get_pc_) const;
};

} // namespace OS
