/// Represents a single process within a process control block
#pragma once

#include "uuid.hpp"

/// Operating System handles: fetch/decode/execute cycle, memory simulator, and warrior processes
namespace OS {

/// Denotes the status of execution
enum class Status { NEW, ACTIVE, TERMINATED, HAULTED, EXIT };

/// Process Control Block: represents a process with a parent ID, status, and program counter
class PCB
{
 private:
    UUID    m_parent_id;      // UUID of the process's parent
    int     m_pc;             // program counter: holds address of next instruction to be executed
    Status  m_status;         // current execution status

 public:
    /// Creates a process
    /// @param _parent_ID UUID of the process's parent
    /// @param _pc_initial initial program counter address 
    PCB(UUID _parent_ID, int _pc_initial);
    PCB();

 /* Utility */

    inline UUID   parent_id()          const { return m_parent_id; }
    inline Status status()             const { return m_status;    }
    inline void   set_status(Status _status) { m_status = _status; }

    /// [setter] Sets the program counters address to the r_value
    /// @param _set_pc PCBs updated program counter address
    void operator<<(int _set_pc);
    /// [getter] Places the program counter address into the r_value
    /// @param get_pc_ Int to store the PCBs program counter address
    void operator>>(int &get_pc_) const;
};  /* PCB */

} /* ::OS */
