#include "instruction.h"

using namespace PSEmu;

Instruction::Instruction(uint32_t val) : m_intRep{ val } { }

uint32_t Instruction::GetImm() const
{
    return m_intRep & 0xFFFF;
}

uint32_t Instruction::GetImmJump() const
{
    return m_intRep & 0x3FFFFFF;
}

uint32_t Instruction::GetImmSe() const 
{ 
    // TODO: Explain
    const int16_t res = GetImm();
    return res; 
}

Opcode Instruction::GetOp() const
{
    const uint32_t coprocessorBit = 0x40000000;
    const uint32_t primaryOpcodePattern = 0xFC000000;
    const uint32_t secondaryOpcodePattern = 0x0000003F;

    uint32_t instOpcode = instToExec & coprocessorBit;
    if (instOpcode == 0)
    {
        instOpcode = instToExec & primaryOpcodePattern;
        
        if (instOpcode == 0)
        {
            instOpcode = instToExec & secondaryOpcodePattern;
        }
    }
    else
    {
        instOpcode = instToExec & 0x7FF00000;
    }

    return instOpcode;
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