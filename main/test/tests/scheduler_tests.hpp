#include "template/test_suite.hpp"

#include "scheduler.hpp"

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

BoolInt ROUND_ROBIN_LOOP(Scheduler _scheduler);    /** TEST: round robin system                */
BoolInt ADD_PRCS(Scheduler _scheduler);            /** TEST: add  processes                    */
BoolInt KILL_PRCS(Scheduler _scheduler);           /** TEST: kill processes                    */
BoolInt MAX_PROCESSES_LIMIT(Scheduler _scheduler); /** TEST: adding process over max           */
} /* ::{anonymous} */

/** ALLTESTS: Scheduler */
BoolInt ALL_TEST()
{
    int constexpr max_warrior_len = 200,
                  max_cycles      = 40,
                  max_processes   = 200,
                  program_counter = 1,
                  n_warriors      = 3;

    ASM::WarriorList warriors;
    warriors.reserve(n_warriors);

    for (int i = 0; i < n_warriors; i++)
    {
        warriors.push_back(
            ASM::UniqWarrior ( new ASM::Warrior("example", 1, max_warrior_len) )
        );
        warriors[i].get()->setCoreIndex(program_counter + i);
    }    

    Scheduler scheduler_(&warriors, max_cycles, max_processes);
 /** ALLTESTS: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    BoolInt results_ = TEST_PASSED;
 /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    if ( results_ += ROUND_ROBIN_LOOP(scheduler_)    ) return results_;
    if ( results_ += ADD_PRCS(scheduler_)            ) return results_;
    if ( results_ += KILL_PRCS(scheduler_)           ) return results_;
    if ( results_ += MAX_PROCESSES_LIMIT(scheduler_) ) return results_;
 /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    return results_;
} /* ::ALL_TESTS() */

namespace /* {anonymous} */
{
/** TEST: round robin loop */
BoolInt ROUND_ROBIN_LOOP(Scheduler _scheduler)
{
    PCB _process    = _scheduler.nextProcess();
    int const _UUID = _process.getParentID();

 /** SUITE: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    Header HDR_ (CMP_OP::EQ, suite_info( {"RoundRobin", "ROUND_ROBIN_LOOP()", ""} ));
    int E_, A_;
 /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
 HDR_.info.test_desc = "Complete Index Rotation";

    E_ = _UUID;

    for (int i = 0; i < _scheduler.totalWarriors(); i++)  // loop back to first warrior
        _process = _scheduler.nextProcess();

    A_ = _process.getParentID();

    RUN_TEST(E_, A_, HDR_);
 /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    return HDR_.result;
} /* ::ROUND_ROBIN_LOOP */

/** TEST: add processes */
BoolInt ADD_PRCS(Scheduler _scheduler)
{
    PCB _process   = _scheduler.nextProcess();
    int _UUID      = _process.getParentID();
    int _processes = _scheduler.warriorPCBs(_UUID);

 /** SUITE: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    Header HDR_ (CMP_OP::EQ, suite_info( {"addProcess()", "ADD_PRCS()", ""} ));
    int E_, A_;
 /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
 HDR_.info.test_desc = "Add Processes";

    E_ = _scheduler.max_processes();
    while (_processes++ != _scheduler.max_processes())
        _scheduler.addProcess(_UUID, 0);

    A_ = _scheduler.warriorPCBs(_UUID);

    RUN_TEST(E_, A_, HDR_);
 /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    return HDR_.result;
} /* ::ADD_PRCS */

/** TEST: kill processes */
BoolInt KILL_PRCS(Scheduler _scheduler)
{
    PCB _process   = _scheduler.nextProcess();
    int _UUID      = _process.getParentID();
    int _processes = _scheduler.warriorPCBs(_UUID);
    
    while (_processes++ != _scheduler.max_processes())
        _scheduler.addProcess(_UUID, 0);

 /** SUITE: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    Header HDR_ (CMP_OP::EQ, suite_info( {"killBackProcess()", "KILL_PRCS()", ""} ));
    int E_, A_;
 /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
 HDR_.info.test_desc = "Kill Processes";

    E_ = 0;
    while (--_processes > 0)
        _scheduler.killBackProcess(_process);

    A_ = _scheduler.warriorPCBs(_UUID);

    RUN_TEST(E_, A_, HDR_);
 /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    return HDR_.result;
} /* ::KILL_PRCS */

/** TEST: adding process over max */
BoolInt MAX_PROCESSES_LIMIT(Scheduler _scheduler)
{
    PCB _process   = _scheduler.nextProcess();
    int _UUID      = _process.getParentID();
    int _processes = _scheduler.warriorPCBs(_UUID);
    int test_limit = _scheduler.max_processes() + 5;

 /** SUITE: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    Header HDR_ (CMP_OP::EQ, suite_info( {"max_processes()", "MAX_PROCESSES_LIMIT()", ""} ));
    int E_, A_;
 /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
 HDR_.info.test_desc = "Add Processes Over Limit";

    E_ = _scheduler.max_processes();
    while (_processes++ != test_limit)
        _scheduler.addProcess(_UUID, 0);

    A_ = _scheduler.warriorPCBs(_UUID);

    RUN_TEST(E_, A_, HDR_);
 /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    return HDR_.result;
} /* ::MAX_PROCESSES_LIMIT */

} /* ::{anonymous}  */
}}/* ::TS::_Scheduler_ */
