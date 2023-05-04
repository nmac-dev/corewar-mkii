/// Parses assembly instructions from a program file into a program class
#pragma once

// #define ASM_PARSER_DEBUG

#define ASSEMBLY_COMMENT ';' // assembly code comment character

#include <unordered_map>
#include "assembly.hpp"

namespace Parser
{
using AssemblyCode = std::vector<std::string>;

namespace /* {anonymous} */
{
    using namespace Asm;

/* Lookup HashTables */
using LabelLinker  = std::unordered_map<std::string, int>;  // stores label name and address
using Opcodes      = std::unordered_map<std::string, Opcode>;
using Modifiers    = std::unordered_map<std::string, Modifier>;
using AddrModes    = std::unordered_map<char, Admo>;

/// Hashes an opcode string to get the enum value
Opcodes opcode_tbl =
{
    {"nop", Opcode::NOP}, {"dat", Opcode::DAT}, {"mov", Opcode::MOV},
    {"cmp", Opcode::SEQ}, {"seq", Opcode::SEQ}, {"sne", Opcode::SNE}, {"slt", Opcode::SLT},
    {"add", Opcode::ADD}, {"sub", Opcode::SUB}, {"mul", Opcode::MUL}, {"div", Opcode::DIV}, {"mod", Opcode::MOD},
    {"jmp", Opcode::JMP}, {"jmz", Opcode::JMZ}, {"jmn", Opcode::JMN}, {"djn", Opcode::DJN}, {"spl", Opcode::SPL}
};

/// Hashes a modifier string to get the enum value
Modifiers mod_tbl =
{
    {"a",  Modifier::A},  {"b",  Modifier::B},
    {"ab", Modifier::AB}, {"ba", Modifier::BA},
    {"f",  Modifier::F},  {"x",  Modifier::X},
    {"i",  Modifier::I}
};

/// Hashes an addressing mode string to get the enum value
AddrModes admo_tbl =
{
    {'#', Admo::IMMEDIATE},  {'$', Admo::DIRECT},
    {'*', Admo::INDIRECT_A}, {'@', Admo::INDIRECT_B},
    {'{', Admo::PRE_DEC_A},  {'<', Admo::PRE_DEC_B},
    {'}', Admo::POST_INC_A}, {'>', Admo::POST_INC_B}
};

/* Utility Functions */

/// Outputs an error for an invalid assembly argument with the full instruction (triggers Exception)
/// @param line full assembly code instruction
/// @param asm_arg asssembly code argument relative to the error
/// @param address line number of assembly instruction
inline void invalid_assembly(int _index, std::string _line, std::string _err)
{
    printf( "\nError: '%s' is not valid argument \n"
            "\tLine:[%d]|%s|",
            _err.c_str(),
            _index,
            _line.c_str()
    );
    throw std::exception(); // begin stack unwind
}

/// Returns true if the test value is between the upper and lower bounds
/// @param val   value to be tested
/// @param begin lower bound
/// @param end   upper bound
template<typename T>
inline bool is_between(T val, T begin, T end)
{
    return val >= begin
        && val <= end;
}

/// Validates if: char is an assembly instruction argument seperator [ ' ' | '\\t' | ' . ' | ' , ' ]
/// @param val char value to check
/// @return true if char is a seperator
inline bool is_seperator(char val)
{
    return val == ' ' || val == '\t' || val == '.' || val == ',';
}

/// Takes an assembly instruction string and returns the next valid argument from the position given
/// @param line assembly instruction string
/// @param pos start position to look for the next assembly argument
/// @return string of the assembly argument
inline std::string find_argument(std::string line, int &pos)
{
    int len = line.length();
    int begin, end;

    for (begin = pos;  is_seperator(line[begin]) && begin < len; begin++) {} // start of arg
    for (end = begin; !is_seperator(line[end])   &&   end < len; end++  ) {} // end of arg

    pos = end; // retain address value for next argument
    return line.substr(begin, end - begin);
}

} /* ::{anonymous} */

/// Cleans instruction string, removes extra inline spaces/comments & resizes string
/// @param line assembly instruction string
std::string clean_assembly(std::string line);

/// Creates a label linker to identify the posistions of all labels within an assembly file
/// @param _assembly Labels start with ( a-z, A-Z, '_' ) and can contain 0-9 plus a ':' at the end
LabelLinker generate_label_linker(AssemblyCode &_assembly);

/// Parses an assembly code string to an object
/// @param line (code) in the format of: <label> [opcode]<.modifier> <mode_a>[op_a],<mode_b>[op_b]
/// @param label_linker used to store and handle labels
/// @param address instruction address (line number) in asm code
Inst assembly_to_inst(std::string &_line, LabelLinker &_linker, int _index);

/// Create a program object by parsing assembly code
/// @param _program_name filename of the program
/// @param _assembly collection containing the programs assembly code
/// @param max_program_insts max instructions a program can consist of
Program *create_program(std::string program_name, AssemblyCode &_assembly, int _max_program_insts);

} /* ::Parser */
