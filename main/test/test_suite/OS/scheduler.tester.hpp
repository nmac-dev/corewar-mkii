#pragma once
#include "template/test_suite.hpp"
/** SCHEDULER: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#include "OS/scheduler.hpp"

namespace TS { namespace _Scheduler_
{
namespace /* {anonymous} */
{
    using namespace OS;

Info suite_info(Info _info)
{
    _info.func_name = "Scheduler::" + _info.func_name;
    return _info;
}

#define TS__SCHEDULER__SET_TEST_ENV(N_WARRIORS)                     \
    int constexpr max_warrior_len = 1,                              \
                  max_cycles      = 200000,                         \
                  max_processes   = 200,                            \
                  program_counter = 1;                              \
                                                                    \
    ASM::WarriorVec warriors;                                       \
    warriors.reserve(N_WARRIORS);                                   \
                                                                    \
    for (int i = 0; i < N_WARRIORS; i++)                            \
    {                                                               \
        warriors.push_back(                                         \
            ASM::UniqWarrior (                                      \
                new ASM::Warrior("example", 1, max_warrior_len)     \
            )                                                       \
        );                                                          \
        warriors[i].get()->set_address(program_counter + i);        \
    }                                                               \
    Scheduler sched_(&warriors, max_cycles, max_processes);         \
                                                                    \
    PCB process_ = sched_.fetch_next();                             \
                   sched_.return_process(&process_);                \
                                                                    \
    int UUID_      = process_.parent_id();                          \
    int processes_ = sched_.processes(UUID_);
    /* TS__SCHEDULER__SET_TEST_ENV() */

BoolInt ROUND_ROBIN_LOOP();    /** TEST: round robin system                */
BoolInt ADD_PRCS();            /** TEST: add  processes                    */
BoolInt KILL_PRCS();           /** TEST: kill processes                    */
BoolInt MAX_PROCESSES_LIMIT(); /** TEST: adding process over max           */

} /* ::{anonymous} */

BoolInt ALL_TESTS(); /** ALLTESTS: ( OS::Scheduler ) */

}}/* ::TS::_Scheduler_ */
