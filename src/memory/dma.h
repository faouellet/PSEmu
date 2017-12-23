#ifndef DMA_H
#define DMA_H

#include "channel.h"
#include "../utils/types.h"

#include <array>

namespace PSEmu
{

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
    DMA();

    // It should not be possible to copy an instance of this class
    DMA(const DMA&) = delete;
    DMA& operator=(const DMA&) = delete;

    // But it should be possible to move it
    DMA(DMA&&) = default;
    DMA& operator=(DMA&&) = default;

public:
    Utils::UInt32 DMARegisterRead(Utils::UInt32 offset) const;
    void DMARegisterWrite(Utils::UInt32 offset, Utils::UInt32 value);

    Utils::UInt32 GetControl() const;
    void SetControl(Utils::UInt32 value);

    bool GetIRQ() const;

    Utils::UInt32 GetInterrupt() const;
    void SetInterrupt(Utils::UInt32 value);

    Channel& GetChannel(Port port);
    const Channel& GetChannel(Port port) const;

    void DoMemoryTransfer(Port port);
    void DoMemoryBlockCopy(Port port);
    void DoLinkedListCopy(Port port);

private:
    // DMA control register
    Utils::UInt32 m_control;

    // Master IRQ enable
    bool m_IRQEnable;
    
    // IRQ enable for individual channels
    Utils::UInt8 m_channelIRQEnable;

    // IRQ flags for individual channels
    Utils::UInt8 m_channelIRQFlags;

    // When set the interrupt is active unconditionally
    // (even if m_IRQEnable is false)
    bool m_forceIRQ;

    // Bits [0:5] of the interrupt registers
    Utils::UInt8 m_dummy;

    // The 7 channel instances
    std::array<Channel, 7> m_channels; 
};

}   // end namespace PSEmu

#endif // DMA_H