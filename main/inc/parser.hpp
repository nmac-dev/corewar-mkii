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

namespace // Anonymous Namespace
{
using namespace ASM;

/* Lookup HashTables */
using LabelLinker  = std::unordered_map<std::string, int>;  // stores label name and index
using Opcodes      = std::unordered_map<std::string, OPCODE>;
using Modifiers    = std::unordered_map<std::string, MOD>;
using AddrModes    = std::unordered_map<char, ADMO>;

/// Hashes an opcode string to get the enum value
Opcodes opcode_tbl =
{
    {"nop", OPCODE::NOP},
    {"dat", OPCODE::DAT}, {"mov", OPCODE::MOV}, 
    {"cmp", OPCODE::SEQ}, {"seq", OPCODE::SEQ}, {"sne", OPCODE::SNE}, {"slt", OPCODE::SLT},
    {"add", OPCODE::ADD}, {"sub", OPCODE::SUB}, {"mul", OPCODE::MUL}, {"div", OPCODE::DIV}, {"mod", OPCODE::MOD}, 
    {"jmp", OPCODE::JMP}, {"jmz", OPCODE::JMZ}, {"jmn", OPCODE::JMN}, {"djn", OPCODE::DJN}, {"spl", OPCODE::SPL}
};

/// Hashes a modifier string to get the enum value
Modifiers mod_tbl =
{
    {"a",  MOD::A},  {"b",  MOD::B}, 
    {"ab", MOD::AB}, {"ba", MOD::BA}, 
    {"f",  MOD::F},  {"x",  MOD::X}, 
    {"i",  MOD::I}
};

/// Hashes an addressing mode string to get the enum value
AddrModes admo_tbl =
{
    {'#', ADMO::IMMEDIATE},  {'$', ADMO::DIRECT},
    {'*', ADMO::INDIRECT_A}, {'@', ADMO::INDIRECT_B},
    {'{', ADMO::PRE_DEC_A},  {'<', ADMO::PRE_DEC_B},
    {'}', ADMO::POST_INC_A}, {'>', ADMO::POST_INC_B} 
};

/* Utility Functions */

/// (dynamic) Returns a stored static string
/// @param cmd operation on stored string: 'G' get | 'S' set | 'C' clear
/// @param str string to store
inline std::string strRegister(char cmd, std::string str = "")
{
    static std::string stored_str;       // holds a single warrior name
    // commands
    switch (cmd)
    {
    case 'S': stored_str = str;   break; // set value 
    case 'C': stored_str.clear(); break; // reset 
    }
    return stored_str;
}

/// Outputs an error for an invalid assembly argument with the full instruction (triggers Exception)
/// @param line full assembly code instruction
/// @param asm_arg asssembly code argument relative to the error
/// @param index line number of assembly instruction
inline void invalidAssemblyError(std::string line, std::string asm_arg, int index)
{
    printf( "\nError: '%s' is not valid argument \n"
            "\tWarrior: '%s' \n"
            "\tLine:[%d]|%s|",
            asm_arg.c_str(),
            strRegister('G').c_str(),
            index, line.c_str());
    throw std::exception(); // begin stack unwind
}

/// Validates if: char is an assembly instruction argument seperator [ ' ' | '\\t' | ' . ' | ' , ' ]
/// @param val char value to check
/// @return true if char is a seperator
inline bool isAsmSeperator(char val)
{
    return val == ' ' || val == '\t' || val == '.' || val == ',';
}

/// Takes an assembly instruction string and returns the next valid argument from the position given 
/// @param line assembly instruction string
/// @param pos start position to look for the next assembly argument
/// @return string of the assembly argument
inline std::string findAsmArgument(std::string line, int &pos)
{
    int len = line.length();
    int begin, end;

    for (begin = pos;  isAsmSeperator(line[begin]) && begin < len; begin++) {} // start of arg
    for (end = begin; !isAsmSeperator(line[end])   &&   end < len; end++  ) {} // end of arg

    pos = end; // retain index value for next argument
    return line.substr(begin, end - begin);
}

} // Anonymous Namespace

/// Cleans instruction string, removes extra inline spaces/comments & resizes string
/// @param line assembly instruction string
std::string cleanAsmStr(std::string line);

/// Gets the Default Modifier for the opcode & addressing modes
/// @param opcode opcode
/// @param admo_a addressing mode A
/// @param admo_b addressing mode B
MOD getDefaultModifier(OPCODE opcode, ADMO admo_a, ADMO admo_b);

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
Warrior *asmCodeToWarrior(std::string warrior_name, AssemblyCode &asm_code, int max_warrior_len);

} // namespace parser
