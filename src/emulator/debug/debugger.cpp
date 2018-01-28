#include "debugger.h"

#include "../cpu/r3000a.h"


#include <algorithm>

using namespace PSEmu;

// Add a breakpoint that will trigger when the instruction at <address>
// is about to be executed
void Debugger::AddBreakpoint(uint32_t address)
{
    if (std::find(m_breakpoints.cbegin(), m_breakpoints.cend(), address) == m_breakpoints.cend())
    {
        m_breakpoints.push_back(address);
    }
}

// Delete breakpoint at <address>. Does nothing if no breakpoint was set for this address
void Debugger::DeleteBreakpoint(uint32_t address)
{
    auto foundIt = std::find(m_breakpoints.cbegin(), m_breakpoints.cend(), address);
    if(foundIt != m_breakpoints.cend())
    {
        m_breakpoints.erase(foundIt);
    }
}

// Called by the CPU when it's about to execute a new instruction. This function
// is called before *all* CPU instructions so it needs to be as fast as possible.
void Debugger::OnPCChange(const R3000A& cpu) const
{
    auto foundIt = std::find(m_breakpoints.cbegin(), m_breakpoints.cend(), cpu.GetPC());
    if(foundIt != m_breakpoints.cend())
    {
        Debug(cpu);
    }
}

void Debugger::Debug(const R3000A&) const
{

}