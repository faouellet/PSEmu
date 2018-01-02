#ifndef R3000A_H
#define R3000A_H

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
    uint8_t LoadByte(uint32_t address);
    uint16_t LoadHalfWord(uint32_t address);
    uint32_t LoadWord(uint32_t address);
    void StoreByte(uint32_t address, uint8_t value);
    void StoreHalfWord(uint32_t address, uint16_t value);
    void StoreWord(uint32_t address, uint32_t value);
    void Branch(uint32_t offset);
    bool WouldOverflow(int32_t lhs, int32_t rhs, std::function<int32_t(int32_t,int32_t)>&& func) const;
    void SetRegister(uint32_t registerIndex, uint32_t value);
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
    static constexpr const uint32_t PRIMARY_OPCODE_PATTERN = 0xFC000000;
    static constexpr const uint32_t SECONDARY_OPCODE_PATTERN = 0x0000003F;

    std::unordered_map<Opcode, void (R3000A::*)(Instruction)> m_opTable;
    std::array<uint32_t, 32> m_registers;  /**< CPU registers */
    std::array<uint32_t, 32> m_outputRegisters;  /**< CPU registers */
    uint32_t m_pc;                         /**< Program counter. Points to the next instruction */
    uint32_t m_nextPC;                     /**< Next value for the PC. Used to simulate the branch delay slot */
    uint32_t m_currentPC;                  /**< Address of the instruction currently being executed.
                                                     Used for setting the EPC in exceptions. */
    uint32_t m_hi;                         /**< Multiplication 64 bit high result or division remainder */
    uint32_t m_lo;                         /**< Multiplication 64 bit low result or division quotient */

    Interconnect m_interconnect;
    Instruction m_nextInst; /**< Next instruction to execute. Used to simulate the branch delay slot */
    std::pair<uint32_t, uint32_t> m_pendingLoad; /**< Load initiated by the current instruction */

    uint32_t m_sr;     /**< Cop0 register 12: Status register
                                 It is used to mask exceptions and control the cache behavior */
    uint32_t m_cause;  /**< Cop0 register 13: Cause register */
    uint32_t m_epc;    /**< Cop0 register 14: EPC register */

    bool m_isBranching;     /**< Set by the current instruction if a branch occured and the next instruction
                                 will be in the delay slot */
    bool m_isInDelaySlot;   /**< Set if the current instruction executes in the delay slot */
};

}   // end namespace PSEmu

#endif // R3000A_H