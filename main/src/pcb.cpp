/// Represents a single process within a process control block

#include "pcb.hpp"

/// Operating System handles: fetch/decode/execute cycle, memory simulator, and warrior processes
namespace OS {

PCB::PCB(int _parent_ID, int pc_initial)
{
    parent_ID = _parent_ID;
    pc        = pc_initial;
    pid       = createPID();
    status    = Status::ACTIVE;
}
PCB::PCB() = default;

void PCB::operator<<(int r_value)        { pc      = r_value; }
void PCB::operator>>(int &r_value) const { r_value = pc;      }

} // namespace OS
