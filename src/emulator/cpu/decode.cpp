#include "decode.h"

namespace PSEmu
{

ALUOperands DecodeSignExtendedImmediate(const std::array<uint32_t, 32>& regs, Instruction inst)
{
    return { inst.GetRt(), regs[inst.GetRs()], inst.GetImmSe() };
}

ALUOperands DecodeZeroExtendedImmediate(const std::array<uint32_t, 32>& regs, Instruction inst)
{
    return { inst.GetRt(), regs[inst.GetRs()], inst.GetImm() };
}

ALUOperands DecodeThreeOperands(const std::array<uint32_t, 32>& regs, Instruction inst)
{
    return { inst.GetRd(), regs[inst.GetRs()], regs[inst.GetRt()] };
}

}   // end namespace PSEmu