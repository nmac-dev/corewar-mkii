/// Represents a single process within a process control block

#include "pcb.hpp"

/// Operating System handles: fetch/decode/execute cycle, memory simulator, and warrior processes
namespace OS {

PCB::PCB(int pc_initial) 
{
    p_id            = createPID();
    status          = Status::ACTIVE;
    program_counter = pc_initial;
}
PCB::PCB(){}

void PCB::operator<<(int r_value)        { this->program_counter = r_value; }
void PCB::operator>>(int &r_value) const { r_value = this->program_counter; }
void PCB::operator++()                   { program_counter++; }
void PCB::operator--()                   { program_counter--; }

} //// namespace OS
