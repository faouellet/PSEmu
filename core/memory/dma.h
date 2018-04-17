#ifndef DMA_H
#define DMA_H

#include "channel.h"

#include <array>
#include <cstdint>

namespace PSEmu
{

class GPU;
class RAM;

enum class Port
{
    // Macroblock decoder input
    M_DEC_IN,
    // Macroblock decoder output
    M_DEC_OUT,
    // Graphics processing unit
    GPU,
    // CD-ROM drive
    CDROM,
    // Sound processing unit
    SPU,
    // Extension port
    PIO,
    // Used to clear the ordering table
    OTC,
};

class DMA
{
public:
    DMA(GPU& gpu, RAM& ram);

    // It should not be possible to copy an instance of this class
    DMA(const DMA&) = delete;
    DMA& operator=(const DMA&) = delete;

    // But it should be possible to move it
    DMA(DMA&&) = default;
    DMA& operator=(DMA&&) = default;

public:
    uint32_t RegisterRead(uint32_t offset) const;
    void RegisterWrite(uint32_t offset, uint32_t value);

    uint32_t GetControl() const;
    void SetControl(uint32_t value);

    bool GetIRQ() const;

    uint32_t GetInterrupt() const;
    void SetInterrupt(uint32_t value);

    Channel& GetChannel(Port port);
    const Channel& GetChannel(Port port) const;

    void DoMemoryTransfer(Port port);
    void DoMemoryBlockCopy(Port port);
    void DoLinkedListCopy(Port port);

private:
    // DMA control register
    uint32_t m_control;

    // Master IRQ enable
    bool m_IRQEnable;
    
    // IRQ enable for individual channels
    uint8_t m_channelIRQEnable;

    // IRQ flags for individual channels
    uint8_t m_channelIRQFlags;

    // When set the interrupt is active unconditionally
    // (even if m_IRQEnable is false)
    bool m_forceIRQ;

    // Bits [0:5] of the interrupt registers
    uint8_t m_dummy;

    // The 7 channel instances
    std::array<Channel, 7> m_channels; 

    RAM& m_ram;

    // 
    GPU& m_gpu;
};

}   // end namespace PSEmu

#endif // DMA_H