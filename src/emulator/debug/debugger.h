#ifndef DEBUGGER_H
#define DEBUGGER_H

#include <cstdint>
#include <vector>

namespace PSEmu
{

class R3000A;

class Debugger
{
public:
    void AddBreakpoint(uint32_t address);
    void DeleteBreakpoint(uint32_t address);
    void OnPCChange(const R3000A& cpu) const;

private:
    void Debug(const R3000A& cpu) const;

private:
    // Vector containing all active breakpoint addresses
    std::vector<uint32_t> m_breakpoints;
};

}   // end namespace PSEmu

#endif // DEBUGGER_H