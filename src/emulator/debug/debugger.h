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

    void AddReadWatch(uint32_t address);
    void DeleteReadWatch(uint32_t address);

    void AddWriteWatch(uint32_t address);
    void DeleteWriteWatch(uint32_t address);

    void OnPCChange(const R3000A& cpu) const;
    void OnMemoryRead(const R3000A& cpu, uint32_t address) const;
    void OnMemoryWrite(const R3000A& cpu, uint32_t address) const;

private:
    void Debug(const R3000A& cpu) const;

private:
    // Vector containing all active breakpoint addresses
    std::vector<uint32_t> m_breakpoints;

    // Vector containing all active read watchpoint addresses
    std::vector<uint32_t> m_readWatches;

    // Vector containing all active write watchpoint addresses
    std::vector<uint32_t> m_writeWatches;
};

}   // end namespace PSEmu

#endif // DEBUGGER_H