/// Collection of enumerations representing assembly code
#pragma once

namespace Asm
{
/// Assembly Instruction Opcode (specifies operation to perform)
enum class Opcode : int
{
    /* System */
    NOP, // Literally does nothing
    DAT, // Data: illegal instruction, kills the executing process
    MOV, // Move: (copy) overwrites B with A
    SPL, // Split: creates a new process at the address of A (unless processes are at max)

    /* Comparision */
    SEQ, // Skip IF Equal: if A is equal to B, skip the next instruction
    SNE, // Skip IF NOT Equal: if A is NOT equal to B, skip the next instruction
    SLT, // Skip if Less Than: if A is less than B, skip the next instruction

    /* Arithmetic */
    ADD, // Add:      B -> A + B
    SUB, // Subtract: B -> B - A
    MUL, // Multiply: B -> A * B
    DIV, // Divide:   B -> B / A (division by zero is illegal, thus kills the process)
    MOD, // Modulus:  B -> B % A 

    /* Jump */
    JMP, // Jump:             set program counter to address of A (no B value)
    JMZ, // Jump IF Zero:     set program counter to address of A, if B is 0
    JMN, // Jump IF NOT Zero: set program counter to address of A, if B is NOT 0
    DJN, // Dec & Jump IF NOT Zero: decrement B, then set program counter to address of A, if B is NOT 0
};

/// Types of opcode arguments 
enum class OpcodeType : int
{
    SYSTEM,         // NOP, DAT, MOV, SPL
    COMPARISION,    // SEQ, SNE, SLT
    ARITHMETIC,     // ADD, SUB, MUL, DIV, MOD
    JUMP            // JMP, JMZ, JMN, DJN
};

/// Assembly Instruction Modifier: determines opcode behaviour for source and destination targets
enum class Modifier : int
{
    A,  // (src) A -> A (dest)
    B,  // (src) B -> B (dest)
    AB, // (src) A -> B (dest)
    BA, // (src) B -> A (dest)
    F,  // (src) A,B -> A,B (dest) 
    X,  // (src) A,B -> B,A (dest)
    I   // (src) Instruction -> Instruction (dest) Only:(MOV, SEQ, SNE) else F 
};

/// Types of modifier arguments 
enum class ModifierType : int
{
    SINGLE, // A, B, AB, BA
    DOUBLE, // F, X
    FULL    // I            Only:(MOV, SEQ, SNE)
};

/// Assembly Instruction Addressing Mode (determines source and destination)
enum class Admo : int
{
    IMMEDIATE,  // "#" stores an immediate value (address evaluated as 0)
    DIRECT,     // "$" [default] relative from the program counter

    /* Indirect */
    INDIRECT_A, // "*" A becomes a pointer to a cell relative the program counter
    INDIRECT_B, // "@" B becomes a pointer to a cell relative the program counter
    
    /* Pre/Dec & Post/Inc */
    PRE_DEC_A,  // "{" (indirect) A is decremented before use
    PRE_DEC_B,  // "<" (indirect) B is decremented before use
    POST_INC_A, // "}" (indirect) A is incremented after use
    POST_INC_B, // ">" (indirect) B is incremented after use
};

/// Instruction operand field ( [A], [B] )
enum class InstField : int 
{ 
    A, // Inst Filed [A]
    B  // Inst Filed [B]
};

} /* ::Asm */
