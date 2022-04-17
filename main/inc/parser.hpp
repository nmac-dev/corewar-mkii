/// Parses assembly instructions from a warrior file into a warrior class 
#pragma once

// #define ASM_PARSER_DEBUG

#define ASM_CDOE_COMMENT ';' // assembly code comment character

#include <unordered_map>
#include <string>
#include <vector>
#include "asm.hpp"

namespace Parser
{

using AssemblyCode = std::vector<std::string>;

namespace //// Anonymous Namespace
{
using namespace ASM;

/* Lookup HashTables */
using LabelLinker  = std::unordered_map<std::string, int>;  // stores label name and index
using Opcodes      = std::unordered_map<std::string, _OP>;
using Modifiers    = std::unordered_map<std::string, _MOD>;
using AddrModes    = std::unordered_map<char, _AM>;

/// Hashes an opcode string to get the enum value
Opcodes opcode_tbl =
{
    {"dat", _OP::DAT}, {"mov", _OP::MOV}, 
    {"cmp", _OP::CMP}, {"slt", _OP::SLT}, {"spl", _OP::SPL},
    {"add", _OP::ADD}, {"sub", _OP::SUB}, {"mul", _OP::MUL}, {"div", _OP::DIV}, {"mod", _OP::MOD}, 
    {"jmp", _OP::JMP}, {"jmz", _OP::JMZ}, {"jmn", _OP::JMN}, {"djn", _OP::DJN}
};

/// Hashes a modifier string to get the enum value
Modifiers modifier_tbl =
{
    {"a",  _MOD::A},  {"b",  _MOD::B}, 
    {"ab", _MOD::AB}, {"ba", _MOD::BA}, 
    {"f",  _MOD::F},  {"x",  _MOD::X}, 
    {"i",  _MOD::I}
};

/// Hashes an addressing mode string to get the enum value
AddrModes admo_tbl =
{
    {'#', _AM::IMMEDIATE},  {'$', _AM::DIRECT},
    {'*', _AM::INDIRECT_A}, {'@', _AM::INDIRECT_B},
    {'{', _AM::PRE_DEC_A},  {'<', _AM::PRE_DEC_B},
    {'}', _AM::POST_INC_A}, {'>', _AM::POST_INC_B} 
};

/* Utility Functions */

/// Outputs an error for an invalid assembly argument with the full instruction (triggers Exception)
/// @param line full assembly code instruction
/// @param asm_arg asssembly code argument relative to the error
/// @param index line number of assembly instruction
inline void invalidAssemblyError(std::string line, std::string asm_arg, int index);

/// Validates if: char is an assembly instruction argument seperator [ ' ' | '\\t' | ' . ' | ' , ' ]
/// @param val char value to check
/// @return true if char is a seperator
inline bool isAsmSeperator(char val);

/// Takes an assembly instruction string and returns the next valid argument from the position given 
/// @param line assembly instruction string
/// @param pos start position to look for the next assembly argument
/// @return string of the assembly argument
inline std::string findAsmArgument(std::string line, int &pos);

/// Cleans instruction string, removes extra inline spaces/comments & resizes string
/// @param line assembly instruction string
inline std::string cleanAsmStr(std::string line);

/// Gets the Default Modifier for the opcode & addressing modes
/// @param opcode opcode
/// @param admo_a addressing mode A
/// @param admo_b addressing mode B
inline _MOD getDefaultModifier(_OP opcode, _AM admo_a, _AM admo_b);

} //// Anonymous Namespace

/* Parser Functions */

/// Creates a label linker to identify the posistions of all labels within an assembly file
/// @param asm_data a vector containing each line of the assembly file data as a string
LabelLinker findAsmLabels(AssemblyCode &asm_data);

/// Parses an assembly code string to an asm_instr object
/// @param line (asm code) in the format of: <label> [opcode]<.modifier> <mode_a>[op_a],<mode_b>[op_b]
/// @param label_linker used to store and handle labels
/// @param index instruction index (line number) in asm code
Inst asmStrToInst(std::string &line, LabelLinker &linker, int index);

/// Create a warrior object by parsing assembly code  
/// @param warrior_name (file)name of the warrior
/// @param asm_code collection containing the warriors assembly code
/// @param max_warrior_len max instructions a warrior can consist of
Warrior asmCodeToWarrior(std::string warrior_name, AssemblyCode &asm_code, int max_warrior_len);

} //// namespace parser
