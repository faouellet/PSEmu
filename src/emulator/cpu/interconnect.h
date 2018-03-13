#ifndef INTERCONNECT_H
#define INTERCONNECT_H

#include "../memory/bios.h"
#include "../memory/dma.h"
#include "../memory/memorymap.h"
#include "../memory/ram.h"
#include "../video/gpu.h"

#include <cassert>

namespace PSEmu
{

class Interconnect
{
public:
    explicit Interconnect(BIOS bios);

public:
    template <typename TSize>
    TSize Load(uint32_t address)
    {
        static_assert(std::is_integral_v<TSize>);

        const uint32_t physAddr = GetPhysicalAddress(address);

        if (auto offset = BIOS_RANGE.Contains(physAddr))
        {
            return m_bios.Load<TSize>(*offset);
        }
        else if (auto offset = RAM_RANGE.Contains(physAddr))
        {
            return m_ram.Load<TSize>(*offset);
        }
        else if (auto offset = IRQ_CONTROL_RANGE.Contains(physAddr))
        {
            return 0;
        }
        else if (auto offset = DMA_RANGE.Contains(physAddr))
        {
            return m_dma.RegisterRead(address);
        }
        else if (auto offset = GPU_RANGE.Contains(physAddr))
        {
            // Load from GPU only works with words
            // TODO: Review how to enable/disable loads depending on the number of bytes demanded
            if constexpr(sizeof(TSize) == 4)
            {
                // TODO: Not implemented yet! Placeholder for now!
                if (*offset == 4)
                {
                    // GPUSTAT: Set bit 28 to signal that the GPU is ready to receive DMA blocks
                    return 0x1C000000;
                }
            }
            
            return 0;
        }
        else if (auto offset = EXPANSION_1_RANGE.Contains(physAddr))
        {
            return 0xFF;
        }

        // TODO: PANIC!!!
        return 0;
    }

    template <typename TSize>
    void Store(uint32_t address, TSize value)
    {
        static_assert(std::is_integral_v<TSize>);

        const uint32_t physAddr = GetPhysicalAddress(address);

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
            m_dma.RegisterWrite(address, value);
        }
        else if (auto offset = GPU_RANGE.Contains(physAddr))
        {
            switch (*offset)
            {
                case 0: m_gpu.SetGP0(value); break;
                default: assert(false && "Unhandled GPU write");
            }
        }
        else if (auto offset = TIMERS_RANGE.Contains(physAddr))
        {
            // TODO: Not implemented yet!
        }
        else if (auto offset = RAM_RANGE.Contains(physAddr))
        {
            m_ram.Store<TSize>(*offset, value);
        }
        else if (EXPANSION_2_RANGE.Contains(physAddr) != std::nullopt)
        {
            // TODO: Not implemented yet
        }
        else if (SPU_RANGE.Contains(physAddr) != std::nullopt)
        {
            // TODO: Not implemented yet
        }

        // TODO: PANIC!!!
    }

private:
    uint32_t GetPhysicalAddress(uint32_t virtAddr);

private:
    BIOS m_bios;
    RAM m_ram;
    GPU m_gpu;
    DMA m_dma;
};

} // end namespace PSEmu

#endif // INTERCONNECT_H