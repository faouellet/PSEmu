#include "debugger.h"

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