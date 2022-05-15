#pragma once
#include "template/test_suite.hpp"
/** CPU: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#include "OS/cpu.hpp"

namespace TS { namespace _CPU_
{
namespace /* {anonymous} */
{
    using namespace OS;

Info suite_info(Info _info)
{
    _info.func_name = "CPU::" + _info.func_name;
    return _info;
}

#define TS__CPU__CONST_OPRS()                         \
    Inst::Operand const base_f {Admo::IMMEDIATE, 0};  \
    Inst::Operand const jump_f {Admo::DIRECT,    3};  \
    Inst::Operand const src_f  {Admo::IMMEDIATE, 8};  \
    Inst::Operand const dest_f {Admo::IMMEDIATE, 4};
    /* TS__CPU__CONST_OPRS() */

#define TS__CPU__SET_TEST_ENV(N_INST, TEST_INSTS)                            \
    int constexpr max_warrior_len = 16,                                      \
                  min_seperation  = 20,                                      \
                  max_cycles      = 1000,                                    \
                  max_processes   = 2,                                       \
                  n_warriors      = 2;                                       \
                                                                             \
    WarriorVec warriors;                                                     \
                warriors.reserve(n_warriors);                                \
    for (int i = 0; i < n_warriors; i++)                                     \
    {                                                                        \
        warriors.push_back(                                                  \
            UniqWarrior (                                                    \
                new Warrior("TS::_CPU_::Warrior", N_INST, max_warrior_len)   \
            )                                                                \
        );                                                                   \
                                                                             \
        for (int k = 0; k < N_INST; k++)                                     \
        {                                                                    \
            warriors[i].get()->push(TEST_INSTS[k]);                          \
        }                                                                    \
    }                                                                        \
                                                                             \
    Memory memory_(&warriors, min_seperation);                               \
    Scheduler sched_(&warriors, max_cycles, max_processes);                  \
                                                                             \
    CPU core_(&memory_, &sched_);
    /* TS__CPU__SET_TEST_ENV() */

#define TS__CPU__RUN_TEST()                  \
    RUN_TEST(E_, A_, HDR_);                  \
    for (int i = 0; i < n_warriors -1; i++)  \
        core_.run_fde_cycle();
    /* TS__CPU__RUN_TEST() */

BoolInt SYSTEM_CODES();      /** TEST: all system [code]...      NOP, DAT, MOV, SPL       */
BoolInt COMPARISION_CODES(); /** TEST: all comparision [code]... SEQ, SNE, SLT            */
BoolInt ARITHMETIC_CODES();  /** TEST: all arithmetic [code]...  ADD, SUB, MUL, DIV, MOD  */
BoolInt JUMP_CODES();        /** TEST: all jump [code]...        JMP, JMZ, JMN, DJN       */

} /* ::{anonymous} */

BoolInt ALL_TESTS(); /** ALLTESTS: ( OS::CPU ) */

}} /* ::TS::_CPU_ */
