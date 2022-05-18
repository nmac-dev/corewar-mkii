#pragma once

/// Operating System handles: fetch/decode/execute cycle, memory simulator, and program processes
namespace OS {

/// Operating System: execution status
enum class Status : int
{ 
    NEW,        // creating new process
    ACTIVE,     // running process
    TERMINATED, // killing process
    HAULTED,    // haulted all execution (programs did NOT finish)
    EXIT        // execution complete, all programs have finished
};

/// Operating System: operation event
enum class Event : int
{ 
    NOOP,       // no operation occurred
    READ,       // read data from address
    WRITE,      // wrote data to address
    EXECUTE,    // executed program counter instruction
    ILLEGAL     // illegal operation from executing instruction
};

} /* ::OS */
