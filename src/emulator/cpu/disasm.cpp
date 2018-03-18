#include "disasm.h"

#include "instruction.h"

#include <cassert>
#include <iostream>

namespace PSEmu
{

std::string GetDebugString(const Instruction& inst)
{
    switch(inst.GetOp())
    {
        case LUI:     return "LUI";
        case ORI:     return "ORI";
        case SW:      return "SW";
        case SLL:     return "SLL";
        case ADDIU:   return "ADDIU";
        case J:       return "J";
        case OR:      return "OR";
        case MTC0:    return "MTC0";
        case BNE:     return "BNE";
        case ADDI:    return "ADDI";
        case LW:      return "LW";
        case SLTU:    return "SLTU";
        case ADDU:    return "ADDU";
        case SH:      return "SH";
        case JAL:     return "JAL";
        case ANDI:    return "ANDI";
        case SB:      return "SB";
        case JR:      return "JR";
        case LB:      return "LB";
        case BEQ:     return "BEQ";
        case MFC0:    return "MFC0";
        case AND:     return "AND";
        case ADD:     return "ADD";
        case BGTZ:    return "BGTZ";
        case BLEZ:    return "BLEZ";
        case LBU:     return "LBU";
        case JALR:    return "JALR";
        case BLTZ:    return "BLTZ";
        case BLTZAL:  return "BLTZAL";
        case BGEZ:    return "BGEZ ";
        case BGEZAL:  return "BGEZAL";
        case SLTI:    return "SLTI";
        case SUBU:    return "SUBU";
        case SRA:     return "SRA";
        case DIV:     return "DIV";
        case MFLO:    return "MFLO";
        case SRL:     return "SRL";
        case SLTIU:   return "SLTIU";
        case DIVU:    return "DIVU";
        case MFHI:    return "MFHI";
        case SLT:     return "SLT";
        case SYSCALL: return "SYSCALL";
        case MTLO:    return "MTLO";
        case MTHI:    return "MTHI";
        case RFE :    return "RFE";
        case LHU :    return "LHU";
        case SLLV:    return "SLLV";
        case LH:      return "LH";
        case NOR:     return "NOR";
        case SRAV:    return "SRAV";
        case SRLV:    return "SRLV";
        case MULTU:   return "MULTU";
        case XOR:     return "XOR";
        case BREAK:   return "BREAK";
        case MULT:    return "MULT";
        case SUB:     return "SUB:";
        case XORI:    return "XORI";
        case COP1:    return "COP1";
        case COP2:    return "COP2";
        case COP3:    return "COP3";
        case LWL:     return "LWL";
        case LWR:     return "LWR";
        case SWL:     return "SWL";
        case SWR:     return "SWR";
        case LWC0:    return "LWC0";
        case LWC1:    return "LWC1";
        case LWC2:    return "LWC2";
        case LWC3:    return "LWC3";
        case SWC0:    return "SWC0";
        case SWC1:    return "SWC1";
        case SWC2:    return "SWC2";
        case SWC3:    return "SWC3";
        default:
            assert(false && "Unhandled opcode");
            std::cout << inst.GetOp() << std::endl;
            return "";
    }
}

}   // end namespace PSEmu
