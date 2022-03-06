/// Defines elements and entities in relation to assembly instructions
#pragma once

namespace ASM
{
/// Assembly Instruction Opcode (specifies operation to perform)
enum class _OP
{
    DAT, // Data: illegal instruction, kills the executing process
    MOV, // Move: (copy) overwrites B with A
    ADD, // Add:      B -> A + B
    SUB, // Subtract: B -> B - A
    MUL, // Multiply: B -> A * B
    DIV, // Divide:   B -> B / A
    MOD, // Modulus:  B -> B % A (remainder of division)
    JMP, // Jump:             queue process execution to address of A (no B value)
    JMZ, // Jump IF Zero:     queue process execution to address of A, if B is 0
    JMN, // Jump IF NOT Zero: queue process execution to address of A, if B is NOT 0
    DJN, // Dec & Jump IF NOT Zero: decrement B, then queue process execution to address of A, if B is NOT 0
    CMP, // Skip IF Equal: if A is equal to B, skip the next instruction
    SLT, // Skip if Less Than: if A is less than B, skip the next instruction
    SPL  // Split: creates a new process at the address of A (unless processes are at max)
};

/// Assembly Instruction Modifier (modifies opcode behaviour)
enum class _MOD
{
    A,    // A -> A
    B,    // B -> B
    AB,   // A -> B
    BA,   // B -> B
    F,    // A -> A & B -> B 
    X,    // A -> B & B -> A
    I     // [default] Opcode is applied to both operands (MOV, CMP, SNE)
};

/// Assembly Instruction Addressing Mode (determines source and destination)
enum class _AM
{
    IMMEDIATE,  // "#" evaluated as of 0
    DIRECT,     // "$" [default] relative to the current cell 
    A_INDIRECT, // "*" A becomes a pointer to a cell relative from its position
    B_INDIRECT, // "@" B becomes a pointer to a cell relative from its position
    A_PRE_DEC,  // "{" (indirect) A is incremented after use
    A_POST_INC, // "}" (indirect) A is incremented before use
    B_PRE_DEC,  // "<" (indirect) B is incremented after use
    B_PRE_INC   // ">" (indirect) B is incremented before use
};

/// Holds label information to be parsed
struct Label
{
    Instr &warrior;  // warrior label belongs too
    const char &name;  // <label> name
    int position;      // label position (relative to first warrior asm instruction)
};

/// Stores an assembly instruction [opcode].[modifier] [mode_a][op_a], [mode_b][op_b]
struct Instr
{
    _OP  *opcode;     // Specifies operation
    _MOD *modifier;   // Modifies opcode behaviour
    _AM  *mode_a, *mode_b; // Addressing mode for operands (A|B)
    int  op_a, op_b;       // Operand (A|B) of the opcode argument
    
    Instr();
    ~Instr();
};

/// Represents a warrior (player) containing assembly code instruction
class Warrior
{
 private:
    int len;        // length of the warrior (lines of asm instructions) 
    int core_index; // location of the first warrior instruction withing the core
    Instr *instructions; // array containing all the warriors assembly instructions
    /// TODO:
    // std::string labels[];  // labels to be tracked
    // Store array of ints to indicate label posistion

    void loadWarrior(const char *filename); // loads and processes the warrior file
 public:
    Warrior();
    ~Warrior();

    /// returns the start index of the warrior within the core
    inline int Warrior::begin() const {return core_index;}
    /// returns the end index of the warrior within the core (core_index + length -1)
    inline int Warrior::end() const {return core_index + len;}
    
};
} // namespace ASM
