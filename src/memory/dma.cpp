#include "dma.h"

#include <cassert>

using namespace PSEmu;

namespace
{

Port IndexToPort(Utils::UInt32 index)
{
    return static_cast<Port>(index);
}

Utils::UInt32 PortToIndex(Port port)
{
    using PortType = std::underlying_type<Port>::type;
    return static_cast<PortType>(port);
}

}   // end anonymous namespace

DMA::DMA() : m_control{ 0x7654321 }, m_IRQEnable{}, m_channelIRQEnable{}, m_channelIRQFlags{}, m_forceIRQ{} {}

Utils::UInt32 DMA::DMARegisterRead(Utils::UInt32 offset) const
{
    const Utils::UInt32 major = (offset & 0x70) >> 4;
    const Utils::UInt32 minor = offset & 0xF;

    Utils::UInt32 value = 0;

    switch (major)
    {
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
            {
                const Channel& chan = GetChannel(IndexToPort(major));
                switch (minor)
                {
                    case 0:
                        return chan.GetBase();
                    case 4:
                        return chan.GetBlockControl();
                    case 8:
                        return chan.GetControl();
                    default:
                        assert(false && "Unhandled DMA read");
                        return value;
                }
            }
        case 7:
        {
            switch (minor)
            {
                case 0:
                    return GetControl();
                case 4:
                    return GetInterrupt();
                default:
                    assert(false && "Unhandled DMA read");    
                    return value;
            }
        }
        default:
            assert(false && "Unhandled DMA read");
            return value;
    }
}

void DMA::DMARegisterWrite(Utils::UInt32 offset, Utils::UInt32 value)
{
    const Utils::UInt32 major = (offset & 0x70) >> 4;
    const Utils::UInt32 minor = offset & 0xF;

    switch (major)
    {
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
            {
                Channel& chan = GetChannel(IndexToPort(major));
                switch (minor)
                {
                    case 0:
                        return chan.SetBase(value);
                    case 4:
                        return chan.SetBlockControl(value);
                    case 8:
                        return chan.SetControl(value);
                    default:
                        assert(false && "Unhandled DMA read");
                }
            }
        case 7:
        {
            switch (minor)
            {
                case 0:
                    return SetControl(value);
                case 4:
                    return SetInterrupt(value);
                default:
                    assert(false && "Unhandled DMA read");    
            }
        }
        default:
            assert(false && "Unhandled DMA read");
    }
}


Utils::UInt32 DMA::GetControl() const
{
     return m_control;
}

void DMA::SetControl(Utils::UInt32 value)
{
    m_control = value;
}

bool DMA::GetIRQ() const
{
    return m_forceIRQ || (m_IRQEnable && (m_channelIRQEnable & m_channelIRQFlags));
}

Utils::UInt32 DMA::GetInterrupt() const
{
    Utils::UInt32 interrupt;

    interrupt |= m_dummy;
    interrupt |= static_cast<Utils::UInt32>(m_forceIRQ) << 15;
    interrupt |= static_cast<Utils::UInt32>(m_channelIRQEnable) << 16;
    interrupt |= static_cast<Utils::UInt32>(m_IRQEnable) << 23;
    interrupt |= static_cast<Utils::UInt32>(m_channelIRQFlags) << 24;
    interrupt |= static_cast<Utils::UInt32>(GetIRQ()) << 31;

    return interrupt;
}

void DMA::SetInterrupt(Utils::UInt32 value)
{
    m_dummy = (value & 0x3F);
    m_forceIRQ = ((value >> 15) & 1) != 0;
    m_channelIRQEnable = ((value >> 16) & 0x7F);
    m_IRQEnable = ((value >> 23) & 1) != 0;

    // Writing 1 to a flag resets it
    const Utils::UInt8 ack = (value >> 24) & 0x3F;
    m_channelIRQFlags &= !ack;
}

Channel& DMA::GetChannel(Port port)
{
    return m_channels[PortToIndex(port)];
}

const Channel& DMA::GetChannel(Port port) const
{
    return m_channels[PortToIndex(port)];
}

void DMA::DoMemoryTransfer(Port port)
{
    const Channel& chan = GetChannel(port);
    
    if (chan.GetSync() == Sync::LINKED_LIST)
    {
        DoLinkedListCopy(port);
    }
    else
    {
        DoMemoryBlockCopy(port);
    }
}

void DMA::DoMemoryBlockCopy(Port port)
{
    Channel& chan = GetChannel(port);

    const Utils::Int32 increment = chan.GetStep() == Step::INCREMENT ? 4 : -4;
    Utils::UInt32& address = chan.GetBase();

    std::optional<Utils::UInt32> remainingSize = chan.GetTransferSize();
    if (remainingSize == std::nullopt)
    {
        assert(false && "Couldn't figure out DMA block transfer size");
        return;
    }

    while (remainingSize > 0)
    {
        Utils::UInt32 curAddr = address & 0x1FFFFC;

        if (chan.GetDirection() == Direction::FROM_RAM)
        {
            assert(false && "Unhandled DMA direction");
            return;
        }
        else
        {
            Utils::UInt32 srcWord;
            if (port == Port::OTC)
            {
                if (remainingSize == 1)
                {
                    srcWord = 0xFFFFFF;
                }
                else
                {
                    srcWord = (address - 4) & 0x1FFFFF;
                }
            }
            else
            {
                assert(false && "Unhandled DMA source port");
                return;
            }
            
            // TODO: Write to RAM
        }

        address += increment;
        --(*remainingSize);
    }

    chan.SetDone();
}

void DMA::DoLinkedListCopy(Port port)
{
    Channel& chan = GetChannel(port);

    const Utils::Int32 increment = chan.GetStep() == Step::INCREMENT ? 4 : -4;
    Utils::UInt32& address = chan.GetBase();

    if (chan.GetDirection() == Direction::TO_RAM)
    {
        assert(false && "Invalid DMA direction for linked list mode");
        return;
    }

    if (port != Port::GPU)
    {
        assert(false && "Attempted linked list DMA on port");
        return;
    }

    // In linked list mode, each entry starts with a
    // *header* word. The high byte contains the number
    // of words in the *packet* (not counting the header word)
    Utils::UInt32 header = 0;   // TODO: Load from RAM
    
    while (header & 0x800000)
    {
        Utils::UInt32 remainingSize = header >> 24;

        while (remainingSize > 0)
        {
            address = (address + 4) & 0x1FFFFC;
            const Utils::UInt32 command = 0; // TODO: Load from RAM

            --remainingSize;
        }

        address = header & 0x1FFFFC;
    }

    chan.SetDone();
}