//// Handles warrior processes in a round robin system, ensure one process each per cycle

#include "process_scheduler.hpp"

namespace WOS {

PCB::PCB(Process *new_process, int pc_initial) 
{
    p_id            = createPID();
    process         = new_process;
    state           = State::NEW;
    program_counter = pc_initial;
}

Scheduler::Scheduler(std::vector<ASM::Warrior> &warriors_list)
{
    n_warriors = warriors_list.size();
    // create a queue for each warrior
    for (int i = 0; i < n_warriors; i++)
    {
        // create a queue for warrior [i] with an initial process 
        std::queue<PCB> pcb_queue;
        PCB pcb = PCB(new Process(&warriors_list[i]), warriors_list[i].begin());
        pcb_queue.push(pcb);

        // initialise warrior [i]'s PCB queue with a single process
        warrior_schedule.push_back(pcb_queue);
        this->addProcess(warriors_list[i], warriors_list[i].begin());
    }   
}

void Scheduler::addProcess(ASM::Warrior &parent, int pc_index)
{
    int i = 0;
    while (i < warrior_schedule.size())
    {
        // find parent warrior of process within queues  
        if (warrior_schedule[i++].back().getParentUUID() == parent.getUUID()) 
        {
            // create new PCB with process and set the program counter
            PCB pcb = PCB(new Process(&parent), pc_index);
            // add PCB to back of queue
            warrior_schedule[i].push(pcb);
        }
    }
}

} //// namespace WOS