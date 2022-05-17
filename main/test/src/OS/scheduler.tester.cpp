#include "OS/scheduler.tester.hpp"

int main(int argc, char const *argv[])
{
    return TS::_Scheduler_::ALL_TESTS();
}

namespace TS { namespace _Scheduler_
{
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
    int constexpr n_programs = 12;
    TS__SCHEDULER__SET_TEST_ENV(n_programs)

 /** SUITE: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    Header HDR_ (suite_info( {"RoundRobin", "ROUND_ROBIN_LOOP()", ""} ));
    int E_, A_;
    int n_queues;
 /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
 HDR_.info.test_desc = "Complete Multiple Index Rotations";

    E_ = UUID_;

    n_queues = sched_.programs();
    for (int i = 0; i < (n_queues * n_queues); i++)
    {
        process_ = sched_.fetch_next();
                   sched_.return_process(&process_);
    }
    A_ = process_.parent_id();
    RUN_TEST(E_, A_, HDR_);
 /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
 HDR_.info.test_desc = "Deplete Round Robin";

    E_ = 1;

    sched_ = Scheduler(&programs, max_cycles, max_processes);

    n_queues = sched_.programs();
    for (int i = 0; i < n_queues; i++)
    {
        process_ = sched_.fetch_next();
                   sched_.kill_process(&process_);
                   sched_.return_process(&process_);
    }
    A_ = sched_.programs();
    RUN_TEST(E_, A_, HDR_);
 /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
 HDR_.info.test_desc = "Exit on Last UUID";

    E_ = (int) Status::EXIT;

    sched_ = Scheduler(&programs, max_cycles, max_processes);

    n_queues = sched_.programs();
    for (int i = 0; i < n_queues; i++)
    {
        process_ = sched_.fetch_next();
                   sched_.kill_process(&process_);
                   sched_.return_process(&process_);
    }
    process_ = sched_.fetch_next();
    A_ = (int) process_.status();
    RUN_TEST(E_, A_, HDR_);
 /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    return HDR_.result;
} /* ROUND_ROBIN_LOOP() */

/** TEST: add processes */
BoolInt ADD_PRCS()
{
    int constexpr n_programs = 3;
    TS__SCHEDULER__SET_TEST_ENV(n_programs)

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
    int constexpr n_programs = 3;
    TS__SCHEDULER__SET_TEST_ENV(n_programs)

    while (sched_.processes(UUID_) != sched_.max_processes())
    {
        sched_.add_process(UUID_, 0);
    }

 /** SUITE: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    Header HDR_ (suite_info( {"kill_process()", "KILL_PRCS()", ""} ));
    int E_, A_;
 /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
 HDR_.info.test_desc = "Kill Processes";

    E_ = 0;

    while (sched_.processes() != 0)
    {
        process_ = sched_.fetch_next();
        sched_.kill_process(&process_);
    }
    process_ = sched_.fetch_next(); // fetch last
        sched_.kill_process(&process_);
        sched_.kill_process(&process_);

    A_ = sched_.processes(UUID_);
    RUN_TEST(E_, A_, HDR_);
 /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    return HDR_.result;
} /* KILL_PRCS() */

/** TEST: adding process over max */
BoolInt MAX_PROCESSES_LIMIT()
{
    int constexpr n_programs = 3;
    TS__SCHEDULER__SET_TEST_ENV(n_programs)

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
