#pragma once

#include <iostream>
#include <string>

namespace TS // Test Suite
{
    using BoolInt = int;         // boolean used to sum all failed tests  (false = 0, else true)
    using Desc    = std::string; // string description of element context

BoolInt constexpr TEST_PASSED = 0,
                  TEST_FAILED = 1;

/// Comparison Operators
enum class CMP_OP
{
    EQ,     // '=='
    NEQ,    // '!='
    LT,     // '<'
    GT,     // '>'
    LTEQ,   // '<='
    GTEQ,   // '>='
};

/// Stores Test Suite description
struct Info
{
    Desc func_name,     // full scope name of function being tested
         suite_name,    // name of test suite
         test_desc;     // brief test desctiprion
};

/// Contains testing information
struct Header
{
    BoolInt result;     // Test Suite result
    CMP_OP cmp_op;
    Info info;

    /// Create a header
    /// @param _cmp_op
    /// @param _info
    Header(CMP_OP _cmp_op, Info _info)
    {
        result = TEST_PASSED;
        cmp_op = _cmp_op;
        info   = _info;
    }

    /// Return suite_name + test_desc 
    Desc full_suite_name()
    {
        Desc full_suite_ = info.suite_name;
        char constexpr seperator[] = " ... ";
        
        if (info.test_desc != "")
            full_suite_.append(seperator);
        
        return full_suite_ + info.test_desc; 
    }

    /// Returns a description of the comparison operator
    Desc inline cmp_to_str()
    {
        Desc val_;
        switch (cmp_op)
        {
        case CMP_OP::EQ:   val_ = "=="; break;
        case CMP_OP::NEQ:  val_ = "!="; break;
        case CMP_OP::LT:   val_ = "< "; break;
        case CMP_OP::GT:   val_ = "> "; break;
        case CMP_OP::LTEQ: val_ = "<="; break;
        case CMP_OP::GTEQ: val_ = ">="; break;
        }
        return val_;
    }
};/* Header */

/// Returns the result of the expected & actual values comparison
/// @param _E   expected
/// @param _A   actual
/// @param _HDR header containing test information
template<typename T>
void RUN_TEST(T _E, T _A, Header &_HDR)
{
    BoolInt compare_ = TEST_PASSED;

    switch (_HDR.cmp_op)
    {
    case CMP_OP::EQ:   compare_ |= !(_E == _A); break;
    case CMP_OP::NEQ:  compare_ |= !(_E != _A); break;
    case CMP_OP::LT:   compare_ |= !(_E <  _A); break;
    case CMP_OP::GT:   compare_ |= !(_E >  _A); break;
    case CMP_OP::LTEQ: compare_ |= !(_E <= _A); break;
    case CMP_OP::GTEQ: compare_ |= !(_E >= _A); break;
    }

    if (compare_)
    {
        char constexpr nl[] = "' \n";
        Desc cmp_desc = _HDR.cmp_to_str();

        std::cout
        << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n"
        << "  FAILED!  | FUNC: '"      << _HDR.info.func_name    << nl 
        << "  #######  | TEST: '"      << _HDR.full_suite_name() << nl
        << "           |  CMP: '|E| " << cmp_desc << " |A|"      << nl
        << "..............................................................\n"
        << " |E|xpected: '" << _E << nl
        << " |A|ctual:   '" << _A << nl
        << "..............................................................\n"
        << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n"
        << std::endl;
    }
    _HDR.result += compare_;
} /* RUN_TEST() */

} // namespace TS
