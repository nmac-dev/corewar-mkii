/// Control Unit handles registers and decoding using the program counter

#include "ctrl_unit.hpp"

namespace OS
{

Register::Register(int _index, Inst *_inst)
{
    address = _index;
    OP    = &_inst->OP;
    A     = &_inst->A;
    B     = &_inst->B;
    event = Event::NOOP; // event applied to the address
}
Register::Register() = default;

ControlUnit::ControlUnit(Register _EXE, Register _SRC, Register _DEST)
{
    EXE    = _EXE;
    SRC    = _SRC;   // EXE [A]
    DEST   = _DEST;  // EXE [B]

    post_A = post_B = nullptr;

    TYPE.code  = OpcodeType::SYSTEM;
    TYPE.mod   =    ModifierType::SINGLE;
}

} /* ::OS */
