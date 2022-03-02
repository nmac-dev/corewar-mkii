/// Defines elements and entities in relation to assembly instructions

#include "asm.hpp"

namespace ASM
{
Instr::Instr()  {}
Instr::~Instr() {}

Warrior::Warrior()  {}
Warrior::~Warrior() {}

inline int Warrior::begin() const {return core_index;}
inline int Warrior::end() const   {return core_index + len;}
}