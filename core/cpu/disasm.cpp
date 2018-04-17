#include "disasm.h"

#include "instruction.h"

#include <cassert>
#include <iostream>
#include <sstream>

namespace
{

template <typename TStream, typename ... Ts>
void DisassembleRegisterTypeInstruction(TStream& stream,
                                        std::string instOp,
                                        Ts&&... regs)
{
    stream << instOp;
    ((stream << " $" << regs << ","), ...);
}

template <typename TStream>
void DisassembleMemoryInstruction(TStream& stream, 
                                  std::string instOp, 
                                  PSEmu::Instruction inst)
{
    stream << instOp
           << " $" 
           << inst.GetRt() 
           << " " 
           << std::hex << "0x" << inst.GetImm() 
           << std::dec << " ($" 
           << inst.GetRs() 
           << ")";
}

template <typename TStream, typename ... Ts>
void DisassembleImmediateTypeInstruction(TStream& stream,
                                         std::string instOp,
                                         uint32_t imm,
                                         Ts&&... regs)
{
    DisassembleRegisterTypeInstruction(stream, std::move(instOp), std::forward<Ts>(regs)...);
    stream << " 0x" << std::hex << imm;
}

}   // end anoymous namespace

namespace PSEmu
{

std::string DisassembleInstruction(const Instruction& inst)
{
    std::ostringstream instStream;

    switch(inst.GetOp())
    {
        case LUI:     DisassembleImmediateTypeInstruction(instStream, "LUI", inst.GetImm(), inst.GetRt()); break;
        case ORI:     DisassembleImmediateTypeInstruction(instStream, "ORI", inst.GetImm(), inst.GetRt(), inst.GetRs()); break;
        case SW:      DisassembleMemoryInstruction(instStream, "SW", inst); break;
        case SLL:     DisassembleImmediateTypeInstruction(instStream, "SLL", inst.GetShamt(), inst.GetRd(), inst.GetRs()); break;
        case ADDIU:   DisassembleImmediateTypeInstruction(instStream, "ADDIU", inst.GetImm(), inst.GetRt(), inst.GetRs()); break;
        case J:       DisassembleImmediateTypeInstruction(instStream, "J", inst.GetImmJump()); break;
        case OR:      DisassembleRegisterTypeInstruction(instStream, "OR", inst.GetRd(), inst.GetRs(), inst.GetRt()); break;
        case MTC0:    DisassembleRegisterTypeInstruction(instStream, "MTC0", inst.GetRt(), inst.GetRd()); break;
        case BNE:     DisassembleImmediateTypeInstruction(instStream, "BNE", inst.GetImm(), inst.GetRt(), inst.GetRs()); break;
        case ADDI:    DisassembleImmediateTypeInstruction(instStream, "ADDI", inst.GetImm(), inst.GetRt(), inst.GetRs()); break;
        case LW:      DisassembleMemoryInstruction(instStream, "LW", inst); break;
        case SLTU:    DisassembleRegisterTypeInstruction(instStream, "SLTU", inst.GetRd(), inst.GetRs(), inst.GetRt()); break;
        case ADDU:    DisassembleRegisterTypeInstruction(instStream, "ADDU", inst.GetRd(), inst.GetRs(), inst.GetRt()); break;
        case SH:      DisassembleMemoryInstruction(instStream, "SH", inst); break;
        case JAL:     DisassembleImmediateTypeInstruction(instStream, "JAL", inst.GetImmJump()); break;
        case ANDI:    DisassembleImmediateTypeInstruction(instStream, "ANDI", inst.GetImm(), inst.GetRt(), inst.GetRs()); break;
        case SB:      DisassembleMemoryInstruction(instStream, "SB", inst); break;
        case JR:      DisassembleRegisterTypeInstruction(instStream, "JR", inst.GetRs()); break;
        case LB:      DisassembleMemoryInstruction(instStream, "LB", inst); break;
        case BEQ:     DisassembleImmediateTypeInstruction(instStream, "BEQ", inst.GetImm(), inst.GetRt(), inst.GetRs()); break;
        case MFC0:    DisassembleRegisterTypeInstruction(instStream, "MFC0", inst.GetRt(), inst.GetRd()); break;
        case AND:     DisassembleRegisterTypeInstruction(instStream, "AND", inst.GetRd(), inst.GetRs(), inst.GetRt()); break;
        case ADD:     DisassembleRegisterTypeInstruction(instStream, "ADD", inst.GetRd(), inst.GetRs(), inst.GetRt()); break;
        case BGTZ:    DisassembleImmediateTypeInstruction(instStream, "BGTZ", inst.GetImm(), inst.GetRs()); break;
        case BLEZ:    DisassembleImmediateTypeInstruction(instStream, "BLEZ", inst.GetImm(), inst.GetRs()); break;
        case LBU:     DisassembleMemoryInstruction(instStream, "LBU", inst); break;
        case JALR:    DisassembleRegisterTypeInstruction(instStream, "JALR", inst.GetRs(), inst.GetRd()); break;
        case BLTZ:    DisassembleImmediateTypeInstruction(instStream, "BLTZ", inst.GetImm(), inst.GetRs()); break;
        case BLTZAL:  DisassembleImmediateTypeInstruction(instStream, "BLTZAL", inst.GetImm(), inst.GetRs()); break;
        case BGEZ:    DisassembleImmediateTypeInstruction(instStream, "BGEZ", inst.GetImm(), inst.GetRs()); break;
        case BGEZAL:  DisassembleImmediateTypeInstruction(instStream, "BGEZAL", inst.GetImm(), inst.GetRs()); break;
        case SLTI:    DisassembleImmediateTypeInstruction(instStream, "SLTI", inst.GetImm(), inst.GetRt(), inst.GetRs()); break;
        case SUBU:    DisassembleRegisterTypeInstruction(instStream, "SUBU", inst.GetRd(), inst.GetRs(), inst.GetRt()); break;
        case SRA:     DisassembleImmediateTypeInstruction(instStream, "SRA", inst.GetShamt(), inst.GetRd(), inst.GetRt()); break;
        case DIV:     DisassembleRegisterTypeInstruction(instStream, "DIV", inst.GetRs(), inst.GetRt()); break;
        case MFLO:    DisassembleRegisterTypeInstruction(instStream, "MFLO", inst.GetRd()); break;
        case SRL:     DisassembleImmediateTypeInstruction(instStream, "SRL", inst.GetShamt(), inst.GetRd(), inst.GetRt()); break;
        case SLTIU:   DisassembleImmediateTypeInstruction(instStream, "SLTIU", inst.GetImm(), inst.GetRt(), inst.GetRs()); break;
        case DIVU:    DisassembleRegisterTypeInstruction(instStream, "DIVU", inst.GetRs(), inst.GetRt()); break;
        case MFHI:    DisassembleRegisterTypeInstruction(instStream, "MFHI", inst.GetRd()); break;
        case SLT:     DisassembleRegisterTypeInstruction(instStream, "SLT", inst.GetRd(), inst.GetRs(), inst.GetRt()); break;
        case SYSCALL: return "SYSCALL";
        case MTLO:    DisassembleRegisterTypeInstruction(instStream, "MTLO", inst.GetRd()); break;
        case MTHI:    DisassembleRegisterTypeInstruction(instStream, "MTHI", inst.GetRd()); break;
        case RFE :    return "RFE";
        case LHU :    DisassembleMemoryInstruction(instStream, "LHU", inst); break;
        case SLLV:    DisassembleRegisterTypeInstruction(instStream, "SLLV", inst.GetRd(), inst.GetRs(), inst.GetRt()); break;
        case LH:      DisassembleMemoryInstruction(instStream, "LH", inst); break;
        case NOR:     DisassembleRegisterTypeInstruction(instStream, "NOR", inst.GetRd(), inst.GetRs(), inst.GetRt()); break;
        case SRAV:    DisassembleRegisterTypeInstruction(instStream, "SRAV", inst.GetRd(), inst.GetRs(), inst.GetRt()); break;
        case SRLV:    DisassembleRegisterTypeInstruction(instStream, "SRLV", inst.GetRd(), inst.GetRs(), inst.GetRt()); break;
        case MULTU:   DisassembleRegisterTypeInstruction(instStream, "MULTU", inst.GetRs(), inst.GetRt()); break;
        case XOR:     DisassembleRegisterTypeInstruction(instStream, "XOR", inst.GetRd(), inst.GetRs(), inst.GetRt()); break;
        case BREAK:   return "BREAK";
        case MULT:    DisassembleRegisterTypeInstruction(instStream, "MULT", inst.GetRs(), inst.GetRt()); break;
        case SUB:     DisassembleRegisterTypeInstruction(instStream, "SUB", inst.GetRd(), inst.GetRs(), inst.GetRt()); break;
        case XORI:    DisassembleImmediateTypeInstruction(instStream, "XORI", inst.GetImm(), inst.GetRt(), inst.GetRs()); break;
        case COP1:    return "COP1";
        case COP2:    return "COP2";
        case COP3:    return "COP3";
        case LWL:     DisassembleMemoryInstruction(instStream, "LWL", inst); break;
        case LWR:     DisassembleMemoryInstruction(instStream, "LWR", inst); break;
        case SWL:     DisassembleMemoryInstruction(instStream, "SWL", inst); break;
        case SWR:     DisassembleMemoryInstruction(instStream, "SWR", inst); break;
        case LWC0:    DisassembleMemoryInstruction(instStream, "LWC0", inst); break;
        case LWC1:    DisassembleMemoryInstruction(instStream, "LWC1", inst); break;
        case LWC2:    DisassembleMemoryInstruction(instStream, "LWC2", inst); break;
        case LWC3:    DisassembleMemoryInstruction(instStream, "LWC3", inst); break;
        case SWC0:    DisassembleMemoryInstruction(instStream, "SWC0", inst); break;
        case SWC1:    DisassembleMemoryInstruction(instStream, "SWC1", inst); break;
        case SWC2:    DisassembleMemoryInstruction(instStream, "SWC2", inst); break;
        case SWC3:    DisassembleMemoryInstruction(instStream, "SWC3", inst); break;
        default:
            assert(false && "Unhandled opcode");
            std::cout << inst.GetOp() << std::endl;
            return "";
    }

    instStream << std::endl;
    return instStream.str();
}

}   // end namespace PSEmu
