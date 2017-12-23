#ifndef R3000A_H
#define R3000A_H

#include "../utils/types.h"
#include "instructions.h"
#include "interconnect.h"
#include "opcodes.h"

#include <array>
#include <functional>
#include <unordered_map>

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
    R3000A(Interconnect&& interconnect);

    // It should not be possible to copy or move this class
    R3000A(const R3000A&) = delete;
    R3000A& operator=(const R3000A&) = delete;

    R3000A(R3000A&&) = delete;
    R3000A& operator=(R3000A&&) = delete;

public:
    void Step();
    void Reset();

private:
    void InitOpTable();
    Utils::UInt8 LoadByte(Utils::UInt32 address);
    Utils::UInt16 LoadHalfWord(Utils::UInt32 address);
    Utils::UInt32 LoadWord(Utils::UInt32 address);
    void StoreByte(Utils::UInt32 address, Utils::UInt8 value);
    void StoreHalfWord(Utils::UInt32 address, Utils::UInt16 value);
    void StoreWord(Utils::UInt32 address, Utils::UInt32 value);
    void Branch(Utils::UInt32 offset);
    bool WouldOverflow(Utils::Int32 lhs, Utils::Int32 rhs, std::function<Utils::Int32(Utils::Int32,Utils::Int32)>&& func) const;
    void SetRegister(Utils::UInt32 registerIndex, Utils::UInt32 value);
    void TriggerException(ExceptionCause cause);

private:
    void ExecuteLUI(Instruction inst);
    void ExecuteORI(Instruction inst);
    void ExecuteSW(Instruction inst);
    void ExecuteSLL(Instruction inst);
    void ExecuteADDIU(Instruction inst);
    void ExecuteJ(Instruction inst);
    void ExecuteOR(Instruction inst);
    void ExecuteMTC0(Instruction inst);
    void ExecuteBNE(Instruction inst);
    void ExecuteADDI(Instruction inst);
    void ExecuteLW(Instruction inst);
    void ExecuteSLTU(Instruction inst);
    void ExecuteADDU(Instruction inst);
    void ExecuteSH(Instruction inst);
    void ExecuteJAL(Instruction inst);
    void ExecuteANDI(Instruction inst);
    void ExecuteSB(Instruction inst);
    void ExecuteJR(Instruction inst);
    void ExecuteLB(Instruction inst);
    void ExecuteBEQ(Instruction inst);
    void ExecuteMFC0(Instruction inst);
    void ExecuteAND(Instruction inst);
    void ExecuteADD(Instruction inst);
    void ExecuteBGTZ(Instruction inst);
    void ExecuteBLEZ(Instruction inst);
    void ExecuteLBU(Instruction inst);
    void ExecuteJALR(Instruction inst);
    void ExecuteBLTZ(Instruction inst);
    void ExecuteBLTZAL(Instruction inst);
    void ExecuteBGEZ(Instruction inst);
    void ExecuteBGEZAL(Instruction inst);
    void ExecuteSLTI(Instruction inst);
    void ExecuteSUBU(Instruction inst);
    void ExecuteSRA(Instruction inst);
    void ExecuteDIV(Instruction inst);
    void ExecuteMFLO(Instruction inst);
    void ExecuteSRL(Instruction inst);
    void ExecuteSLTIU(Instruction inst);
    void ExecuteDIVU(Instruction inst);
    void ExecuteMFHI(Instruction inst);
    void ExecuteSLT(Instruction inst);
    void ExecuteSYSCALL(Instruction inst);
    void ExecuteMTLO(Instruction inst);
    void ExecuteMTHI(Instruction inst);
    void ExecuteRFE(Instruction inst);
    void ExecuteLHU(Instruction inst);
    void ExecuteSLLV(Instruction inst);
    void ExecuteLH(Instruction inst);
    void ExecuteNOR(Instruction inst);
    void ExecuteSRAV(Instruction inst);
    void ExecuteSRLV(Instruction inst);
    void ExecuteMULTU(Instruction inst);
    void ExecuteXOR(Instruction inst);
    void ExecuteBREAK(Instruction inst);
    void ExecuteMULT(Instruction inst);
    void ExecuteSUB(Instruction inst);
    void ExecuteXORI(Instruction inst);
    void ExecuteCOP1(Instruction inst);
    void ExecuteCOP2(Instruction inst);
    void ExecuteCOP3(Instruction inst);
    void ExecuteLWL(Instruction inst);
    void ExecuteLWR(Instruction inst);
    void ExecuteSWL(Instruction inst);
    void ExecuteSWR(Instruction inst);
    void ExecuteLWC0(Instruction inst);
    void ExecuteLWC1(Instruction inst);
    void ExecuteLWC2(Instruction inst);
    void ExecuteLWC3(Instruction inst);
    void ExecuteSWC0(Instruction inst);
    void ExecuteSWC1(Instruction inst);
    void ExecuteSWC2(Instruction inst);
    void ExecuteSWC3(Instruction inst);

private:
    static constexpr const Utils::UInt32 OPCODE_PATTERN = 0xFC00003F;

    std::unordered_map<Opcode, void (R3000A::*)(Instruction)> m_opTable;
    std::array<Utils::UInt32, 32> m_registers;  /**< CPU registers */
    std::array<Utils::UInt32, 32> m_outputRegisters;  /**< CPU registers */
    Utils::UInt32 m_pc;                         /**< Program counter. Points to the next instruction */
    Utils::UInt32 m_nextPC;                     /**< Next value for the PC. Used to simulate the branch delay slot */
    Utils::UInt32 m_currentPC;                  /**< Address of the instruction currently being executed.
                                                     Used for setting the EPC in exceptions. */
    Utils::UInt32 m_hi;                         /**< Multiplication 64 bit high result or division remainder */
    Utils::UInt32 m_lo;                         /**< Multiplication 64 bit low result or division quotient */

    Interconnect m_interconnect;
    Instruction m_nextInst; /**< Next instruction to execute. Used to simulate the branch delay slot */
    std::pair<Utils::UInt32, Utils::UInt32> m_pendingLoad; /**< Load initiated by the current instruction */

    Utils::UInt32 m_sr;     /**< Cop0 register 12: Status register
                                 It is used to mask exceptions and control the cache behavior */
    Utils::UInt32 m_cause;  /**< Cop0 register 13: Cause register */
    Utils::UInt32 m_epc;    /**< Cop0 register 14: EPC register */

    bool m_isBranching;     /**< Set by the current instruction if a branch occured and the next instruction
                                 will be in the delay slot */
    bool m_isInDelaySlot;   /**< Set if the current instruction executes in the delay slot */
};

}   // end namespace PSEmu

#endif // R3000A_H