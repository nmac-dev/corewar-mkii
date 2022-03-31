/// Parses assembly instructions from a warrior file into a warrior class 
#pragma once

#define ASM_PARSER_DEBUG

#include <string>
#include <unordered_map>

#include "file_loader.hpp"
#include "settings.hpp"
#include "asm.hpp"

namespace parser
{
    using namespace ASM;

    /**** Alias Declarations ****/
    
/// A hash table used as a label linker, storing the label position relative to the warrior
using LabelLinker = std::unordered_map<std::string, int>;


namespace /**** Anonymous Namespace ****/
{

    /**** Lookup HashTables ****/

/// Hashes an opcode string to get the enum value
std::unordered_map<std::string, _OP> opcode_tbl =
{
    {"dat", _OP::DAT}, {"mov", _OP::MOV}, {"add", _OP::ADD}, {"sub", _OP::SUB}, 
    {"mul", _OP::MUL}, {"div", _OP::DIV}, {"mod", _OP::MOD}, {"jmp", _OP::JMP}, 
    {"jmz", _OP::JMZ}, {"jmn", _OP::JMN}, {"djn", _OP::DJN}, {"cmp", _OP::CMP},  
    {"slt", _OP::SLT}, {"spl", _OP::SPL}
};

/// Hashes a modifier string to get the enum value
std::unordered_map<std::string, _MOD> modifier_tbl =
{
    {"a", _MOD::A}, {"b", _MOD::B}, {"ab", _MOD::AB}, {"ba", _MOD::BA}, 
    {"f", _MOD::F}, {"x", _MOD::X}, {"i",  _MOD::I}
};

/// Hashes an addressing mode string to get the enum value
std::unordered_map<char, _AM> admo_tbl =
{
    {'#', _AM::IMMEDIATE}, {'$', _AM::DIRECT},     {'*', _AM::A_INDIRECT}, {'@', _AM::B_INDIRECT}, 
    {'{', _AM::A_PRE_DEC}, {'}', _AM::A_POST_INC}, {'<', _AM::B_PRE_DEC},  {'>', _AM::B_PRE_INC}
};


    /**** Parser Utility Functions ****/

/** Outputs an error for an invalid assembly argument with the full instruction, then triggers an Exception
 * @param full_inst 
 * @param asm_arg 
 */
inline void invalidAssemblyError(std::string full_inst, std::string asm_arg);

/** Validates if: char is an assembly instruction argument seperator [ ' ' | '\\t' | ' . ' | ' , ' ]
 * @param val char value to check
 * @return true if char is a number
 */
inline bool isAsmSeperator(char val);

/** Takes an assembly instruction string and returns the next valid argument from the position given 
 * @param str assembly instruction string
 * @param pos start position to look for the next assembly argument
 * @return string of the assembly argument
 */
inline std::string findAsmArgument(std::string str, int &pos);

/** Cleans instruction string (removes extra inline spaces & resizes string)
 * @param str assembly instruction as a string
 */
inline std::string cleanAsmStr(std::string str);

/** Gets the Default Modifier for the opcode & addressing modes
 * @param opcode assembly enum opcode
 * @param admo_a assembly enum addressing mode A
 * @param admo_b assembly enum addressing mode B
 * @return default addressing mode reference
 */
inline _MOD getDefaultModifier(_OP opcode, _AM admo_a, _AM admo_b);

} /// Anonymous Namespace


    /**** Parser Functions ****/

/** Creates a label linker to identify the posistions of all labels within an assembly file
 * @param asm_data a vector containing each line of the assembly file data as a string
 * @return LabelLinker as a hashtable
 */
LabelLinker findAsmLabels(std::vector<std::string> &asm_data);

/** Parses an assembly code string to an asm_instr object
 * @param str in the format of: <label> [opcode]<.modifier> <mode_a>[op_a],<mode_b>[op_b]
 * @param label_linker used to store and handle labels
 * @return Asm Instruction object
 */
Inst asmStrToInst(std::string &str, LabelLinker &linker);

/// Loads a warrior from a .asm file
Warrior asmFileToWarrior(std::string warrior_name);

} /// namespace parser