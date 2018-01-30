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

// Add a breakpoint that will trigger when the CPU attempts to read from <address>
void Debugger::AddReadWatch(uint32_t address)
{
    if (std::find(m_readWatches.cbegin(), m_readWatches.cend(), address) == m_readWatches.cend())
    {
        m_readWatches.push_back(address);
    }
}

// Delete read watchpoint at <address>. Does nothing if no watchpoint was set for this address
void Debugger::DeleteReadWatch(uint32_t address)
{
    auto foundIt = std::find(m_readWatches.cbegin(), m_readWatches.cend(), address);
    if(foundIt != m_readWatches.cend())
    {
        m_readWatches.erase(foundIt);
    }
}

// Add a breakpoint that will trigger when the CPU attempts to write at <address>
void Debugger::AddWriteWatch(uint32_t address)
{
    if (std::find(m_writeWatches.cbegin(), m_writeWatches.cend(), address) == m_writeWatches.cend())
    {
        m_writeWatches.push_back(address);
    }
}

// Delete write watchpoint at <address>. Does nothing if no watchpoint was set for this address
void Debugger::DeleteWriteWatch(uint32_t address)
{
    auto foundIt = std::find(m_writeWatches.cbegin(), m_writeWatches.cend(), address);
    if(foundIt != m_writeWatches.cend())
    {
        m_writeWatches.erase(foundIt);
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

void Debugger::OnMemoryRead(const R3000A& cpu, uint32_t address) const
{
    auto foundIt = std::find(m_readWatches.cbegin(), m_readWatches.cend(), address);
    if(foundIt != m_readWatches.cend())
    {
        Debug(cpu);
    }
}

void Debugger::OnMemoryWrite(const R3000A& cpu, uint32_t address) const
{
    auto foundIt = std::find(m_writeWatches.cbegin(), m_writeWatches.cend(), address);
    if(foundIt != m_writeWatches.cend())
    {
        Debug(cpu);
    }
}

void Debugger::Debug(const R3000A&) const
{

}