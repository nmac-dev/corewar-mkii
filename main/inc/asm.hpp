/// Defines elements and entities in relation to assembly instructions
#pragma once

#include <string>

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
    I     // [default] Opcode is applied to both operands
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

/// Stores an assembly instruction [opcode].[modifier] [mode_a][op_a], [mode_b][op_b]
struct Inst
{
    _OP  opcode;               // Specifies operation
    _MOD modifier;             // Modifies opcode behaviour
    _AM  admo_a, admo_b;       // Addressing mode for operands (A|B)
    int  operand_a, operand_b; // Operand (A|B) of the opcode argument
    
    /// Constructs a default Instuction: DAT.F $0, $0
    Inst();
    /// Constructs a custom Instuction: [op]<mod> <am_a>[o_a], <am_b>[o_b]
    /// @param op    opcode
    /// @param mod   modifier
    /// @param am_a  addressing mode A
    /// @param o_a   operand A
    /// @param am_b  addressing mode B
    /// @param o_b   operand B
    Inst(_OP op, _MOD mod, _AM am_a, int o_a, _AM am_b, int o_b);
};

/// Represents a warrior (player) containing assembly code instruction
class Warrior
{
 private:
    std::string name;   // warrior's name (filename)
    int  uuid;          // universally unique warrior identifier
    int  length;        // length of the warrior (lines of asm instructions) 
    int  core_index;    // location of the first warrior instruction in the core (late init) 
    Inst *instructions; // array containing all the warriors asm instructions

    // On each call a unique number is created as a new warrior ID
    inline static int createUUID() 
    {
        static int unique_number;
        return unique_number++;
    }

 public:
    /// Constructs a warrior
    /// @param warrior_name warrior's name (filename)
    /// @param inst_array   array of all the warriors asm instructions
    /// @param n_inst       number (lines) of asm instructions 
    Warrior(std::string warrior_name, Inst *inst_array, int n_inst);
    ~Warrior();

    /// returns the start index of the warrior within the core
    inline int begin() const { return core_index; }
    /// returns the end index of the warrior within the core (core_index + length -1)
    inline int end()   const { return core_index + length; }

    /// Returns the name (file) of the warrior
    inline std::string getName() const { return name; }
    /// Returns length (number of instructions) of the warrior
    inline int len()             const { return length; }
    /// Returns the UUID to identify the warrior
    inline int getUUID()         const { return uuid; }

    /// access index of warrior's instruction array
    Inst  operator[](int index) const;
    /// modify index of warrior's instruction array
    Inst &operator[](int index);
};

} // namespace ASM
