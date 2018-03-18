#ifndef DISASM_H
#define DISASM_H

#include <string>

namespace PSEmu
{

class Instruction;

std::string GetDebugString(const Instruction& inst);

}

#endif //  DISASM_H