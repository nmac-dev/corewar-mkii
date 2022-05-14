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
using WarriorVec  = std::vector<UniqWarrior>;
using InstVec     = std::vector<Inst>;

/// Assembly Instruction Opcode (specifies operation to perform)
enum class Opcode
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
enum class OpcodeType
{
    SYSTEM,         // NOP, DAT, MOV, SPL
    COMPARISION,    // SEQ, SNE, SLT
    ARITHMETIC,     // ADD, SUB, MUL, DIV, MOD
    JUMP            // JMP, JMZ, JMN, DJN
};

/// Assembly Instruction Modifier: determines opcode behaviour for source and destination targets
enum class Modifier
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
enum class ModifierType
{
    SINGLE, // A, B, AB, BA
    DOUBLE, // F, X
    FULL    // I            Only:(MOV, SEQ, SNE)
};

/// Assembly Instruction Addressing Mode (determines source and destination)
enum class Admo
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
enum class InstField { A, B };

/// ArgType: operation code and modifier argument types
struct ArgType {
    OpcodeType   code;  // opcode   operation
    ModifierType mod;   // modifier argument
};

/// Stores an assembly instruction [opcode].[modifier] [mode_a][op_a], [mode_b][op_b]
struct Inst
{
    struct Operation {  // Operation (code) to apply to the operand
        Opcode code;    // Specifies opcode operation
        Modifier    mod;     // Modifies  opcode behaviour
    };
    struct Operand {    // Operand (OPR) value to be used with operation
        Admo admo;      // Addressing mode
        int  val;       // Operand Value
    };
    Operation OP;  // opcode with modifier
    Operand A, B;  // operand addressing mode and value 

    /// DEFAULT: 'DAT.F #0, #0'
    Inst();    
    /// CUSTOM: OP:{ [code]<mod> } A:{ <admo>[val] } B:{ <admo>[val] }
    /// @param _OP operation to apply to the operand
    /// @param _A  A value to be used with operation
    /// @param _B  B value
    Inst(Operation _OP, Operand _A, Operand _B);

    /// Returns default modifier for the opcode & addressing modes
    /// @param _code opcode
    /// @param _A addressing mode A
    /// @param _B addressing mode B
    static Modifier find_default_mod(Opcode _code, Admo _A, Admo _B);
    /// returns a string of the assembly instruction as assembly code
    std::string to_assembly();

};  /* Inst */

/// Represents a warrior (player) containing assembly code instruction
class Warrior
{
 private:
    inline static int ini_max_warrior_len; // max instructions a warrior can consist of (.ini)

    std::string m_name;         // warrior's name (filename)
    int         m_uuid;         // universally unique warrior identifier
    int         m_length;       // length of the warrior (lines of instructions) 
    int         m_address;   // location of the first warrior instruction in the core (late init) 
    InstVec    m_insts;        // contains all the warriors instructions

    // On each call a unique number is created as a new warrior ID
    inline static int create_uuid() 
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

    /// returns the start address of the warrior within the core
    inline int begin() const { return m_address; }
    /// returns the end address of the warrior within the core (address + length -1)
    inline int end()   const { return m_address + m_length; }

    /// Returns the name (file) of the warrior
    inline std::string name() const { return m_name; }
    /// Returns the warriors posistion within the core
    inline int address()        const { return m_address; }
    /// Returns length (number of instructions) of the warrior
    inline int len()          const { return m_length; }
    /// Returns the UUID to identify the warrior
    inline int uuid()         const { return m_uuid; }

    /// Adds an instruction to the warriors collection
    /// @param _inst instruction object to add
    void push(Inst _inst);
    /// Sets the warriors posistion within the core
    inline void set_address(int val) { m_address = val; }

    /// Access address of warrior's instruction array
    Inst  operator[](int address) const;
    /// Modify address of warrior's instruction array
    Inst &operator[](int address);

}; /* Warrior */

} /* ::ASM */
