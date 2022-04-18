/// Defines elements and entities in relation to assembly instructions
#pragma once

#include <memory>
#include <vector>
#include <string>

namespace ASM
{
class Warrior;
class Inst;
using WarriorList = std::vector<Warrior>;
using InstList    = std::vector<Inst>;

/// Assembly Instruction Opcode (specifies operation to perform)
enum class _OP
{
    /* Read/Write */
    DAT, // Data: illegal instruction, kills the executing process
    MOV, // Move: (copy) overwrites B with A

    /* Comparision */
    CMP, // Skip IF Equal: if A is equal to B, skip the next instruction
    SLT, // Skip if Less Than: if A is less than B, skip the next instruction
    SPL, // Split: creates a new process at the address of A (unless processes are at max)

    /* Arithmetic */
    ADD, // Add:      B -> A + B
    SUB, // Subtract: B -> B - A
    MUL, // Multiply: B -> A * B
    DIV, // Divide:   B -> B / A
    MOD, // Modulus:  B -> B % A (remainder of division)

    /* Jump */
    JMP, // Jump:             set program counter to address of A (no B value)
    JMZ, // Jump IF Zero:     set program counter to address of A, if B is 0
    JMN, // Jump IF NOT Zero: set program counter to address of A, if B is NOT 0
    DJN, // Dec & Jump IF NOT Zero: decrement B, then set program counter to address of A, if B is NOT 0
};

/// Types of opcode arguments 
enum class _OP_TYPE
{
    READWRITE,  // DAT, MOV
    COMPARE,    // CMP, SLT, SPL
    ARITHMETIC, // ADD, SUB, MUL, DIV, MOD
    JUMP        // JMP, JMZ, JMN, DJN
};

/// Assembly Instruction Modifier: determines opcode behaviour for source and destination targets
enum class _MOD
{
    A,  // (src) A -> A (dest)
    B,  // (src) B -> B (dest)
    AB, // (src) A -> B (dest)
    BA, // (src) B -> A (dest)
    F,  // (src) A,B -> A,B (dest) 
    X,  // (src) A,B -> B,A (dest)
    I   // [default] (src) Instruction -> Instruction (dest) 
};

/// Types of modifier arguments 
enum class _MOD_TYPE
{
    SINGLE, // A, B, AB, BA
    DOUBLE, // F, X
    FULL    // I
};

/// Assembly Instruction Addressing Mode (determines source and destination)
enum class _AM
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

/// Stores an assembly instruction [opcode].[modifier] [mode_a][op_a], [mode_b][op_b]
struct Inst
{
    _OP  opcode;               // Specifies operation
    _MOD modifier;             // Modifies opcode behaviour
    _AM  admo_a, admo_b;       // Addressing mode for operands (A|B)
    int  operand_a, operand_b; // Operand (A|B) of the opcode argument
    
    /// Constructs a custom Instuction: [op]<mod> <am_a>[o_a], <am_b>[o_b]
    /// @param op    opcode
    /// @param mod   modifier
    /// @param am_a  addressing mode A
    /// @param o_a   operand A
    /// @param am_b  addressing mode B
    /// @param o_b   operand B
    Inst(_OP op, _MOD mod, _AM am_a, int o_a, _AM am_b, int o_b);
    /// Constructs a default Instuction: DAT.F $0, $0
    Inst();
};

/// Represents a warrior (player) containing assembly code instruction
class Warrior
{
 private:
    std::string name;       // warrior's name (filename)
    int  uuid;              // universally unique warrior identifier
    int  length;            // length of the warrior (lines of instructions) 
    int  core_index;        // location of the first warrior instruction in the core (late init) 
    InstList instructions;  // contains all the warriors instructions

    // On each call a unique number is created as a new warrior ID
    inline static int createUUID() 
    {
        static int unique_number;
        return   ++unique_number;
    }

 public:
    /// Constructs a warrior
    /// @param _name   warrior's name (filename)
    /// @param _length number (lines) of asm instructions 
    Warrior(std::string _name, const int _length);
    // ~Warrior();

    /// returns the start index of the warrior within the core
    inline int begin() const { return core_index; }
    /// returns the end index of the warrior within the core (core_index + length -1)
    inline int end()   const { return core_index + length; }

    /// Returns the name (file) of the warrior
    inline std::string getName() const { return name; }
    /// Returns the warriors posistion within the core
    inline int getCoreIndex()    const { return core_index; }
    /// Returns length (number of instructions) of the warrior
    inline int len()             const { return length; }
    /// Returns the UUID to identify the warrior
    inline int getUUID()         const { return uuid; }

    // Sets the warriors posistion within the core
    inline void setCoreIndex(int val) { core_index = val; }

    /// Access index of warrior's instruction array
    Inst  operator[](int index) const;
    /// Modify index of warrior's instruction array
    Inst &operator[](int index);
};

} // namespace ASM
