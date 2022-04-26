/// Defines elements and entities in relation to assembly instructions
#pragma once

#include <memory>
#include <vector>
#include <string>

namespace ASM
{
class Warrior;
struct Inst;
using UniqWarrior = std::unique_ptr<Warrior>;
using WarriorList = std::vector<UniqWarrior>;
using InstList    = std::vector<Inst>;

/// Assembly Instruction Opcode (specifies operation to perform)
enum class OPCODE
{
    NOP, // Literally does nothing
    
    /* Read/Write */
    DAT, // Data: illegal instruction, kills the executing process
    MOV, // Move: (copy) overwrites B with A

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
    SPL, // Split: creates a new process at the address of A (unless processes are at max)
};

/// Types of opcode arguments 
enum class OPCODE_TYPE
{
    READWRITE,      // DAT, MOV
    COMPARISION,    // SEQ, SNE, SLT
    ARITHMETIC,     // ADD, SUB, MUL, DIV, MOD
    JUMP            // JMP, JMZ, JMN, DJN, SPL
};

/// Assembly Instruction Modifier: determines opcode behaviour for source and destination targets
enum class MOD
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
enum class MOD_TYPE
{
    SINGLE, // A, B, AB, BA
    DOUBLE, // F, X
    FULL    // I            Only:(MOV, SEQ, SNE)
};

/// Assembly Instruction Addressing Mode (determines source and destination)
enum class ADMO
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
    struct Operation    // Operation to apply to the operand
    {
        OPCODE code;    // Specifies opcode operation
        MOD    mod;     // Modifies  opcode behaviour
        Operation();
    };
    struct Operand      // value to be used with operation
    {
        ADMO admo;      // Addressing mode
        int  val;       // Operand Value
        Operand();
    };
    Operation OP;  // opcode with modifier
    Operand A, B;  // operand addressing mode and value 
    
    /// Constructs a custom Instuction: OP:[code]<mod> A:<admo>[val], B:<admo>[val]
    /// @param _OP operation to apply to the operand
    /// @param _A  A value to be used with operation
    /// @param _B  B value
    Inst(Operation _OP, Operand _A, Operand _B);
    /// Constructs a default Instuction: DAT.F $0, $0
    Inst();
    ~Inst();

    /// returns a string of the assembly instruction as assembly code
    std::string toAsmCode();
};

/// Represents a warrior (player) containing assembly code instruction
class Warrior
{
 private:
    inline static int max_warrior_len; // max instructions a warrior can consist of

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
    /// Creates a warrior
    /// @param _name   warrior's name (filename)
    /// @param _length number (lines) of asm instructions
    /// @param _max_warrior_len [static] max length of a warrior (corewar.ini) 
    Warrior(std::string _name, const int _length, int _max_warrior_len);
    ~Warrior();

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

    /// Adds an instruction to the warriors collection
    /// @param _inst instruction object to add
    void push(Inst _inst);
    /// Sets the warriors posistion within the core
    inline void setCoreIndex(int val) { core_index = val; }

    /// Access index of warrior's instruction array
    Inst  operator[](int index) const;
    /// Modify index of warrior's instruction array
    Inst &operator[](int index);
};

} // namespace ASM
