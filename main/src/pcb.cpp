/// Represents a single process within a process control block

#include "pcb.hpp"

/// Operating System handles: fetch/decode/execute cycle, memory simulator, and warrior processes
namespace OS {

PCB::PCB(int _parent_ID, int _pc_initial)
{
    m_parent_id = _parent_ID;
    m_pc        = _pc_initial;
    m_status    = Status::NEW;
}
PCB::PCB() = default;

void PCB::operator<<(int r_value)        { m_pc      = r_value; }
void PCB::operator>>(int &r_value) const { r_value = m_pc;      }

} /* ::OS */
