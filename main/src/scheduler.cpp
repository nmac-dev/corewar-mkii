/// Handles warrior processes in a round robin system, ensures one process each per cycle

#include "scheduler.hpp"

/// Operating System handles: fetch/decode/execute cycle, memory simulator, and warrior processes
namespace OS {

Scheduler::Scheduler(ASM::WarriorVec *_warriors, int _cycles, int _processes)
{
    ini_max_cycles     = _cycles;
    ini_max_processes  = _processes;
    cycles_counter     = 0;
    schedules_tbl.reserve(_warriors->size());

    // create a queue for each warrior
    for (int i = 0; i < _warriors->size(); i++)
    {
        int UUID_ = (*_warriors)[i].get()->uuid();
        RR.push_back(UUID_);

        // create a  schedule for the warrior with an initial process
        schedules_tbl[UUID_] = PrcsQueue();
        this->add_process(UUID_, (*_warriors)[i].get()->address());
    }

    #ifdef SCHEDULER_DEBUG
    printf("\nScheduler::Scheduler: initialised with |%d| processes \n",
            size());
    #endif
}
Scheduler::Scheduler()  = default;

void Scheduler::add_process(int _parent, int _pc_initial)
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
    printf("\nScheduler::add_process: \t PC:|%d| \t Warrior:[%d] \n",
            _pc_initial, _parent);
    #endif
}

void Scheduler::kill_process(PCB& _process)
{
    int _UUID = _process.parent_id();
                _process.set_status(Status::TERMINATED);

    // check warrior queue exists
    if (schedules_tbl.count(_UUID))
    {   
        // remove warrior if defeated
        if (processes(_UUID) < 1)
        {
            auto itr = schedules_tbl.find(_UUID);
            schedules_tbl.erase(itr);
            RR.remove(RR.prev());
        }
    }
    else printf("ERROR: scheduler kill_process failed... Warrior:[%d] \n", _UUID);

    #ifdef SCHEDULER_DEBUG_KILL_PCB
    printf("\nScheduler::kill_process:\t Warrior:[%d] \t PCBs:|%d| \n",
                _UUID, processes(_process.parent_id()));
    #endif
}

PCB Scheduler::fetch_next()
{
    PCB process_;
    schedules_tbl[RR.i()].dequeue(&process_);

    process_.set_status(Status::ACTIVE);

    // hault OS to notify of draw
    if (++cycles_counter > max_cycles())
    {
        process_.set_status(Status::HAULTED);
    }
    // OS exit, all other warriors have been killed (victory)
    if (schedules_tbl.size() < 2) 
    {
        process_.set_status(Status::EXIT);
    }

    // schedules_tbl[RR.i()].push(process_); // add process to back of queue
    RR.next();                         // move to next round robin sequance
    
    #ifdef SCHEDULER_DEBUG
    printf("\nScheduler::fetch_next: \t Warrior:[%d] \t Processes:|%d| \n",
           process_.parent_id(), processes(process_.parent_id()) );
    #endif

    return process_;
}

void Scheduler::return_process(PCB _process)
{
    int _UUID = _process.parent_id();
    if (_process.status() == Status::TERMINATED)
        return;
    
    if (schedules_tbl.count(_UUID))
    {
        if (processes(_UUID) < max_processes())
        {
            schedules_tbl[_UUID].enqueue(_process);
        }
    }
}

} /* ::OS */
