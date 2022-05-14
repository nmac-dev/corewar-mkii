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
                   sched_.return_process(process_);                 \
                                                                    \
    int UUID_      = process_.parent_id();                          \
    int processes_ = sched_.processes(UUID_);
    /* TS__SCHEDULER__SET_TEST_ENV() */

BoolInt ROUND_ROBIN_LOOP();    /** TEST: round robin system                */
BoolInt ADD_PRCS();            /** TEST: add  processes                    */
BoolInt KILL_PRCS();           /** TEST: kill processes                    */
BoolInt MAX_PROCESSES_LIMIT(); /** TEST: adding process over max           */
} /* ::{anonymous} */

/** ALLTESTS: ( OS::Scheduler ) */
BoolInt ALL_TESTS()
{
 /** ALLTESTS: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    BoolInt results_ = TEST_PASSED;
 /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    if ( results_ += ROUND_ROBIN_LOOP()    ) return results_;
    if ( results_ += ADD_PRCS()            ) return results_;
    if ( results_ += KILL_PRCS()           ) return results_;
    if ( results_ += MAX_PROCESSES_LIMIT() ) return results_;
 /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    return results_;
} /* ALL_TESTS() */

namespace /* {anonymous} */
{
/** TEST: round robin loop */
BoolInt ROUND_ROBIN_LOOP()
{
    int constexpr n_warriors = 12;
    TS__SCHEDULER__SET_TEST_ENV(n_warriors)

 /** SUITE: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    Header HDR_ (suite_info( {"RoundRobin", "ROUND_ROBIN_LOOP()", ""} ));
    int E_, A_;
 /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
 HDR_.info.test_desc = "Complete Multiple Index Rotations";

    E_ = UUID_;

    int len = sched_.rr_len();
    for (int i = 0; i < (len * len); i++)
    {
        process_ = sched_.fetch_next();
                   sched_.return_process(process_);
    }
    A_ = process_.parent_id();
    RUN_TEST(E_, A_, HDR_);
 /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    return HDR_.result;
} /* ROUND_ROBIN_LOOP() */

/** TEST: add processes */
BoolInt ADD_PRCS()
{
    int constexpr n_warriors = 3;
    TS__SCHEDULER__SET_TEST_ENV(n_warriors)

 /** SUITE: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    Header HDR_ (suite_info( {"add_process()", "ADD_PRCS()", ""} ));
    int E_, A_;
 /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
 HDR_.info.test_desc = "Add Processes";

    E_ = sched_.max_processes();

    while (processes_++ != sched_.max_processes())
    {
        sched_.add_process(UUID_, 0);
    }
    A_ = sched_.processes(UUID_);
    RUN_TEST(E_, A_, HDR_);
 /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    return HDR_.result;
} /* ADD_PRCS() */

/** TEST: kill processes */
BoolInt KILL_PRCS()
{
    int constexpr n_warriors = 3;
    TS__SCHEDULER__SET_TEST_ENV(n_warriors)

    while (sched_.size() != sched_.max_processes())
    {
        sched_.add_process(UUID_, 0);
    }
    processes_ = sched_.max_processes();

 /** SUITE: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    Header HDR_ (suite_info( {"kill_process()", "KILL_PRCS()", ""} ));
    int E_, A_;
 /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
 HDR_.info.test_desc = "Kill Processes";

    E_ = 0;

    while (processes_-- > 0)
    {
        process_ = sched_.fetch_next();
        sched_.kill_process(process_);
    }
    process_ = sched_.fetch_next(); // fetch last

    A_ = sched_.processes(UUID_);
    RUN_TEST(E_, A_, HDR_);
 /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    return HDR_.result;
} /* KILL_PRCS() */

/** TEST: adding process over max */
BoolInt MAX_PROCESSES_LIMIT()
{
    int constexpr n_warriors = 3;
    TS__SCHEDULER__SET_TEST_ENV(n_warriors)

    int test_limit = sched_.max_processes() + 5;

 /** SUITE: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    Header HDR_ (suite_info( {"max_processes()", "MAX_PROCESSES_LIMIT()", ""} ));
    int E_, A_;
 /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
 HDR_.info.test_desc = "Add Processes Over Limit";

    E_ = sched_.max_processes();

    while (processes_++ != test_limit)
    {
        sched_.add_process(UUID_, 0);
    }
    A_ = sched_.processes(UUID_);
    RUN_TEST(E_, A_, HDR_);
 /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    return HDR_.result;
} /* MAX_PROCESSES_LIMIT() */

} /* ::{anonymous}  */
}}/* ::TS::_Scheduler_ */
