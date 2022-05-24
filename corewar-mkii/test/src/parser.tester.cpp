#include "parser.tester.hpp"

int main(int argc, char const *argv[])
{
    return TS::_Parser_::ALL_TESTS();
}

namespace TS { namespace _Parser_
{
/** ALLTESTS: ( Parser ) */
BoolInt ALL_TESTS()
{
 /** ALLTESTS: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    BoolInt results_ = TEST_PASSED;
 /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    if ( results_ += CLEAN_ASSEMBLY() ) return results_;
    if ( results_ += LABEL_LINKER()   ) return results_;
    if ( results_ += PARSE_ASSEMBLY() ) return results_;
 /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    return results_;
} /* ALL_TESTS() */

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
    Header HDR_ (suite_info( {"clean_assembly()", "CLEAN_ASSEMBLY()", ""} ));
    std::string E_, A_;
 /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
 HDR_.info.test_desc = "Clean Raw Assembly Code";

    for (size_t i = 0; i < n_tests; i++)
    {
        E_ = clean_[i];
        A_ = clean_assembly(raw_lines[i]);

        TS::RUN_TEST(E_, A_, HDR_);
    }
    return HDR_.result;
} /* CLEAN_ASSEMBLY() */

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
    Header HDR_ (suite_info( {"generate_label_linker()", "LABEL_LINKER()", ""} ));
    bool E_, A_;
 /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
 HDR_.info.test_desc = "Label Linker";

    E_ = true;

    LabelLinker linker_;
    linker_ = generate_label_linker(raw_code);

    for (int i = 0; i < raw_code.size(); i--)
    {
        int pos = 0;
        std::string first_arg = find_argument(raw_code[i], pos);

        A_ = linker_.count(labels_[i]);

        RUN_TEST(E_, A_, HDR_);
    }
 /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    return HDR_.result;
} /* LABEL_LINKER() */

/** TEST: parsed assembly file */
BoolInt PARSE_ASSEMBLY()
{
    char constexpr test_file[]     = "warriors.tester/parser.tester.asm",
                   defaults_file[] = "warriors.tester/syntax_defaults.asm";

    AssemblyCode assembly_, // clean 
                 defaults_; // DO NOT clean
    try
    {
        assembly_ = AssemblyCode(File_Loader::load_file_data(test_file, ASSEMBLY_COMMENT));
        defaults_ = AssemblyCode(File_Loader::load_file_data(defaults_file, ASSEMBLY_COMMENT));
    }
    catch(const std::exception &) { return TEST_FAILED; }

    for (int i = 0; i < assembly_.size(); i++)
    {
        assembly_[i] = clean_assembly(assembly_[i]);
    }
 /** SUITE: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    Header HDR_ (suite_info( {"asmCodeToInst()", "PARSE_ASSEMBLY()", ""} ));
    std::string E_, A_;
 /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
 HDR_.info.test_desc = "Parsed Assembly Code";

    LabelLinker label_linker = generate_label_linker(assembly_);

    for (int i = 0; i < defaults_.size(); i++)
    {
        E_ = defaults_[i];
        A_ = assembly_to_inst(assembly_[i], label_linker, i).to_assembly();

        RUN_TEST(E_, A_, HDR_);
    }
 /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    return HDR_.result;
} /* PARSE_ASSEMBLY() */

} /* ::{anonymous} */
}}/* ::TS::_Parser_ */
