#include "template/test_suite.hpp"

#include "file_loader.hpp"
#include "parser.hpp"

namespace TS { namespace _Parser_
{
namespace /* {anonymous} */
{
    using namespace Parser;

Info suite_info(Info _info)
{
    _info.func_name = "Parser::" + _info.func_name;
    return _info;
}

BoolInt CLEAN_ASSEMBLY();                       /** TEST: cleaning assembly code */
BoolInt LABEL_LINKER();                         /** TEST: label linker           */
BoolInt PARSE_ASSEMBLY(AssemblyCode _assembly); /** TEST: parsed assembly file   */
} /* ::{anonymous} */

/** ALLTESTS: Parser */
BoolInt ALL_TESTS()
{
    char constexpr test_file[] = "test_warriors/parser_test.asm";
    AssemblyCode assembly_; 
    try
    {
        assembly_ = AssemblyCode(file_loader::getFileData(test_file, ASM_CDOE_COMMENT));
    }
    catch(const std::exception &) { return TEST_FAILED; }
 /** ALLTESTS: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    BoolInt results_ = TEST_PASSED;
 /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    if ( results_ += CLEAN_ASSEMBLY()          ) return results_;
    if ( results_ += LABEL_LINKER()            ) return results_;
    if ( results_ += PARSE_ASSEMBLY(assembly_) ) return results_;
 /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    return results_;
} /* ::ALL_TESTS() */

namespace /* {anonymous} */
{
/** TEST: cleaning assembly code */
BoolInt CLEAN_ASSEMBLY()
{
    int const n_tests = 3;

    std::string clean_[n_tests]
    {
        "dat 0, 0",
        "label_name: add.ab #0, $0",
        "mov 0, 0"
    };
    std::string raw_lines[n_tests] 
    {
        "dat 0,0            ;; COMMENT",
        "label_name: add.ab   #0,   $0",
        "       mov    0    ,    0    "
    };

 /** SUITE: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    Header HDR_ (CMP_OP::EQ, suite_info( {"cleanAsmStr()", "CLEAN_ASSEMBLY()", ""} ));
    std::string E_, A_;
 /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
 HDR_.info.test_desc = "Clean Raw Assembly Code";

    for (size_t i = 0; i < n_tests; i++)
    {
        E_ = clean_[i];
        A_ = cleanAsmStr(raw_lines[i]);

        TS::RUN_TEST(E_, A_, HDR_);
    }
    return HDR_.result;
} /* ::CLEAN_ASSEMBLY() */

/** TEST: label linker */
BoolInt LABEL_LINKER()
{
    AssemblyCode labels_ {
        "abcdefghijklmnopqrstuvwxyz", // a -> z
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ", // A -> Z
        "_0123456789",                //_, 0 -> 9
        "no_colon"                    // No Colon (valid)
    };

    std::string nop_line = " nop 0, 0";
    AssemblyCode raw_code {
        labels_[0] + ":" + nop_line,
        labels_[1] + ":" + nop_line,
        labels_[2] + ":" + nop_line,
        labels_[3]       + nop_line,
    };

 /** SUITE: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    Header HDR_ (CMP_OP::EQ, suite_info( {"findAsmLabels()", "LABEL_LINKER()", ""} ));
    bool E_, A_;
 /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
 HDR_.info.test_desc = "Label Linker";

    E_ = true;

    LabelLinker linker_;
    linker_ = findAsmLabels(raw_code);

    for (int i = 0; i < raw_code.size(); i--)
    {
        int pos = 0;
        std::string first_arg = findAsmArgument(raw_code[i], pos);

        A_ = linker_.count(labels_[i]);

        RUN_TEST(E_, A_, HDR_);
    }
 /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    return HDR_.result;
} /* ::LABEL_LINKER() */

/** TEST: parsed assembly file */
BoolInt PARSE_ASSEMBLY(AssemblyCode _assembly)
{
    char constexpr defaults_file[] = "test_warriors/syntax_defaults.asm";

    AssemblyCode _defaults;  // DO NOT ClEAN
    try
    {
        _defaults = AssemblyCode(file_loader::getFileData(defaults_file, ASM_CDOE_COMMENT));
    }
    catch(const std::exception &) { return TEST_FAILED; }

    for (int i = 0; i < _assembly.size(); i++)
        _assembly[i] = cleanAsmStr(_assembly[i]);

 /** SUITE: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    Header HDR_ (CMP_OP::EQ, suite_info( {"asmCodeToInst()", "PARSE_ASSEMBLY()", ""} ));
    std::string E_, A_;
 /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
 HDR_.info.test_desc = "Parsed Assembly Code";

    LabelLinker label_linker = findAsmLabels(_assembly);

    for (int i = 0; i < _defaults.size(); i++)
    {
        E_ = _defaults[i];
        A_ = asmStrToInst(_assembly[i], label_linker, i).toAsmCode();

        RUN_TEST(E_, A_, HDR_);
    }
 /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    return HDR_.result;
} /* ::PARSE_ASSEMBLY() */

} /* ::{anonymous} */
}}/* ::TS::_Parser_ */
