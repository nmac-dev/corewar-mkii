#pragma once
#include "template/test_suite.hpp"
/** MEMORY: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#include "OS/memory.hpp"

namespace TS { namespace _Memory_
{
namespace /* {anonymous} */
{
    using namespace OS;
    using namespace ASM;

Info suite_info(Info _info)
{
    _info.func_name = "Memory::" + _info.func_name;
    return _info;
}

#define TS__MEMORY__SET_TEST_ENV()                                              \
    constexpr int max_warrior_len = 12,                                         \
                  min_seperation  = 12;                                         \
                                                                                \
    ASM::WarriorVec warriors;                                                   \
    warriors.push_back(                                                         \
        ASM::UniqWarrior ( new ASM::Warrior("example", 1, max_warrior_len) )    \
    );                                                                          \
    warriors[0].get()->push(Inst());                                            \
                                                                                \
    Memory mars_(&warriors, min_seperation);
    /* TS__MEMORY__SET_TEST_ENV() */

BoolInt OUT_OF_BOUNDS(); /** TEST: out of bounds (lower | upper)                        */
BoolInt ALL_ADMOS();     /** TEST: all <admo> '#', '$', '*', '@', '<', '{', '>', '}'    */
BoolInt ALL_MODIFIERS(); /** TEST: all <mod> '.a', '.b', '.ab', '.ba', '.f', '.x', '.i' */

} /* ::{anonymous} */

BoolInt ALL_TESTS(); /** ALLTESTS: ( OS::Memory ) */

}} /* ::TS::_Memory_ */
