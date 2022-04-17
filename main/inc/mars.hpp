/// Memory array assembly simulator containts array of instruction objects and decoding functions
#pragma once

// #define MARS_DEBUG

#include <stdint.h>
#include <time.h>
#include "asm.hpp"

namespace OS
{

// Memory Array Redcode Simulator (MARS): handles stored assembly instruction objects
class MARS
{
 private:
    // settings (.ini) value
    int core_size,
        min_seperation;

    // memory array of assembly instructions
    ASM::Inst *RAM;
    
 public:
    /// Defines event types commited by the core
    enum class Event
    {
        NOOP,
        READ,
        WRITE,
    };
    /// Logs the operations performed over the src & dest operands
    struct CommitLog
    {
        int ir_pc;              // program counter index executing the instruction register
        ASM::_OP opcode;

        int adr_A, adr_B;       // relative address   for A|B
        int *src_A,  *src_B,    // source operand      of A|B
            *dest_A, *dest_B;   // destination operand of A|B
        int pip_a, pip_b;       // post-increment value pointer A|B
        int pdp_a, pdp_b;       // pre-decrement  value pointer A|B

        Event event_src,        // event applied to the adr_A
            event_dest;         // event applied to the adr_B
        
        /// Creates a commit log with the initial program counter index (pre-configured for DAT opcode)
        CommitLog(int _i_pc);
        CommitLog();
    };

 /* functions */
 private:
    /// Generates a random positive integer using the process clock and xorshift algorithm
    /// @param maxRange output will not exceed this value
    uint32_t randomInt(uint32_t maxRange);

    /// Loops the index when outside the bounds of the memory array
    /// @param index memory array index
    inline int loopIndex(int index)
    {
        // loop index until it is within the 
        while (index < 0 || index > core_size)
        {
            // index to upper core section
            if (index < 0)  index += core_size;
            // index to lower core (> core_size)
            else index -= core_size;
        }
        return index;
    }

    /// Returns a pointer to either operand A or B for the index given
    /// @param index instruction index within the MARS
    /// @param select 0 selects A, else B
    inline int *selectOperand(int index, int select)
    {
        return select == 0 ? &RAM[index].operand_a : &RAM[index].operand_b;
    }
    
    /// Decodes the addressing mode for the input instruction, buffers the commit log
    /// @param cl commit log; used as a buffer for the decoded results
    /// @param op_select 0 selects operand A, else B
    void decodeAdmo(CommitLog *cl, int op_select);
 
 public:
    /// Initialises the simulator by loading a default asm instruction (dat #0, #0) into every address,
    /// then places each warrior at a random location in accordance with the 'min_seperation' setting
    /// @param warrior_list collection of warriors to be loaded into the core
    /// @param _size       (core_size) of the simulator
    /// @param _seperation (min_seperation) minimum seperation between warriors in the simulator
    MARS(ASM::WarriorList &warriors, int _size, int _seperation);
    MARS();
    ~MARS();

    /// Returns the next commit to be made on the simulator
    /// @param ir_pc instruction register program counter
    CommitLog nextCommit(int ir_pc);

    /// Returns the size of the simulator
    inline int size() const { return core_size; }

    /// access index of core simulators's memory array
    ASM::Inst &operator[](int index) const;
    /// modify index of core simulators's memory array
    ASM::Inst &operator[](int index);
};

} //// namespace OS
