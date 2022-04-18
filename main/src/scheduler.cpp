/// Handles warrior processes in a round robin system, ensures one process each per cycle

#include "scheduler.hpp"

/// Operating System handles: fetch/decode/execute cycle, memory simulator, and warrior processes
namespace OS {

Scheduler::PrcsLog::PrcsLog(ASM::Warrior *_warrior, PCB _process, int _n_pcbs)
{
    warrior = _warrior;
    process = _process;
    n_pcbs  = _n_pcbs;
    status  = Status::ACTIVE;
    has_won = false;
}
Scheduler::PrcsLog::PrcsLog(){}

RRSystem::RRSystem(ASM::WarriorList &warriors, int _length) 
{
    length  = _length;
    pos     = 0;
    uuids.reserve(length);

    // populate
    while (pos < len())
    {
       uuids[pos] = warriors[pos].getUUID();
       this->next();
    }
    this->next(); // reset to 0
}
RRSystem::RRSystem() {}

Scheduler::Scheduler(ASM::WarriorList &warriors, int _rounds, int _cycles)
{
    max_rounds  = _rounds;
    max_cycles  = _cycles;
    t_pcbs      = 0;
    RR          = RRSystem(warriors, warriors.size());
    schedules_tbl.reserve(RR.len() -1);

    // create a queue for each warrior
    for (int i = 0; i < RR.len(); i++)
    {
        ASM::Warrior *warrior_i = &warriors[i];

        // create a  schedule for the warrior with an initial process
        schedules_tbl[RR.UUID()] = PrcsQueue(new PQ_T(warrior_i));
        this->addProcess(warrior_i, warrior_i->getCoreIndex());
        RR.next();
    }
    RR.next();  // reset RR to 0

    #ifdef SCHEDULER_DEBUG
        printf("\nScheduler::Scheduler: initialised with |%d| processes accross |%d| warriors \n",
                schedules_tbl.size(), RR.len()
        );
    #endif
}
Scheduler::Scheduler(){}

void Scheduler::addProcess(ASM::Warrior *parent, int pc_initial)
{
    // validate not null
    if (!parent || parent == nullptr || parent == NULL) return;
    
    // validate warrior was pre-loaded in contructor
    if (schedules_tbl.count(parent->getUUID()))
    {
        schedules_tbl[parent->getUUID()].get()->push(PCB(pc_initial));
        t_pcbs++;
    }

    #ifdef SCHEDULER_DEBUG
        schedules_tbl[parent->getUUID()].get()->back() >> pc_initial;
        printf("\nScheduler::addProcess: \t PC:|%d| \t Parent:[%d] '%s' \n",
                pc_initial, parent->getUUID(), parent->getName().c_str()
        );
    #endif
}

int Scheduler::peekPC()
{
    int process_pc;
    schedules_tbl[RR.UUID()].get()->front() >> process_pc;    
    return process_pc;
}

Scheduler::PrcsLog Scheduler::nextRR(ASM::_OP opcode)
{
    // process log to return
    Scheduler::PrcsLog pl_ (
        schedules_tbl[RR.UUID()].get()->parent(),
        schedules_tbl[RR.UUID()].get()->pop(),
        schedules_tbl[RR.UUID()].get()->size()
    );

    // return if warrior has won
    if (schedules_tbl.size() == 1)
    {
        pl_.has_won = true;
        return pl_;
    }
    
    // kill process on DAT opcode
    if (opcode == ASM::_OP::DAT)
    {
        pl_.status = Status::TERMINATED;
        pl_.n_pcbs--;
        
        // remove warrior if defeated
        if (schedules_tbl[RR.UUID()].get()->size() < 1)
        {
            auto itr = schedules_tbl.find(RR.UUID());
            schedules_tbl.erase(itr);
            RR.remove(RR.UUID());
        }
    }
    else // place process to back of queue
    {
        schedules_tbl[RR.UUID()].get()->push(pl_.process);
    }

    #ifdef SCHEDULER_DEBUG
        printf("\nScheduler::nextRR: \t PID:[%d] PCBs:|%d| \t Parent:[%d] '%s' \n",
                pl_.process.getPID(), pl_.n_pcbs, 
                pl_.warrior->getUUID(), pl_.warrior->getName().c_str()
        );
    #endif
    
    RR.next();
    return pl_;
}
    
} //// namespace OS
