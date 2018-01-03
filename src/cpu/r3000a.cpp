#include "r3000a.h"

#include "opcodes.h"

#include "../memory/memorymap.h"

#include <cassert>
#include <limits>

using namespace PSEmu;

R3000A::R3000A(Interconnect&& interconnect) 
    : m_interconnect{ std::move(interconnect) }, 
      m_nextInst{ 0x0 }
{
    Reset();
    InitOpTable();
}

void R3000A::Step()
{
    // If the last instruction was a branch then we're in the delay slot
    m_isInDelaySlot = m_isBranching;
    m_isBranching = false;

    // Save the address of the current instruction to save in EPC in case of an exception
    m_currentPC = m_pc;

    if (m_currentPC % 4 != 0)
    {
        // PC is not correctly aligned!
        // Might happen after a bad jump
        TriggerException(ExceptionCause::LOAD_ADDRESS_ERROR);
        return;
    }

    // Fetch instruction at PC
    const Instruction instToExec = LoadWord(m_pc);

    // Increment next PC to point to the next instruction
    m_pc = m_nextPC;
    m_nextPC += 4;

    // Execute any pending load
    SetRegister(m_pendingLoad.first, m_pendingLoad.second);
    m_pendingLoad = {};
    
    auto foundIt = std::find_if(m_opTable.cbegin(), m_opTable.cend(), 
                                [&instToExec, this](const auto& opFtor)
                                { 
                                    uint32_t instOpcode = instToExec & PRIMARY_OPCODE_PATTERN;
                                    if (instOpcode == 0)
                                    {
                                        instOpcode = instToExec & SECONDARY_OPCODE_PATTERN;
                                    }
                                    return opFtor.first == instOpcode;
                                });

    if (foundIt != m_opTable.cend())
    {
        // TODO: Error handling
        std::invoke(foundIt->second, this, instToExec);

        // Copy the output registers as input for the next instruction
        m_registers = m_outputRegisters;
    }
    else
    {
        assert(false && "ILLEGAL INSTRUCTION!!!");
        TriggerException(ExceptionCause::ILLEGAL_INSTRUCTION);
    }
}

void R3000A::Reset()
{
    m_pc = 0xBFC00000;
    m_nextPC = m_pc + 4;
    m_registers.fill(0x0);
    m_outputRegisters.fill(0x0);
    m_sr = {};
    m_hi = {};
    m_lo = {};
    m_isBranching = false;
    m_isInDelaySlot = false;
}

void R3000A::InitOpTable()
{
    // TODO: order
    m_opTable[LUI]      = &R3000A::ExecuteLUI;
    m_opTable[ORI]      = &R3000A::ExecuteORI;
    m_opTable[SW]       = &R3000A::ExecuteSW;
    m_opTable[SLL]      = &R3000A::ExecuteSLL;
    m_opTable[ADDIU]    = &R3000A::ExecuteADDIU;
    m_opTable[J]        = &R3000A::ExecuteJ;
    m_opTable[OR]       = &R3000A::ExecuteOR;
    m_opTable[MTC0]     = &R3000A::ExecuteMTC0;
    m_opTable[BNE]      = &R3000A::ExecuteBNE;
    m_opTable[ADDI]     = &R3000A::ExecuteADDI;
    m_opTable[LW]       = &R3000A::ExecuteLW;
    m_opTable[SLTU]     = &R3000A::ExecuteSLTU;
    m_opTable[SH]       = &R3000A::ExecuteSH;
    m_opTable[JAL]      = &R3000A::ExecuteJAL;
    m_opTable[ANDI]     = &R3000A::ExecuteANDI;
    m_opTable[JR]       = &R3000A::ExecuteJR;
    m_opTable[LB]       = &R3000A::ExecuteLB;
    m_opTable[BEQ]      = &R3000A::ExecuteBEQ;
    m_opTable[MFC0]     = &R3000A::ExecuteMFC0;
    m_opTable[AND]      = &R3000A::ExecuteAND;
    m_opTable[ADD]      = &R3000A::ExecuteADD;
    m_opTable[BGTZ]     = &R3000A::ExecuteBGTZ;
    m_opTable[BLEZ]     = &R3000A::ExecuteBLEZ;
    m_opTable[LBU]      = &R3000A::ExecuteLBU;
    m_opTable[JALR]     = &R3000A::ExecuteJALR;
    m_opTable[BLTZ]     = &R3000A::ExecuteBLTZ;
    m_opTable[BLTZAL]   = &R3000A::ExecuteBLTZAL;
    m_opTable[BGEZ]     = &R3000A::ExecuteBGEZ;
    m_opTable[BGEZAL]   = &R3000A::ExecuteBGEZAL;
    m_opTable[SLTI]     = &R3000A::ExecuteSLTI;
    m_opTable[SUBU]     = &R3000A::ExecuteSUBU;
    m_opTable[SRA]      = &R3000A::ExecuteSRA;
    m_opTable[DIV]      = &R3000A::ExecuteDIV;
    m_opTable[MFLO]     = &R3000A::ExecuteMFLO;
    m_opTable[SRL]      = &R3000A::ExecuteSRL;
    m_opTable[SLTIU]    = &R3000A::ExecuteSLTIU;
    m_opTable[DIVU]     = &R3000A::ExecuteDIVU;
    m_opTable[MFHI]     = &R3000A::ExecuteMFHI;
    m_opTable[SLT]      = &R3000A::ExecuteSLT;
    m_opTable[SYSCALL]  = &R3000A::ExecuteSYSCALL;
    m_opTable[MTLO]     = &R3000A::ExecuteMTLO;
    m_opTable[MTHI]     = &R3000A::ExecuteMTHI;
    m_opTable[RFE]      = &R3000A::ExecuteRFE;
    m_opTable[LHU]      = &R3000A::ExecuteLHU;
    m_opTable[SLLV]     = &R3000A::ExecuteSLLV;
    m_opTable[LH]       = &R3000A::ExecuteLH;
    m_opTable[NOR]      = &R3000A::ExecuteNOR;
    m_opTable[SRAV]     = &R3000A::ExecuteSRAV;
    m_opTable[SRLV]     = &R3000A::ExecuteSRLV;
    m_opTable[MULTU]    = &R3000A::ExecuteMULTU;
    m_opTable[XOR]      = &R3000A::ExecuteXOR;
    m_opTable[BREAK]    = &R3000A::ExecuteBREAK;
    m_opTable[MULT]     = &R3000A::ExecuteMULT;
    m_opTable[SUB]      = &R3000A::ExecuteSUB;
    m_opTable[XORI]     = &R3000A::ExecuteXORI;
    m_opTable[COP1]     = &R3000A::ExecuteCOP1;
    m_opTable[COP2]     = &R3000A::ExecuteCOP2;
    m_opTable[COP3]     = &R3000A::ExecuteCOP3;
    m_opTable[LWL]      = &R3000A::ExecuteLWL;
    m_opTable[LWR]      = &R3000A::ExecuteLWR;
    m_opTable[SWL]      = &R3000A::ExecuteSWL;
    m_opTable[SWR]      = &R3000A::ExecuteSWR;
    m_opTable[LWC0]     = &R3000A::ExecuteLWC0;
    m_opTable[LWC1]     = &R3000A::ExecuteLWC1;
    m_opTable[LWC2]     = &R3000A::ExecuteLWC2;
    m_opTable[LWC3]     = &R3000A::ExecuteLWC3;
    m_opTable[SWC0]     = &R3000A::ExecuteSWC0;
    m_opTable[SWC1]     = &R3000A::ExecuteSWC1;
    m_opTable[SWC2]     = &R3000A::ExecuteSWC2;
    m_opTable[SWC3]     = &R3000A::ExecuteSWC3;
}

uint8_t R3000A::LoadByte(uint32_t address)
{
    if (m_sr & 0x10000 != 0)
    {
        // TODO: Msg -> Cache is isolated, load ignored
        return -1;
    }
    return m_interconnect.LoadByte(address);
}

uint16_t R3000A::LoadHalfWord(uint32_t address)
{
    if (m_sr & 0x10000 != 0)
    {
        // TODO: Msg -> Cache is isolated, load ignored
        return -1;
    }

    // Address must be 32 bits aligned
    if (address % 2 != 0)
    {
        TriggerException(ExceptionCause::LOAD_ADDRESS_ERROR);
    }

    return m_interconnect.LoadHalfWord(address);
}

uint32_t R3000A::LoadWord(uint32_t address)
{
    if (m_sr & 0x10000 != 0)
    {
        // TODO: Msg -> Cache is isolated, load ignored
        return -1;
    }

    // Address must be 32 bits aligned
    if (address % 4 != 0)
    {
        TriggerException(ExceptionCause::LOAD_ADDRESS_ERROR);
    }

    return m_interconnect.LoadWord(address);
}

void R3000A::StoreByte(uint32_t address, uint8_t value)
{
    if (m_sr & 0x10000 != 0)
    {
        // TODO: Msg -> Cache is isolated, write ignored
        return;
    }
    m_interconnect.StoreByte(address, value);
}

void R3000A::StoreHalfWord(uint32_t address, uint16_t value)
{
    if (m_sr & 0x10000 != 0)
    {
        // TODO: Msg -> Cache is isolated, write ignored
        return;
    }

    // Address must be 16 bits aligned
    if (address % 2 != 0)
    {
        TriggerException(ExceptionCause::STORE_ADDRESS_ERROR);
    }

    m_interconnect.StoreHalfWord(address, value);
}

void R3000A::StoreWord(uint32_t address, uint32_t value)
{
    if (m_sr & 0x10000 != 0)
    {
        // TODO: Msg -> Cache is isolated, write ignored
        return;
    }

    // Address must be 32 bits aligned
    if (address % 4 != 0)
    {
        TriggerException(ExceptionCause::STORE_ADDRESS_ERROR);
    }

    m_interconnect.StoreWord(address, value);
}

void R3000A::Branch(uint32_t offset)
{
    m_isBranching = true;
    const uint32_t alignedOffset = offset << 2;
    m_nextPC += alignedOffset;
}

bool R3000A::WouldOverflow(int32_t lhs, int32_t rhs, std::function<int32_t(int32_t,int32_t)>&& func) const
{
    return lhs > func(rhs, std::numeric_limits<int32_t>::max());
}

void R3000A::SetRegister(uint32_t registerIndex, uint32_t value)
{
    m_outputRegisters[registerIndex] = value;

    // Make sure R0 is always equal to 0
    m_outputRegisters[0] = 0;
}

void R3000A::TriggerException(ExceptionCause cause)
{
    // Exception handler address depends on the BEV bit
    const uint32_t handler = (m_sr & (1 << 22)) == 0 ? 0x80000080 : 0xBFC00180;

    // Shift bits [5:0] of SR two places to the left.
    // Those bits are three pairs of Interrupt Enable/User Mode
    // bits behaving like a stack 3 entries deep.
    // Entering an exception pushes a pair of zeroes
    // by left shifting the stack which disables
    // interrupts and puts the CPU in kernel mode.
    // The original third entry is discarded (it's up
    // to the kernel to handle more than two recursive
    // exception levels).
    const uint32_t mode = m_sr & 0x3F;
    m_sr &= ~0x3f;
    m_sr |= (mode << 2) & 0x3F;

    // Update the CAUSE register with the exception code (bits [6:2])
    m_cause <<= 2;

    // Save current instruction address in EPC
    m_epc = m_currentPC;

    if (m_isInDelaySlot)
    {
        // When an exception occurs in the branch delay slot, EPC
        // points to the branch instruction and bit 31 of CAUSE is set
        m_epc -= 4;
        m_cause |= 1 << 31;
    }

    // Exceptions don't have a branch delay, we jump directly into the handler
    m_pc = handler;
    m_nextPC = m_pc + 4;
}

void R3000A::ExecuteLUI(Instruction inst)
{
    uint32_t res = inst.GetImm() << 16;
    SetRegister(inst.GetRt(), res);
}

void R3000A::ExecuteORI(Instruction inst)
{
    SetRegister(inst.GetRt(), inst.GetImm() | m_registers[inst.GetRs()]);
}

void R3000A::ExecuteSW(Instruction inst)
{
    const uint32_t address = m_registers[inst.GetRs()] + inst.GetImmSe();
    const uint32_t value = m_registers[inst.GetRt()];
    StoreWord(address, value);
}

void R3000A::ExecuteSLL(Instruction inst)
{
    SetRegister(inst.GetRd(), m_registers[inst.GetRt()] << inst.GetShamt());
}

void R3000A::ExecuteADDIU(Instruction inst)
{
    SetRegister(inst.GetRt(), m_registers[inst.GetRs()] + inst.GetImmSe());
}

void R3000A::ExecuteJ(Instruction inst)
{
    m_isBranching = true;
    m_nextPC = (m_pc & 0xF0000000) | (inst.GetImmJump() << 2);
}

void R3000A::ExecuteOR(Instruction inst)
{
    SetRegister(inst.GetRd(), m_registers[inst.GetRs()] | m_registers[inst.GetRt()]);
}

void R3000A::ExecuteMTC0(Instruction inst)
{
    uint32_t cop0Reg = inst.GetRd();
    if (cop0Reg != 0xC)
    {
        // TODO: unimplemented register access
        return;
    }

    m_sr = m_registers[inst.GetRt()];
}

void R3000A::ExecuteBNE(Instruction inst)
{
    if (m_registers[inst.GetRs()] != m_registers[inst.GetRt()])
    {
        Branch(inst.GetImmSe());
    }
}

void R3000A::ExecuteADDI(Instruction inst)
{
    const int32_t lhs = m_registers[inst.GetRs()];
    const int32_t rhs = inst.GetImmSe();

    // Check for overflow.
    if (WouldOverflow(lhs, rhs, std::minus<int32_t>()))
    {
        TriggerException(ExceptionCause::OVERFLOW);
    }

    SetRegister(inst.GetRt(), lhs + rhs);
}

void R3000A::ExecuteLW(Instruction inst)
{
    uint32_t address = m_registers[inst.GetRs()] + inst.GetImmSe();
    uint32_t value = LoadWord(address); 
    SetRegister(inst.GetRt(), value);

    m_pendingLoad = {{inst.GetRt()}, value};
}

void R3000A::ExecuteSLTU(Instruction inst)
{
    SetRegister(inst.GetRd(), m_registers[inst.GetRs()] < m_registers[inst.GetRt()]);
}

void R3000A::ExecuteADDU(Instruction inst)
{
    SetRegister(inst.GetRd(), m_registers[inst.GetRs()] + m_registers[inst.GetRt()]);
}

void R3000A::ExecuteSH(Instruction inst)
{
    const uint32_t address = m_registers[inst.GetRs()] + inst.GetImmSe();
    const uint16_t value = m_registers[inst.GetRs()];
    StoreHalfWord(address, value);
}

void R3000A::ExecuteJAL(Instruction inst)
{
    m_isBranching = true;

    // Store return address in the RA register
    SetRegister(31, m_pc);

    m_nextPC = (m_pc & 0xF0000000) | (inst.GetImm() << 2);
}

void R3000A::ExecuteANDI(Instruction inst)
{
    SetRegister(inst.GetRt(), inst.GetImm() & m_registers[inst.GetRs()]);
}

void R3000A::ExecuteSB(Instruction inst)
{
    const uint32_t address = m_registers[inst.GetRs()] + inst.GetImmSe();
    const uint8_t value = m_registers[inst.GetRs()];
    StoreHalfWord(address, value);
}

void R3000A::ExecuteJR(Instruction inst)
{
    m_isBranching = true;
    m_nextPC = m_registers[inst.GetRs()];
}

void R3000A::ExecuteLB(Instruction inst)
{
    const uint32_t addr = m_registers[inst.GetRs()] + inst.GetImmSe();
    const int8_t value = LoadByte(addr);

    m_pendingLoad = {{inst.GetRt()}, value};
}

void R3000A::ExecuteBEQ(Instruction inst)
{
    if (m_registers[inst.GetRs()] == m_registers[inst.GetRt()])
    {
        Branch(inst.GetImmSe());
    }
}

void R3000A::ExecuteMFC0(Instruction inst)
{
    uint32_t cop0Reg = inst.GetRd();
    if (cop0Reg != 0xC
        || cop0Reg != 0xD
        || cop0Reg != 0xE)
    {
        // TODO: unimplemented register access
        return;
    }

    SetRegister(inst.GetRt(), m_sr);

    m_pendingLoad = {{inst.GetRt()}, m_sr};
}

void R3000A::ExecuteAND(Instruction inst)
{
    SetRegister(inst.GetRt(), inst.GetImm() & m_registers[inst.GetRs()]);
}

void R3000A::ExecuteADD(Instruction inst)
{
    const int32_t lhs = m_registers[inst.GetRs()];
    const int32_t rhs = m_registers[inst.GetRt()];

    // Check for overflow.
    if (WouldOverflow(lhs, rhs, std::minus<int32_t>()))
    {
        TriggerException(ExceptionCause::OVERFLOW);
    }   
 
    SetRegister(inst.GetRd(), lhs + rhs);
}

void R3000A::ExecuteBGTZ(Instruction inst)
{
    const int32_t value = m_registers[inst.GetRs()];
    if (value > 0)
    {
        Branch(inst.GetImmSe());
    }
}

void R3000A::ExecuteBLEZ(Instruction inst)
{
    const int32_t value = m_registers[inst.GetRs()];
    if (value <= 0)
    {
        Branch(inst.GetImmSe());
    }
}

void R3000A::ExecuteLBU(Instruction inst)
{
    const uint32_t addr = m_registers[inst.GetRs()] + inst.GetImm();
    const int8_t value = LoadByte(addr);

    m_pendingLoad = {{inst.GetRt()}, value};
}

void R3000A::ExecuteJALR(Instruction inst)
{
    m_isBranching = true;
    SetRegister(inst.GetRd(), m_pc);
    m_nextPC = m_registers[inst.GetRs()];
}

void R3000A::ExecuteBLTZ(Instruction inst)
{
    const int32_t value = m_registers[inst.GetRs()];
    if (value < 0)
    {
        Branch(inst.GetImmSe());
    }
}

void R3000A::ExecuteBLTZAL(Instruction inst)
{
    const int32_t value = m_registers[inst.GetRs()];

    // Store return address in the RA register
    SetRegister(31, m_pc);

    if (value < 0)
    {
        Branch(inst.GetImmSe());
    }
}

void R3000A::ExecuteBGEZ(Instruction inst)
{
    const int32_t value = m_registers[inst.GetRs()];
    if (value > 0)
    {
        Branch(inst.GetImmSe());
    }
}

void R3000A::ExecuteBGEZAL(Instruction inst)
{
    const int32_t value = m_registers[inst.GetRs()];

    // Store return address in the RA register
    SetRegister(31, m_pc);

    if (value > 0)
    {
        Branch(inst.GetImmSe());
    }
}

void R3000A::ExecuteSLTI(Instruction inst)
{
    SetRegister(inst.GetRt(), static_cast<int32_t>(m_registers[inst.GetRs()]) < inst.GetImmSe());
}

void R3000A::ExecuteSUBU(Instruction inst)
{
    SetRegister(inst.GetRd(), m_registers[inst.GetRs()] - m_registers[inst.GetRt()]);
}

void R3000A::ExecuteSRA(Instruction inst)
{
    const uint32_t value = (static_cast<int32_t>(inst.GetRt()) >> inst.GetShamt());
    SetRegister(inst.GetRd(), value);
}

void R3000A::ExecuteDIV(Instruction inst)
{
    const int32_t numerator = m_registers[inst.GetRs()];
    const int32_t denominator = m_registers[inst.GetRt()];

    if (denominator == 0)
    {
        // Division by 0, results are bogus
        m_hi = numerator;
        m_lo = numerator >= 0 ? 0xFFFFFFFF : 1;
    }
    else if((static_cast<uint32_t>(numerator) == 0x80000000) && (denominator == -1))
    {
        // Result is not reprensentable in 32 bits
        m_hi = 0;
        m_lo = 0x80000000;
    }
    else
    {
        m_hi = numerator % denominator;
        m_lo = numerator / denominator;
    }
}

void R3000A::ExecuteMFLO(Instruction inst)
{
    SetRegister(inst.GetRd(), m_lo);
}

void R3000A::ExecuteSRL(Instruction inst)
{
    SetRegister(inst.GetRd(), inst >> inst.GetShamt());
}

void R3000A::ExecuteSLTIU(Instruction inst)
{
    SetRegister(inst.GetRt(), m_registers[inst.GetRs()] < inst.GetImmSe());
}

void R3000A::ExecuteDIVU(Instruction inst)
{
    const uint32_t numerator = m_registers[inst.GetRs()];
    const uint32_t denominator = m_registers[inst.GetRt()];

    if (denominator == 0)
    {
        // Division by 0, results are bogus
        m_hi = numerator;
        m_lo = 0xFFFFFFFF;
    }
    else
    {
        m_hi = numerator % denominator;
        m_lo = numerator / denominator;
    }
}

void R3000A::ExecuteMFHI(Instruction inst)
{
    SetRegister(inst.GetRd(), m_hi);
}

void R3000A::ExecuteSLT(Instruction inst)
{
    SetRegister(inst.GetRd(), static_cast<int32_t>(m_registers[inst.GetRs()]) < static_cast<int32_t>(m_registers[inst.GetRt()]));
}

void R3000A::ExecuteSYSCALL(Instruction inst)
{
    TriggerException(ExceptionCause::SYSCALL);
}

void R3000A::ExecuteMTLO(Instruction inst)
{
    SetRegister(m_lo, m_registers[inst.GetRs()]);
}

void R3000A::ExecuteMTHI(Instruction inst)
{
    SetRegister(m_hi, m_registers[inst.GetRs()]);
}

void R3000A::ExecuteRFE(Instruction inst)
{
    // Restore the pre-exception mode by shifting the
    // Interrupt Enable/User Mode stack back to its 
    // original position.
    const uint32_t mode = m_sr & 0x3F;
    m_sr &= !0x3F;
    m_sr |= mode >> 2;
}

void R3000A::ExecuteLHU(Instruction inst)
{
    const uint32_t addr = m_registers[inst.GetRs()] + inst.GetImmSe();

    // TODO: Subtle bug here and in all memory related operations
    //       The problem is that a load/store shouldn't be completed
    //       if an exception is triggered
    const uint32_t value = LoadHalfWord(addr);

    m_pendingLoad = {{inst.GetRt()}, value};
}

void R3000A::ExecuteSLLV(Instruction inst)
{
    // Shift amount is truncated to 5 bits
    SetRegister(inst.GetRd(), m_registers[inst.GetRt()] << (m_registers[inst.GetRs()] & 0x1F));
}

void R3000A::ExecuteLH(Instruction inst)
{
    const uint32_t addr = m_registers[inst.GetRs()] + inst.GetImmSe();
    const int16_t value = LoadHalfWord(addr);

    m_pendingLoad = {{inst.GetRt()}, value};
}

void R3000A::ExecuteNOR(Instruction inst)
{
    SetRegister(inst.GetRd(), !(m_registers[inst.GetRs()] | m_registers[inst.GetRt()]));
}

void R3000A::ExecuteSRAV(Instruction inst)
{
    // Shift amount is truncated to 5 bits
    SetRegister(inst.GetRd(), static_cast<int32_t>(m_registers[inst.GetRt()]) >> (m_registers[inst.GetRs()] & 0x1F));
}

void R3000A::ExecuteSRLV(Instruction inst)
{
    // Shift amount is truncated to 5 bits
    SetRegister(inst.GetRd(), m_registers[inst.GetRt()] >> (m_registers[inst.GetRs()] & 0x1F));
}

void R3000A::ExecuteMULTU(Instruction inst)
{
    const uint64_t value = m_registers[inst.GetRs()] * m_registers[inst.GetRt()];

    m_hi = value >> 32;
    m_lo = value;
}

void R3000A::ExecuteXOR(Instruction inst)
{
    SetRegister(inst.GetRd(), m_registers[inst.GetRs()] ^ m_registers[inst.GetRt()]);
}

void R3000A::ExecuteBREAK(Instruction inst)
{
    TriggerException(ExceptionCause::BREAK);
}

void R3000A::ExecuteMULT(Instruction inst)
{
    const int64_t lhs = m_registers[inst.GetRs()];
    const int64_t rhs = m_registers[inst.GetRt()];

    const uint64_t result = lhs * rhs;

    m_hi = result >> 32;
    m_lo = result;
}

void R3000A::ExecuteSUB(Instruction inst)
{
    const int32_t lhs = m_registers[inst.GetRs()];
    const int32_t rhs = m_registers[inst.GetRt()];

    // Check for overflow.
    if (WouldOverflow(lhs, rhs, std::plus<int32_t>()))
    {
        TriggerException(ExceptionCause::OVERFLOW);
    }

    SetRegister(inst.GetRd(), lhs - rhs);
}

void R3000A::ExecuteXORI(Instruction inst)
{
    SetRegister(inst.GetRt(), inst.GetImm() ^ m_registers[inst.GetRs()]);
}

void R3000A::ExecuteCOP1(Instruction inst)
{
    TriggerException(ExceptionCause::COPROCESSOR_ERROR);
}

void R3000A::ExecuteCOP2(Instruction inst)
{
    // TODO: Not implemented yet!
}

void R3000A::ExecuteCOP3(Instruction inst)
{
    TriggerException(ExceptionCause::COPROCESSOR_ERROR);
}

void R3000A::ExecuteLWL(Instruction inst)
{
    const uint32_t address = m_registers[inst.GetRs()] + inst.GetImmSe();

    // This instruction bypasses the load delay restriction:
    // this instruction will merge the new contents with the
    // value currently being loaded if need be.
    const uint32_t curValue = m_outputRegisters[inst.GetRt()];

    // Next, we load the *aligned* word containing the first
    // addressed byte
    const uint32_t alignedAddr = address & !3;
    const uint32_t alignedWord = LoadWord(alignedAddr);

    // Depending on the address alignment, we fetch the 1,2,3 or 4
    // *most* significant bytes and put them in the target register
    uint32_t newValue;
    switch (address & 3)
    {
        case 0:
            newValue = (curValue & 0x00FFFFFF) | (alignedWord << 24);
            break;
        case 1:
            newValue = (curValue & 0x0000FFFF) | (alignedWord << 16);
            break;
        case 2:
            newValue = (curValue & 0x000000FF) | (alignedWord << 8);
            break;
        case 3:
            newValue = (curValue & 0x00000000) | (alignedWord << 0);
            break;
        default:
            assert(false && "You've reached the unreachable!");
    }

    m_pendingLoad = {{inst.GetRt()}, newValue};
}

void R3000A::ExecuteLWR(Instruction inst)
{
    const uint32_t address = m_registers[inst.GetRs()] + inst.GetImmSe();

    // This instruction bypasses the load delay restriction:
    // this instruction will merge the new contents with the
    // value currently being loaded if need be.
    const uint32_t curValue = m_outputRegisters[inst.GetRt()];

    // Next, we load the *aligned* word containing the first
    // addressed byte
    const uint32_t alignedAddr = address & !3;
    const uint32_t alignedWord = LoadWord(alignedAddr);

    // Depending on the address alignment, we fetch the 1,2,3 or 4
    // *least* significant bytes and put them in the target register
    uint32_t newValue;
    switch (address & 3)
    {
        case 0:
            newValue = (curValue & 0x00000000) | (alignedWord >> 0);
            break;
        case 1:
            newValue = (curValue & 0xFF000000) | (alignedWord >> 8);
            break;
        case 2:
            newValue = (curValue & 0xFFFF0000) | (alignedWord << 16);
            break;
        case 3:
            newValue = (curValue & 0xFFFFFF00) | (alignedWord >> 24);
            break;
        default:
            assert(false && "You've reached the unreachable!");
    }

    m_pendingLoad = {{inst.GetRt()}, newValue};
}

void R3000A::ExecuteSWL(Instruction inst)
{
    const uint32_t address = m_registers[inst.GetRs()] + inst.GetImmSe();
    const uint32_t value = m_registers[inst.GetRt()];

    const uint32_t alignedAddr = address & !3;

    // Load the current value for the aligned word at the target address
    const uint32_t curValue = LoadWord(alignedAddr);

    uint32_t newValue;
    switch (address & 3)
    {
        case 0:
            newValue = (curValue & 0x00FFFFFF) | (value >> 24);
            break;
        case 1:
            newValue = (curValue & 0x0000FFFF) | (value >> 16);
            break;
        case 2:
            newValue = (curValue & 0x000000FF) | (value >> 8);
            break;
        case 3:
            newValue = (curValue & 0x00000000) | (value >> 0);
            break;
        default:
            assert(false && "You've reached the unreachable!");
    }

    StoreWord(alignedAddr, newValue);
}

void R3000A::ExecuteSWR(Instruction inst)
{
    const uint32_t address = m_registers[inst.GetRs()] + inst.GetImmSe();
    const uint32_t value = m_registers[inst.GetRt()];

    const uint32_t alignedAddr = address & !3;

    // Load the current value for the aligned word at the target address
    const uint32_t curValue = LoadWord(alignedAddr);

    uint32_t newValue;
    switch (address & 3)
    {
        case 0:
            newValue = (curValue & 0x00000000) | (value << 0);
            break;
        case 1:
            newValue = (curValue & 0xFF000000) | (value << 8);
            break;
        case 2:
            newValue = (curValue & 0xFFFF0000) | (value << 16);
            break;
        case 3:
            newValue = (curValue & 0xFFFFFF00) | (value << 24);
            break;
        default:
            assert(false && "You've reached the unreachable!");
    }

    StoreWord(alignedAddr, newValue);
}

void R3000A::ExecuteLWC0(Instruction inst)
{
    TriggerException(ExceptionCause::COPROCESSOR_ERROR);
}

void R3000A::ExecuteLWC1(Instruction inst)
{
    TriggerException(ExceptionCause::COPROCESSOR_ERROR);
}

void R3000A::ExecuteLWC2(Instruction inst)
{
    // TODO: Not implemented yet!
}

void R3000A::ExecuteLWC3(Instruction inst)
{
    TriggerException(ExceptionCause::COPROCESSOR_ERROR);
}

void R3000A::ExecuteSWC0(Instruction inst)
{
    TriggerException(ExceptionCause::COPROCESSOR_ERROR);
}

void R3000A::ExecuteSWC1(Instruction inst)
{
    TriggerException(ExceptionCause::COPROCESSOR_ERROR);
}

void R3000A::ExecuteSWC2(Instruction inst)
{
    // TODO: Not implemented yet!
}

void R3000A::ExecuteSWC3(Instruction inst)
{
    TriggerException(ExceptionCause::COPROCESSOR_ERROR);
}
