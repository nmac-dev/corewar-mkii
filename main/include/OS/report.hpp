/// Stores the details of the CPU's FDE cycle
#pragma once

#include "pcb.hpp"
#include "ctrl_unit.hpp"

namespace OS
{
/// Report containing details of the OS after each FDE cycle
struct Report
{
    /// Logs the address and event for an instruction in memory
    struct Log {
        int   address;  // location of instruction in memory
        Event event;    // instruction event
    };

    UUID program_id;    // executing program UUID
    Status status;      // m_status of the execution

    int next_pc;
    Log exe,            // executing instruction    (EXE)
        src,            // EXE: source instruction
        dest;           // EXE: destination instruction

    /// Generates a report from the OS detailing its last FDE cycle
    /// @param _process executing process
    /// @param _ctrl control unit containing registers (exe, src, and dest
    Report(PCB &_process, ControlUnit &_ctrl);
    Report();
};

} /* ::OS */
