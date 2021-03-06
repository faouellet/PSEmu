#ifndef R3000A_H
#define R3000A_H

#include "../debug/debugger.h"
#include "instruction.h"
#include "interconnect.h"

#include <array>
#include <functional>

bool WouldOverflow(int32_t lhs, int32_t rhs, std::function<int32_t(int32_t,int32_t)> func);

namespace PSEmu
{

class R3000A
{
private:
    enum class ExceptionCause
    {
        LOAD_ADDRESS_ERROR  = 0x4,
        STORE_ADDRESS_ERROR = 0x5,
        SYSCALL             = 0x8,
        BREAK               = 0x9,
        ILLEGAL_INSTRUCTION = 0xA,
        COPROCESSOR_ERROR   = 0xB,
        OVERFLOW            = 0xC,
    };

public:
    R3000A(Interconnect interconnect, Debugger debugger);

    // It should not be possible to copy or move this class
    R3000A(const R3000A&) = delete;
    R3000A& operator=(const R3000A&) = delete;

    R3000A(R3000A&&) = delete;
    R3000A& operator=(R3000A&&) = delete;

public:
    void Step();
    void Reset();

public:
    uint32_t GetPC() const;
    const std::array<uint32_t, 32>& GetRegisters() const;

private:
    void Branch(uint32_t offset);
    void SetRegister(uint32_t registerIndex, uint32_t value);
    void TriggerException(ExceptionCause cause);

private:
    void ExecuteLUI(Instruction inst);
    void ExecuteSLL(Instruction inst);
    void ExecuteJ(Instruction inst);
    void ExecuteMTC0(Instruction inst);
    void ExecuteBNE(Instruction inst);
    void ExecuteJAL(Instruction inst);
    void ExecuteJR(Instruction inst);
    void ExecuteBEQ(Instruction inst);
    void ExecuteMFC0(Instruction inst);
    void ExecuteJALR(Instruction inst);
    void ExecuteSLTI(Instruction inst);
    void ExecuteSRA(Instruction inst);
    void ExecuteDIV(Instruction inst);
    void ExecuteSRL(Instruction inst);
    void ExecuteDIVU(Instruction inst);
    void ExecuteSLT(Instruction inst);
    void ExecuteRFE(Instruction inst);
    void ExecuteSLLV(Instruction inst);
    void ExecuteNOR(Instruction inst);
    void ExecuteSRAV(Instruction inst);
    void ExecuteSRLV(Instruction inst);
    void ExecuteMULTU(Instruction inst);
    void ExecuteMULT(Instruction inst);
    void ExecuteLWL(Instruction inst);
    void ExecuteLWR(Instruction inst);
    void ExecuteSWL(Instruction inst);
    void ExecuteSWR(Instruction inst);

private:
    // TODO: Load should probably return an optional value since it'll be ignored when the cache is isolated
    template <typename TSize>
    TSize Load(uint32_t address);

    template <typename TSize>
    void Store(uint32_t address, TSize value);

    template <typename TOperator, typename TDecoder>
    void ExecuteALU(TOperator&& op, TDecoder&& dec, Instruction inst);

    template <typename TOperator, typename TOverflow, typename TDecoder>
    void ExecuteTrappingALU(TOperator&& op, TOverflow&& overflowHandler, TDecoder&& dec, Instruction inst);

    template <typename TComparator>
    void ExecuteBranch(TComparator&& comp, Instruction inst);

    template <typename TComparator>
    void ExecuteBranchAndLink(TComparator&& comp, Instruction inst);

    template <typename TLoad, typename TExtension = TLoad>
    void ExecuteLoad(Instruction inst);

    template <typename TSize>
    void ExecuteStore(Instruction inst);

private:
    std::array<uint32_t, 32> m_registers;        /**< CPU registers */  
    std::array<uint32_t, 32> m_outputRegisters;  /**< CPU registers. Used ti simulate the load delay slot */
    uint32_t m_pc;                /**< Program counter. Points to the next instruction */
    uint32_t m_nextPC;            /**< Next value for the PC. Used to simulate the branch delay slot */
    uint32_t m_currentPC;         /**< Address of the instruction currently being executed.
                                       Used for setting the EPC in exceptions. */
    uint32_t m_hi;                /**< Multiplication 64 bit high result or division remainder */
    uint32_t m_lo;                /**< Multiplication 64 bit low result or division quotient */

    Interconnect m_interconnect;
    Instruction m_nextInst;       /**< Next instruction to execute. Used to simulate the branch delay slot */
    std::pair<uint32_t, uint32_t> m_pendingLoad; /**< Load initiated by the current instruction */

    uint32_t m_sr;                /**< Cop0 register 12: Status register
                                        It is used to mask exceptions and control the cache behavior */
    uint32_t m_cause;             /**< Cop0 register 13: Cause register */
    uint32_t m_epc;               /**< Cop0 register 14: EPC register */

    bool m_isBranching;           /**< Set by the current instruction if a branch occured and the next instruction
                                       will be in the delay slot */
    bool m_isInDelaySlot;         /**< Set if the current instruction executes in the delay slot */

    Debugger m_debugger;
};

template <typename TSize>
TSize R3000A::Load(uint32_t address)
{
    static_assert(std::is_integral_v<TSize>);

    if ((m_sr & 0x10000) != 0)
    {
        // TODO: Msg -> Cache is isolated, load ignored
        return -1;
    }

    // Address must be aligned to the number of bytes we want to load
    if (address % sizeof(TSize) != 0)
    {
        TriggerException(ExceptionCause::LOAD_ADDRESS_ERROR);
    }

    return m_interconnect.Load<TSize>(address);
}

template <typename TSize>
void R3000A::Store(uint32_t address, TSize value)
{
    static_assert(std::is_integral_v<TSize>);

    if ((m_sr & 0x10000) != 0)
    {
        // TODO: Msg -> Cache is isolated, write ignored
        return;
    }

    // Address must be aligned to the number of bytes we want to load
    if (address % sizeof(TSize) != 0)
    {
        TriggerException(ExceptionCause::STORE_ADDRESS_ERROR);
    }

    m_interconnect.Store<TSize>(address, value);
}

template <typename TOperator, typename TDecoder>
void R3000A::ExecuteALU(TOperator&& op, TDecoder&& dec, Instruction inst)
{
    const auto& [destReg, lhs, rhs] = dec(m_registers, inst);
    SetRegister(destReg, op(lhs, rhs));
}

template <typename TOperator, typename TOverflow, typename TDecoder>
void R3000A::ExecuteTrappingALU(TOperator&& op, TOverflow&& overflowHandler, TDecoder&& dec, Instruction inst)
{
    const auto& [destReg, lhs, rhs] = dec(m_registers, inst);

    // Check for overflow.
    if (WouldOverflow(lhs, rhs, overflowHandler))
    {
        TriggerException(ExceptionCause::OVERFLOW);
    }

    SetRegister(destReg, op(lhs, rhs));
}

template <typename TComparator>
void R3000A::ExecuteBranch(TComparator&& comp, Instruction inst)
{
    const int32_t value = m_registers[inst.GetRs()];
    if (comp(value, 0))
    {
        Branch(inst.GetImmSe());
    }
}

template <typename TComparator>
void R3000A::ExecuteBranchAndLink(TComparator&& comp, Instruction inst)
{
    // Store return address in the RA register
    SetRegister(31, m_pc);

    ExecuteBranch(std::forward<TComparator>(comp), inst);
}

template <typename TLoad, typename TExtension>
void R3000A::ExecuteLoad(Instruction inst)
{
    uint32_t address = m_registers[inst.GetRs()] + inst.GetImmSe();

    // TODO: Subtle bug here and in all memory related operations
    //       The problem is that a load/store shouldn't be completed
    //       if an exception is triggered
    TExtension value = Load<TLoad>(address); 

    m_pendingLoad = {{inst.GetRt()}, value};
}

template <typename TSize>
void R3000A::ExecuteStore(Instruction inst)
{
    const uint32_t address = m_registers[inst.GetRs()] + inst.GetImmSe();
    const TSize value = m_registers[inst.GetRt()];
    Store<TSize>(address, value);
}

}   // end namespace PSEmu

#endif // R3000A_H