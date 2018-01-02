#include "instruction.h"

using namespace PSEmu;

// Giving a specific value to a default constructed instruction
// will help with the debugging
Instruction::Instruction() : m_intRep{ 0xDEADBEEF } { }

Instruction::Instruction(uint32_t val) : m_intRep{ val } { }

uint32_t Instruction::GetImmSe() const 
{ 
    // TODO: validate
    int16_t res = GetImm();
    return res; 
}

uint32_t Instruction::GetImm() const
{
    return m_intRep & 0xFFFF;
}

uint32_t Instruction::GetOp() const
{
    return m_intRep >> 26;
}

uint32_t Instruction::GetRd() const
{
    return (m_intRep >> 11) & 0x1F;
}

uint32_t Instruction::GetRs() const
{
    return (m_intRep >> 21) & 0x1F;
}

uint32_t Instruction::GetRt() const
{
    return (m_intRep >> 16) & 0x1F;
}

uint32_t Instruction::GetTarget() const
{
    return m_intRep & 0x3FFFFFF;
}

uint32_t Instruction::GetShamt() const
{
    return (m_intRep >> 6) & 0x1F;
}

uint32_t Instruction::GetFunct() const
{
    return m_intRep & 0x3F;
}

Instruction::operator uint32_t() const
{
    return m_intRep;
}