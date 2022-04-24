/// Handles warrior processes in a round robin system, ensures one process each per cycle

#include "scheduler.hpp"

/// Operating System handles: fetch/decode/execute cycle, memory simulator, and warrior processes
namespace OS {

Scheduler::Scheduler(ASM::WarriorList *warriors, int _cycles, int _processes)
{
    max_cycles    = _cycles;
    max_processes = _processes;
    schedules_tbl.reserve(warriors->size());

    cycles = 0;

    std::vector<int> warrior_UUIDs;
    warrior_UUIDs.reserve(warriors->size());

    // create a queue for each warrior
    for (int i = 0; i < warriors->size(); i++)
    {
        int core_index        = (*warriors)[i].get()->getCoreIndex();
        warrior_UUIDs.push_back((*warriors)[i].get()->getUUID());

        // create a  schedule for the warrior with an initial process
        schedules_tbl[warrior_UUIDs[i]] = PrcsQueue(new Queue<PCB>);
        this->addProcess(warrior_UUIDs[i], (*warriors)[i].get()->getCoreIndex());
    }
    RR = RR_T(warrior_UUIDs);

    #ifdef SCHEDULER_DEBUG
    printf("\nScheduler::Scheduler: initialised with |%d| processes accross |%d| warriors \n",
            totalPCBs(), totalWarriors());
    #endif
}
Scheduler::Scheduler(){}

void Scheduler::addProcess(int parent_ID, int pc_initial)
{
    // validate warrior was pre-loaded in contructor
    if (schedules_tbl.count(parent_ID))
    {
        if (schedules_tbl[parent_ID].get()->size() < max_processes)
        {
            schedules_tbl[parent_ID].get()->push(PCB(parent_ID, pc_initial));
        }
    }
    else printf("ERROR: scheduler failed to add process... UUID|%d| \n", parent_ID);

    #ifdef SCHEDULER_DEBUG_ADD_PCB
    schedules_tbl[parent_ID].get()->back() >> pc_initial;
    printf("\nScheduler::addProcess: \t PC:|%d| \t Warrior:[%d] \n",
            pc_initial, parent_ID );
    #endif
}

void Scheduler::pushJump(PCB const &_prcs, int value)
{
    // check exists
    if (schedules_tbl.count(_prcs.getParentID()))
    {
        // validate matching process 
        PCB *back_process = schedules_tbl[_prcs.getParentID()].get()->editBack();
        if (back_process != nullptr)
        {
            *back_process << value;
        }
        else printf("ERROR: scheduler failed to push Jump... UUID|%d| \n", _prcs.getParentID());
    }
}

void Scheduler::killBackProcess(PCB& _prcs)
{
    int _UUID = _prcs.getParentID(); // parent UUID

    // check warrior queue exists
    if (schedules_tbl.count(_UUID))
    {
        // validate rollback success
        schedules_tbl[_UUID].get()->kickBack();
    
        _prcs.setStatus(Status::TERMINATED);
        
        // remove warrior if defeated
        if (schedules_tbl[_UUID].get()->size() < 1)
        {
            auto itr = schedules_tbl.find(_UUID);
            schedules_tbl.erase(itr);
            RR.remove(RR.prev());
        }
    }
    else printf("ERROR: scheduler killBackProcess failed... Warrior:[%d] \n", _UUID);

    #ifdef SCHEDULER_DEBUG_KILL_PCB
    printf("\nScheduler::killBackProcess:\t Warrior:[%d] \t PCBs:|%d| \n",
                _UUID, warriorPCBs(_prcs.getParentID()));
    #endif
}

PCB Scheduler::nextProcess()
{
    PCB prcs_;
    schedules_tbl[RR.i()].get()->pop(&prcs_);

    // hault process to notify of draw
    if (cycles++ > max_cycles)
    {
        prcs_.setStatus(Status::HAULTED);
    }

    // process exit, all other warriors have been killed (victory)
    if (schedules_tbl.size() < 2) 
    {
        prcs_.setStatus(Status::EXIT);
    }

    #ifdef SCHEDULER_DEBUG
    printf("\nScheduler::nextProcess: \t PID:[%d] PCBs:|%d| \t Warrior:[%d] \n",
            prcs_.getPID(), warriorPCBs(prcs_.getParentID()), prcs_.getParentID());
    #endif

    schedules_tbl[RR.i()].get()->push(prcs_); // add process to back of queue
    RR.next();                                // move to next round robin sequance
    return prcs_;
}
    
} // namespace OS
