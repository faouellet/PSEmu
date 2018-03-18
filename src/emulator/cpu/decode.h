#ifndef DECODE_H
#define DECODE_H

#include "instruction.h"

#include <array>
#include <tuple>

namespace PSEmu
{

// TODO: This might not work with ExecuteTrappingALU which works on signed integers
using ALUOperands = std::tuple<uint32_t, uint32_t, uint32_t>;

ALUOperands DecodeSignExtendedImmediate(const std::array<uint32_t, 32>& regs, Instruction inst);
ALUOperands DecodeZeroExtendedImmediate(const std::array<uint32_t, 32>& regs, Instruction inst);
ALUOperands DecodeThreeOperands(const std::array<uint32_t, 32>& regs, Instruction inst);

}   // end namespace PSEmu

#endif // DECODE_H
