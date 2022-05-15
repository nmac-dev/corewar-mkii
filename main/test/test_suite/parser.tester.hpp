#pragma once
#include "template/test_suite.hpp"
/** PARSER: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

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

BoolInt CLEAN_ASSEMBLY(); /** TEST: cleaning assembly code */
BoolInt LABEL_LINKER();   /** TEST: label linker           */
BoolInt PARSE_ASSEMBLY(); /** TEST: parsed assembly file   */

} /* ::{anonymous} */

BoolInt ALL_TESTS(); /** ALLTESTS: ( Parser ) */

}}/* ::TS::_Parser_ */
