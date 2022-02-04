/// asm_inst.hpp

#include <cstdint>

/// Asm Instruction Opcode (specifies operation to perform)
enum Opcode
{
    DAT,    // Data: Illegal instruction, kills the executing warriors executing process
    MOV,    // Move: Copies data from operand A (source) to B (destination)
    ADD,    // Add:  Adds the number from the address of operand A to address of B
    SUB,    // Subtract: Subtracts the number from the address of operand A to address of B
    JMP     // Jump: jumps process execution to the specified address
};

/// Asm Instruction Modifier (modifies opcode behaviour)
enum Modifier
{
    NONE,
    A,  //
    B,  //
    AB, //
    BA, //
    F,  //
    X,  // 
    I   // Opcode is applied to both operands [MOV, SEQ, SNE]
};

/// Asm Instruction Addressing Mode (determines source and destination)
enum AddrMode
{
    NONE,
    IMMEDIATE,
    DIRECT,
    A_INDIR,
    B_INDIR,
    A_PRE_DEC,
    A_POST_INC,
    B_PRE_DEC
};

/// AsmInst: a struct for storing an assembly instruction
struct AsmInst
{
    Opcode     *opcode;
    Modifier   *modifier;
    AddrMode   *addrmode_a,
               *addrmode_b;   
    const char *operand_a,    // Operand A of the opcode argument
               *operand_b;    // Operand B of the opcode argument

    AsmInst(const char *asm_inst);
    ~AsmInst();
};