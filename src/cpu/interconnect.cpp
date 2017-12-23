#include "interconnect.h"

#include "../memory/memorymap.h"

#include <cassert>

using namespace PSEmu;

namespace
{

// TODO: Document
Utils::UInt32 GetPhysicalAddress(Utils::UInt32 virtAddr)
{
    return virtAddr & REGION_MASK[virtAddr >> 29];
}

}   // end anonymous namespace

Interconnect::Interconnect(BIOS&& bios) : m_bios{ std::move(bios) } { }

Utils::UInt8 Interconnect::LoadByte(Utils::UInt32 address)
{
    const Utils::UInt32 physAddr = GetPhysicalAddress(address);

    if (auto offset = BIOS_RANGE.Contains(physAddr))
    {
        return m_bios.LoadByte(*offset);
    }
    else if (auto offset = EXPANSION_1_RANGE.Contains(physAddr))
    {
        return 0xFF;
    }

    // TODO: PANIC!!!
    return 0;
}

Utils::UInt16 Interconnect::LoadHalfWord(Utils::UInt32 address)
{
    const Utils::UInt32 physAddr = GetPhysicalAddress(address);

    if (auto offset = IRQ_CONTROL_RANGE.Contains(physAddr))
    {
        return 0;
    }
}

Utils::UInt32 Interconnect::LoadWord(Utils::UInt32 address)
{
    const Utils::UInt32 physAddr = GetPhysicalAddress(address);

    if (auto offset = BIOS_RANGE.Contains(physAddr))
    {
        return m_bios.LoadWord(*offset);
    }
    else if (auto offset = RAM_RANGE.Contains(physAddr))
    {
        return m_ram.LoadWord(*offset);
    }
    else if (auto offset = IRQ_CONTROL_RANGE.Contains(physAddr))
    {
        return 0;
    }
    else if (auto offset = DMA_RANGE.Contains(physAddr))
    {
        return m_dma.DMARegisterRead(address);
    }
    else if (auto offset = GPU_RANGE.Contains(physAddr))
    {
        // TODO: Not implemented yet! Placeholder for now!
        if (*offset == 4)
        {
            // GPUSTAT: Set bit 28 to signal that the GPU is ready to receive DMA blocks
            return 0x1C000000;
        }
        return 0;
    }

    // TODO: PANIC!!!
    return 0;
}

void Interconnect::StoreByte(Utils::UInt32 address, Utils::UInt8 value)
{
    const Utils::UInt32 physAddr = GetPhysicalAddress(address);

    if (EXPANSION_2_RANGE.Contains(physAddr) != std::nullopt)
    {
        // TODO: Not implemented yet
    }
    else if (auto offset = RAM_RANGE.Contains(physAddr))
    {
        m_ram.StoreByte(*offset, value);
    }

    // TODO: PANIC !!!
}

void Interconnect::StoreHalfWord(Utils::UInt32 address, Utils::UInt16 value)
{
    const Utils::UInt32 physAddr = GetPhysicalAddress(address);

    if (SPU_RANGE.Contains(physAddr) != std::nullopt)
    {
        // TODO: Not implemented yet
    }
    else if (auto offset = TIMERS_RANGE.Contains(physAddr))
    {
        // TODO: Not implemented yet
    }
    else if (auto offset = RAM_RANGE.Contains(physAddr))
    {
        m_ram.StoreHalfWord(*offset, value);
    }

    // TODO: PANIC!!!
}

void Interconnect::StoreWord(Utils::UInt32 address, Utils::UInt32 value)
{
    const Utils::UInt32 physAddr = GetPhysicalAddress(address);

    if (MEMCONTROL_RANGE.Contains(physAddr) != std::nullopt)
    {
        // TODO: Handle error
    }
    else if (IRQ_CONTROL_RANGE.Contains(physAddr) != std::nullopt)
    {
        // TODO: Not implemented yet
    }
    else if (auto offset = DMA_RANGE.Contains(physAddr))
    {
        m_dma.DMARegisterWrite(address, value);
    }
    else if (auto offset = GPU_RANGE.Contains(physAddr))
    {
        switch (*offset)
        {
            case 0: m_gpu.SetGP0(value);
            default: assert(false && "Unhandled GPU write");
        }
    }
    else if (auto offset = TIMERS_RANGE.Contains(physAddr))
    {
        // TODO: Not implemented yet!
    }

    // TODO: PANIC!!!
}
