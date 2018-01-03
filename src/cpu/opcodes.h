#ifndef OPCODE_H
#define OPCODE_H

namespace PSEmu
{
    /**
     * @brief Opcode pattern. Represents both the primary (bits 26 to 31) and the secondary opcode (bits 0 to 5)
     * 
     */
    enum Opcode
    {
        // General
        J       = 0x08000000,
        JAL     = 0x0C000000,
        BEQ     = 0x10000000,
        BNE     = 0x14000000,
        BLEZ    = 0x18000000,
        BGTZ    = 0x1C000000,
        ADDI    = 0x20000000,
        ADDIU   = 0x24000000,
        SLTI    = 0x28000000,
        SLTIU   = 0x2C000000,
        ANDI    = 0x30000000,
        ORI     = 0x34000000,
        XORI    = 0x38000000,
        LUI     = 0x3C000000,
        COP1    = 0x44000000,
        COP2    = 0x48000000,
        COP3    = 0x4C000000,
        LB      = 0x80000000,
        LH      = 0x84000000,
        LWL     = 0x88000000,
        LW      = 0x8C000000,
        LBU     = 0x90000000,
        LHU     = 0x94000000,
        LWR     = 0x98000000,
        SB      = 0xA0000000,
        SH      = 0xA4000000,
        SWL     = 0xA8000000,
        SW      = 0xAC000000,
        SWR     = 0xB8000000,
        LWC0    = 0xC0000000,
        LWC1    = 0xC4000000,
        LWC2    = 0xC8000000,
        LWC3    = 0xCC000000,
        SWC0    = 0xDC000000,
        SWC1    = 0xE4000000,
        SWC2    = 0xE8000000,
        SWC3    = 0xEC000000,
    
        // Special
        SLL     = 0x00000000,
        SRL     = 0x00000002,
        SRA     = 0x00000003,
        SLLV    = 0x00000004,
        SRLV    = 0x00000006,
        SRAV    = 0x00000007,
        JR      = 0x00000008,
        JALR    = 0x00000009,
        SYSCALL = 0x0000000C,
        BREAK   = 0x0000000D,
        MFHI    = 0x00000010,
        MTHI    = 0x00000011,
        MFLO    = 0x00000012,
        MTLO    = 0x00000013,
        MULT    = 0x00000018,
        MULTU   = 0x00000019,
        DIV     = 0x0000001A,
        DIVU    = 0x0000001B,
        ADD     = 0x00000020,
        ADDU    = 0x00000021,
        SUB     = 0x00000022,
        SUBU    = 0x00000023,
        AND     = 0x00000024,
        OR      = 0x00000025,
        XOR     = 0x00000026,
        NOR     = 0x00000027,
        SLT     = 0x0000002A,
        SLTU    = 0x0000002B,

        // BCOND
        BLTZ    = 0x04000000,
        BGEZ    = 0x04010000,
        BLTZAL  = 0x04100000,
        BGEZAL  = 0x04110000,

        // COP0
        MTC0    = 0x40800000,
        MFC0    = 0x40000000,
        RFE     = 0x42000010,
    };
}   // end namespace PSEmu

#endif // OPCODE_H