/// Stores the details of the CPU's FDE cycle

#include "OS/report.hpp"

namespace OS
{
Report::Report(PCB& _process, ControlUnit& _ctrl)
{
    program_id = _process.parent_id();
    _process   >> next_pc;
    status     = _process.status();

    exe  = { _ctrl.EXE.address,  _ctrl.EXE.event  };
    src  = { _ctrl.SRC.address,  _ctrl.SRC.event  };
    dest = { _ctrl.DEST.address, _ctrl.DEST.event };
}
Report::Report() = default;

} /* ::OS */
