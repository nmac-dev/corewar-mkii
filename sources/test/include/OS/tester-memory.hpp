#pragma once
#include "template/test_suite.hpp"
/** MEMORY: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#include "memory.hpp"

namespace TS { namespace _Memory_
{
namespace /* {anonymous} */
{
    using namespace OS;
    using namespace Asm;

Info suite_info(Info _info)
{
    _info.func_name = "Memory::" + _info.func_name;
    return _info;
}

#define TS__MEMORY__SET_TEST_ENV()                           \
    constexpr int min_seperation = 12;                       \
                                                             \
    Asm::ProgramVec programs;                                \
    programs.push_back(                                      \
        Asm::UniqProgram ( new Asm::Program("example", 1) )  \
    );                                                       \
    programs[0].get()->push(Inst());                         \
                                                             \
    Memory mars_(&programs, min_seperation);
    /* TS__MEMORY__SET_TEST_ENV() */

BoolInt OUT_OF_BOUNDS(); /** TEST: out of bounds (lower | upper)                        */
BoolInt ALL_ADMOS();     /** TEST: all <admo> '#', '$', '*', '@', '<', '{', '>', '}'    */
BoolInt ALL_MODIFIERS(); /** TEST: all <mod> '.a', '.b', '.ab', '.ba', '.f', '.x', '.i' */

} /* ::{anonymous} */

BoolInt ALL_TESTS(); /** ALLTESTS: ( OS::Memory ) */

}} /* ::TS::_Memory_ */
