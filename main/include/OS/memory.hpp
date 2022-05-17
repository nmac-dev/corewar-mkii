/// Memory using circular RAM with assembly instruction objects and decoding functions
#pragma once

// #define MEMORY_DEBUG

#include <stdint.h>
#include <time.h>
#include "assembly.hpp"
#include "ctrl_unit.hpp"
#include "template/c_ram.hpp"

namespace OS
{
/// Memory Array Redcode Simulator (Memory): handles assembly instruction objects within RAM
class Memory
{
 private:
    static int constexpr ram_size = 8000;   // number of memory addresses within the core 
    int ini_min_seperation;                 // min distance between programs at the start of a round (config.ini) 

    C_RAM<Inst> RAM;                        // Array of instruction objects (circular)

 public:
    /// Initialises the simulator by loading a default asm instruction (dat #0, #0) into every address,
    /// then places each program at a random location in accordance with the 'min_seperation' setting
    /// @param _programs collection of programs to be loaded into the core
    /// @param _min_seperation minimum seperation between programs in the simulator
    Memory(ProgramVec *_programs, int _min_seperation);
    Memory();
    
 /* Decode */

    /// Decodes the addressing mode for the input instruction
    /// @param _ctrl buffer for the decoded results
    /// @param exe_select selects operand A or B
    Register decode_admo(ControlUnit *_ctrl, InstField const exe_select);

    /// Decodes the modifier for the input instruction
    /// @param _ctrl buffer for the decoded results
    void decode_modifier(ControlUnit * _ctrl);
    /// Returns a control unit which has decoded the addressing modes and modifier
    /// @param _pc instruction register program counter to decode
    ControlUnit generate_ctrl(int const _pc);

    /// Applies the post-increment if the pointer target exists 
    /// @param _ctrl control unit containing Registers to be incremented
    inline void apply_post_inc(ControlUnit & _ctrl)
    {
        if (_ctrl.post_A != nullptr)
            (*_ctrl.post_A)++;

        if (_ctrl.post_B != nullptr)
            (*_ctrl.post_B)++;
    }

 /* Utility */

    /// Returns the size of the simulator
    static int constexpr size() { return ram_size; }

    /// access address of core simulators's memory array
    Inst &operator[](int address) const;
    /// modify address of core simulators's memory array
    Inst &operator[](int address);

 private:
    /// Generates a random positive integer using the process clock and xorshift algorithm
    /// @param _max_range output will not exceed this value
    uint32_t random_int(uint32_t max_range);

    /// Returns a pointer to either operand A or B values for the address given
    /// @param address instruction address within the Memory
    /// @param select selects operand A or B
    inline int *fetch_operand(int address, InstField select)
    {
        static int constexpr max_itr = 8,                       // max RAM address loop iterations
                             itr_limiter = ram_size * max_itr; // adjusts value over limit 

        int *operand = select == InstField::A ? &RAM[address]->A.val    // A
                                              : &RAM[address]->B.val;   // B

        // loop value to limiter bounds: stops Integer overflow,
        // and increases RAM indexing performance
        while (*operand < -(itr_limiter) ) *operand += itr_limiter;    // lower bound
        while (*operand >   itr_limiter  ) *operand -= itr_limiter;    // upper bound

        return operand;
    }


}; /* ::Memory */
} /* ::OS */
