/// Handles program processes in a round robin system, ensures one process each per cycle

#include "scheduler.hpp"

/// Operating System handles: fetch/decode/execute cycle, memory simulator, and program processes
namespace OS {

Scheduler::Scheduler(Asm::ProgramVec *_programs, int _max_cycles, int _max_processes)
{
    ini_max_cycles     = _max_cycles;
    ini_max_processes  = _max_processes;
    m_cycles           = 0;
    m_total_prcs       = 0;

    schedules_tbl.reserve(_programs->size());

    // create a queue for each program
    for (int i = 0; i < _programs->size(); i++)
    {
        UUID uuid_ = (*_programs)[i].get()->uuid();
        RR.push_back(uuid_);

        // create a  schedule for the program with an initial process
        schedules_tbl[uuid_] = PrcsQueue();
        this->add_process(uuid_, (*_programs)[i].get()->address());
    }

    #ifdef SCHEDULER_DEBUG
    printf("\nScheduler::Scheduler: initialised with |%d| processes \n", processes());
    #endif
}
Scheduler::Scheduler()  = default;

void Scheduler::add_process(UUID _parent, int _pc_initial)
{
    PCB process_(_parent, _pc_initial);
        process_.set_status(Status::NEW);

    if (schedules_tbl.count(_parent))
    {
        if (processes(_parent) < max_processes())
        {
            schedules_tbl[_parent].enqueue(process_);
        }
    }
    else printf("ERROR: scheduler failed to add process... UUID|%d| \n", _parent);

    #ifdef SCHEDULER_DEBUG_ADD_PCB
    schedules_tbl[_parent].back() >> _pc_initial;
    printf("\nScheduler::add_process: \t PC:|%d| \t Program:[%d] \n",
            _pc_initial, _parent);
    #endif
}

void Scheduler::kill_process(PCB* _process)
{
    if (_process == nullptr)
        return;

    UUID _uuid = _process->parent_id();
                 _process->set_status(Status::TERMINATED);

    // check parent queue exists
    if (schedules_tbl.count(_uuid))
    {
        // remove parent if depleted
        if (processes(_uuid) < 1)
        {
            RR.remove(_process->parent_id());
        }
    }
    else printf("ERROR: scheduler failed to kill process... UUID:[%d] \n", _uuid);

    #ifdef SCHEDULER_DEBUG_KILL_PCB
    printf("\nScheduler::kill_process:\t Program:[%d] \t PCBs:|%d| \n",
            _uuid, processes(_process->parent_id()));
    #endif
}

PCB Scheduler::fetch_next()
{
    PCB process_;
    schedules_tbl[RR.next()].dequeue(&process_);

    process_.set_status(Status::ACTIVE);

    // hault OS to notify of draw
    if (++m_cycles > max_cycles())
    {
        process_.set_status(Status::HAULTED);
    }
    // OS exit, all processes executed
    if (RR.is_running() == false)
    {
        process_.set_status(Status::EXIT);
    }

    #ifdef SCHEDULER_DEBUG
    printf("\nScheduler::fetch_next: \t Program:[%d] \t Processes:|%d| \n",
           process_.parent_id(), processes(process_.parent_id()) );
    #endif

    return process_;
}

void Scheduler::return_process(PCB *_process)
{
    if (_process == nullptr || _process->status() == Status::TERMINATED)
        return;

    UUID _uuid = _process->parent_id();

    if (schedules_tbl.count(_uuid))
    {
        if (processes(_uuid) < max_processes())
        {
            schedules_tbl[_uuid].enqueue(*_process);
        }
    }
}

} /* ::OS */
