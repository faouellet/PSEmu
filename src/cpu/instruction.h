#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

#include <cstdint>

namespace PSEmu
{

class Instruction
{
public:
    Instruction();
    Instruction(uint32_t val);

public:
    uint32_t GetImmSe() const;
    uint32_t GetImm() const;
    uint32_t GetOp() const;
    uint32_t GetRd() const;
    uint32_t GetRs() const;
    uint32_t GetRt() const;
    uint32_t GetTarget() const;
    uint32_t GetShamt() const;
    uint32_t GetFunct() const;

public:
    operator uint32_t() const;

private:
    uint32_t m_intRep;
};

} // end namespace PSEmu

#endif // INSTRUCTIONS_H