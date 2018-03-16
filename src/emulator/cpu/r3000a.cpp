#include "r3000a.h"

#include "opcodes.h"

#include "../memory/memorymap.h"

#include <cassert>
#include <limits>


namespace
{

}

R3000A::R3000A(Interconnect interconnect, Debugger debugger) 
    : m_interconnect{ std::move(interconnect) }, 
      m_nextInst{ 0x0 },
      m_debugger{ std::move(debugger) }
{
    Reset();
}

void R3000A::Step()
{
    // If the last instruction was a branch then we're in the delay slot
    m_isInDelaySlot = m_isBranching;
    m_isBranching = false;

    // Save the address of the current instruction to save in EPC in case of an exception
    m_currentPC = m_pc;

    // Debugger entry point: used for code breakpoints and stepping
    m_debugger.OnPCChange(*this);

    if (m_currentPC % 4 != 0)
    {
        // PC is not correctly aligned!
        // Might happen after a bad jump
        TriggerException(ExceptionCause::LOAD_ADDRESS_ERROR);
        return;
    }

    // Fetch instruction at PC
    const Instruction instToExec = m_interconnect.Load<uint32_t>(m_pc);

    // Increment next PC to point to the next instruction
    m_pc = m_nextPC;
    m_nextPC += 4;

    // Execute any pending load
    SetRegister(m_pendingLoad.first, m_pendingLoad.second);
    m_pendingLoad = {};

    switch(instToExec.GetOp())
    {
        case LUI:     ExecuteLUI(instToExec); break;
        case ORI:     ExecuteORI(instToExec); break;
        case SW:      ExecuteSW(instToExec); break;
        case SLL:     ExecuteSLL(instToExec); break;
        case ADDIU:   ExecuteADDIU(instToExec); break;
        case J:       ExecuteJ(instToExec); break;
        case OR:      ExecuteOR(instToExec); break;
        case MTC0:    ExecuteMTC0(instToExec); break;
        case BNE:     ExecuteBNE(instToExec); break;
        case ADDI:    ExecuteADDI(instToExec); break;
        case LW:      ExecuteLW(instToExec); break;
        case SLTU:    ExecuteSLTU(instToExec); break;
        case ADDU:    ExecuteADDU(instToExec); break;
        case SH:      ExecuteSH(instToExec); break;
        case JAL:     ExecuteJAL(instToExec); break;
        case ANDI:    ExecuteANDI(instToExec); break;
        case SB:      ExecuteSB(instToExec); break;
        case JR:      ExecuteJR(instToExec); break;
        case LB:      ExecuteLB(instToExec); break;
        case BEQ:     ExecuteBEQ(instToExec); break;
        case MFC0:    ExecuteMFC0(instToExec); break;
        case AND:     ExecuteAND(instToExec); break;
        case ADD:     ExecuteADD(instToExec); break;
        case BGTZ:    ExecuteBGTZ(instToExec); break;
        case BLEZ:    ExecuteBLEZ(instToExec); break;
        case LBU:     ExecuteLBU(instToExec); break;
        case JALR:    ExecuteJALR(instToExec); break;
        case BLTZ:    ExecuteBLTZ(instToExec); break;
        case BLTZAL:  ExecuteBLTZAL(instToExec); break;
        case BGEZ:    ExecuteBGEZ(instToExec); break;
        case BGEZAL:  ExecuteBGEZAL(instToExec); break;
        case SLTI:    ExecuteSLTI(instToExec); break;
        case SUBU:    ExecuteSUBU(instToExec); break;
        case SRA:     ExecuteSRA(instToExec); break;
        case DIV:     ExecuteDIV(instToExec); break;
        case MFLO:    ExecuteMFLO(instToExec); break;
        case SRL:     ExecuteSRL(instToExec); break;
        case SLTIU:   ExecuteSLTIU(instToExec); break;
        case DIVU:    ExecuteDIVU(instToExec); break;
        case MFHI:    ExecuteMFHI(instToExec); break;
        case SLT:     ExecuteSLT(instToExec); break;
        case SYSCALL: ExecuteSYSCALL(instToExec); break;
        case MTLO:    ExecuteMTLO(instToExec); break;
        case MTHI:    ExecuteMTHI(instToExec); break;
        case RFE :    ExecuteRFE(instToExec); break;
        case LHU :    ExecuteLHU(instToExec); break;
        case SLLV:    ExecuteSLLV(instToExec); break;
        case LH:      ExecuteLH(instToExec); break;
        case NOR:     ExecuteNOR(instToExec); break;
        case SRAV:    ExecuteSRAV(instToExec); break;
        case SRLV:    ExecuteSRLV(instToExec); break;
        case MULTU:   ExecuteMULTU(instToExec); break;
        case XOR:     ExecuteXOR(instToExec); break;
        case BREAK:   ExecuteBREAK(instToExec); break;
        case MULT:    ExecuteMULT(instToExec); break;
        case SUB:     ExecuteSUB(instToExec); break;
        case XORI:    ExecuteXORI(instToExec); break;
        case COP1:    ExecuteCOP1(instToExec); break;
        case COP2:    ExecuteCOP2(instToExec); break;
        case COP3:    ExecuteCOP3(instToExec); break;
        case LWL:     ExecuteLWL(instToExec); break;
        case LWR:     ExecuteLWR(instToExec); break;
        case SWL:     ExecuteSWL(instToExec); break;
        case SWR:     ExecuteSWR(instToExec); break;
        case LWC0:    ExecuteLWC0(instToExec); break;
        case LWC1:    ExecuteLWC1(instToExec); break;
        case LWC2:    ExecuteLWC2(instToExec); break;
        case LWC3:    ExecuteLWC3(instToExec); break;
        case SWC0:    ExecuteSWC0(instToExec); break;
        case SWC1:    ExecuteSWC1(instToExec); break;
        case SWC2:    ExecuteSWC2(instToExec); break;
        case SWC3:    ExecuteSWC3(instToExec); break;
        default:    assert(false && "ILLEGAL INSTRUCTION!!!"); TriggerException(ExceptionCause::ILLEGAL_INSTRUCTION); break;
    }
       
    // Copy the output registers as input for the next instruction
    m_registers = m_outputRegisters;
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

uint32_t R3000A::GetPC() const
{
    return m_pc;
}

void R3000A::Branch(uint32_t offset)
{
    m_isBranching = true;
    const uint32_t alignedOffset = offset << 2;
    m_nextPC += alignedOffset;
}

bool R3000A::WouldOverflow(int32_t lhs, int32_t rhs, std::function<int32_t(int32_t,int32_t)>&& func) const
{
    return lhs > func(std::numeric_limits<int32_t>::max(), rhs);
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
    m_cause = static_cast<uint32_t>(cause) << 2;

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
    Store<uint32_t>(address, value);
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
    uint32_t value = Load<uint32_t>(address); 

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
    const uint16_t value = m_registers[inst.GetRt()];
    Store<uint16_t>(address, value);
}

void R3000A::ExecuteJAL(Instruction inst)
{
    // Store return address in the RA register
    SetRegister(31, m_pc);

    ExecuteJ(inst);
}

void R3000A::ExecuteANDI(Instruction inst)
{
    SetRegister(inst.GetRt(), inst.GetImm() & m_registers[inst.GetRs()]);
}

void R3000A::ExecuteSB(Instruction inst)
{
    const uint32_t address = m_registers[inst.GetRs()] + inst.GetImmSe();
    const uint8_t value = m_registers[inst.GetRt()];
    Store<uint8_t>(address, value);
}

void R3000A::ExecuteJR(Instruction inst)
{
    m_isBranching = true;
    m_nextPC = m_registers[inst.GetRs()];
}

void R3000A::ExecuteLB(Instruction inst)
{
    const uint32_t addr = m_registers[inst.GetRs()] + inst.GetImmSe();
    // Converting to int8_t to force sign extension
    const int8_t value = Load<uint8_t>(addr);

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
    const int8_t value = Load<uint8_t>(addr);

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
    SetRegister(inst.GetRt(), static_cast<int32_t>(m_registers[inst.GetRs()]) < static_cast<int32_t>(inst.GetImmSe()));
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

void R3000A::ExecuteSYSCALL(Instruction)
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

void R3000A::ExecuteRFE(Instruction)
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
    const uint32_t value = Load<uint16_t>(addr);

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
    // Converting to int16_t to force sign extension
    const int16_t value = Load<uint16_t>(addr);

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

void R3000A::ExecuteBREAK(Instruction)
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

void R3000A::ExecuteCOP1(Instruction)
{
    TriggerException(ExceptionCause::COPROCESSOR_ERROR);
}

void R3000A::ExecuteCOP2(Instruction)
{
    // TODO: Not implemented yet!
}

void R3000A::ExecuteCOP3(Instruction)
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
    const uint32_t alignedWord = Load<uint32_t>(alignedAddr);

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
    const uint32_t alignedWord = Load<uint32_t>(alignedAddr);

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
    const uint32_t curValue = Load<uint32_t>(alignedAddr);

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

    Store<uint32_t>(alignedAddr, newValue);
}

void R3000A::ExecuteSWR(Instruction inst)
{
    const uint32_t address = m_registers[inst.GetRs()] + inst.GetImmSe();
    const uint32_t value = m_registers[inst.GetRt()];

    const uint32_t alignedAddr = address & !3;

    // Load the current value for the aligned word at the target address
    const uint32_t curValue = Load<uint32_t>(alignedAddr);

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

    Store<uint32_t>(alignedAddr, newValue);
}

void R3000A::ExecuteLWC0(Instruction)
{
    TriggerException(ExceptionCause::COPROCESSOR_ERROR);
}

void R3000A::ExecuteLWC1(Instruction)
{
    TriggerException(ExceptionCause::COPROCESSOR_ERROR);
}

void R3000A::ExecuteLWC2(Instruction)
{
    // TODO: Not implemented yet!
}

void R3000A::ExecuteLWC3(Instruction)
{
    TriggerException(ExceptionCause::COPROCESSOR_ERROR);
}

void R3000A::ExecuteSWC0(Instruction)
{
    TriggerException(ExceptionCause::COPROCESSOR_ERROR);
}

void R3000A::ExecuteSWC1(Instruction)
{
    TriggerException(ExceptionCause::COPROCESSOR_ERROR);
}

void R3000A::ExecuteSWC2(Instruction)
{
    // TODO: Not implemented yet!
}

void R3000A::ExecuteSWC3(Instruction)
{
    TriggerException(ExceptionCause::COPROCESSOR_ERROR);
}
