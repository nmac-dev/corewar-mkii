/// Defines elements and entities in relation to assembly instructions
#pragma once

#include <memory>
#include <vector>
#include <string>
#include "assembly_enums.hpp"
#include "uuid.hpp"

namespace ASM
{
class Warrior;
struct Inst;

using UniqWarrior = std::unique_ptr<Warrior>;
using WarriorVec  = std::vector<UniqWarrior>;
using InstVec     = std::vector<Inst>;

/// ArgType: operation code and modifier argument types
struct ArgType {
    OpcodeType   code;  // opcode   operation
    ModifierType mod;   // modifier argument
};

/// Stores an assembly instruction [opcode].[modifier] [mode_a][op_a], [mode_b][op_b]
struct Inst
{
    struct Operation {  // Operation (code) to apply to the operand
        Opcode   code;  // Specifies opcode operation
        Modifier mod;   // Modifies  opcode behaviour
    };
    struct Operand {    // Operand (OPR) value to be used with operation
        Admo admo;      // Addressing mode
        int  val;       // Operand Value
    };
    Operation OP;       // opcode with modifier
    Operand A, B;       // operand addressing mode and value 

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
    int ini_max_warrior_inst;   // max insts a warrior can consist of

    std::string m_name;         // warrior's name (filename)
    UUID m_uuid;                // universally unique identifier
    int  m_length,              // length of the warrior (lines of instructions) 
         m_address;             // location of the first warrior instruction in the core (late init) 
    InstVec m_insts;            // contains all the warriors instructions

 public:
    /// Creates a warrior
    /// @param _name   warrior's name (filename)
    /// @param _length number (lines) of asm instructions
    /// @param _max_warrior_len [static] max length of a warrior (corewar.ini) 
    Warrior(std::string _name, const int _length, int _max_warrior_len);

    /// Adds an instruction to the warriors collection
    /// @param _inst instruction object to add
    void push(Inst _inst);

 /* Utility */

    /// Returns the name (file) of the warrior
    inline std::string name() const { return m_name; }
    
    /// Returns the UUID to identify the warrior
    inline UUID uuid() const { return m_uuid; }

    /// Returns length (number of instructions) of the warrior
    inline int len() const { return m_length; }

    /// Returns the warriors posistion within the core
    inline int address() const { return m_address; }

    /// Sets the warriors posistion within the core
    inline void set_address(int val) { m_address = val; }

    /// returns the start address of the warrior within the core
    inline int begin() const { return m_address; }

    /// returns the end address of the warrior within the core (address + length -1)
    inline int end() const { return m_address + m_length; }



    /// Access address of warrior's instruction array
    Inst  operator[](int address) const;
    /// Modify address of warrior's instruction array
    Inst &operator[](int address);

}; /* Warrior */

} /* ::ASM */
