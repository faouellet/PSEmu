#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include "opcodes.h"

#include <cstdint>

namespace PSEmu
{

class Instruction
{
public:
    Instruction();
    Instruction(uint32_t val);

public:
    uint32_t GetImm() const;
    uint32_t GetImmJump() const;
    uint32_t GetImmSe() const;
    Opcode GetOp() const;
    uint32_t GetRd() const;
    uint32_t GetRs() const;
    uint32_t GetRt() const;
    uint32_t GetTarget() const;
    uint32_t GetShamt() const;
    uint32_t GetFunct() const;

public:
    operator uint32_t() const;

private:
    // Giving a specific value to a default constructed instruction
    // will help with the debugging
    uint32_t m_intRep = 0xDEADBEEF;
};

} // end namespace PSEmu

#endif // INSTRUCTION_H