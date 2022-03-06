/// Parses assembly instructions from a warrior file into a warrior class 
#pragma once

#include <string>
#include <unordered_map>

#include "file_loader.hpp"
#include "asm.hpp"

namespace parser
{
using namespace ASM;

/// Hashes an opcode string to its enum
static const std::unordered_map<std::string, _OP> opcode_tbl =
{
    {"dat", _OP::DAT}, {"mov", _OP::MOV}, {"add", _OP::ADD}, {"sub", _OP::SUB}, 
    {"mul", _OP::MUL}, {"div", _OP::DIV}, {"mod", _OP::MOD}, {"jmp", _OP::JMP}, 
    {"jmz", _OP::JMZ}, {"jmn", _OP::JMN}, {"djn", _OP::DJN}, {"cmp", _OP::CMP},  
    {"slt", _OP::SLT}, {"spl", _OP::SPL}
};

/// Hashes a modifier string to its enum
static const std::unordered_map<std::string, _MOD> modifier_tbl =
{
    {"a", _MOD::A}, {"b", _MOD::B}, {"ab", _MOD::AB}, {"ba", _MOD::BA}, 
    {"f", _MOD::F}, {"x", _MOD::X}, {"i",  _MOD::I}
};

/// Hashes a addressing mode string to its enum
static const std::unordered_map<char, _AM> adr_mode_tbl =
{
    {'#', _AM::IMMEDIATE}, {'$', _AM::DIRECT},     {'*', _AM::A_INDIRECT}, {'@', _AM::B_INDIRECT}, 
    {'{', _AM::A_PRE_DEC}, {'}', _AM::A_POST_INC}, {'<', _AM::B_PRE_DEC},  {'>', _AM::B_PRE_INC}
};

/** Validates if: char is a number ('0'->'9')
 * @param val char value to check
 * @return true if char is a number
 */
inline static bool isNumber(char& val);

/** Validates if: char is an assembly instruction argument seperator [ ' ' | '\\t' | ' . ' | ' , ' ]
 * @param val char value to check
 * @return true if char is a number
 */
inline static bool isAsmSeperator(char& alv);

/** Cleans instruction string (removes extra inline spaces & resizes string)
 * @param str assembly instruction as a string
 */
static void cleanAsmStr(std::string &str);

/*************************************
 * TODO: strToInstr(), loadWarrior()
**************************************/

/** Parses an assembly code string to an asm_instr object
 * @param str in the format of: <label> [opcode]<.modifier> <mode_a>[op_a],<mode_b>[op_b]
 * @param label_linker used to store and handle labels
 * @return Asm Instruction object
 */
static Instr *strToInstr(std::string &str, std::unordered_map<std::string, Label> &label_linker);

/// Loads a warrior from a .asm file
static Warrior *loadWarrior(const char *filename);
} /// namespace parser
