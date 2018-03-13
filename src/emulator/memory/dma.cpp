#include "dma.h"

#include "../video/gpu.h"
#include "ram.h"

#include <cassert>

using namespace PSEmu;

namespace
{

Port IndexToPort(uint32_t index)
{
    return static_cast<Port>(index);
}

uint32_t PortToIndex(Port port)
{
    using PortType = std::underlying_type<Port>::type;
    return static_cast<PortType>(port);
}

}   // end anonymous namespace

DMA::DMA(GPU& gpu, RAM& ram) 
    : m_control{ 0x7654321 }, m_IRQEnable{}, m_channelIRQEnable{}, 
      m_channelIRQFlags{}, m_forceIRQ{}, m_dummy{}, m_channels{}, 
      m_ram{ ram }, m_gpu{ gpu } { }

uint32_t DMA::RegisterRead(uint32_t offset) const
{
    const uint32_t major = (offset & 0x70) >> 4;
    const uint32_t minor = offset & 0xF;

    uint32_t value = 0;

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
        // Common DMA registers
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

void DMA::RegisterWrite(uint32_t offset, uint32_t value)
{
    const uint32_t major = (offset & 0x70) >> 4;
    const uint32_t minor = offset & 0xF;

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
                        chan.SetBase(value);
                        break;
                    case 4:
                        chan.SetBlockControl(value);
                        break;
                    case 8:
                        chan.SetControl(value);
                        break;
                    default:
                        assert(false && "Unhandled DMA read");
                }
                break;
            }
        // Common DMA registers
        case 7:
        {
            switch (minor)
            {
                case 0:
                    SetControl(value);
                    break;
                case 4:
                    SetInterrupt(value);
                    break;
                default:
                    assert(false && "Unhandled DMA read");    
            }
            break;
        }
        default:
            assert(false && "Unhandled DMA read");
    }
}


uint32_t DMA::GetControl() const
{
     return m_control;
}

void DMA::SetControl(uint32_t value)
{
    m_control = value;
}

bool DMA::GetIRQ() const
{
    return m_forceIRQ || (m_IRQEnable && (m_channelIRQEnable & m_channelIRQFlags));
}

uint32_t DMA::GetInterrupt() const
{
    uint32_t interrupt{};

    interrupt |= m_dummy;
    interrupt |= static_cast<uint32_t>(m_forceIRQ) << 15;
    interrupt |= static_cast<uint32_t>(m_channelIRQEnable) << 16;
    interrupt |= static_cast<uint32_t>(m_IRQEnable) << 23;
    interrupt |= static_cast<uint32_t>(m_channelIRQFlags) << 24;
    interrupt |= static_cast<uint32_t>(GetIRQ()) << 31;

    return interrupt;
}

void DMA::SetInterrupt(uint32_t value)
{
    m_dummy = (value & 0x3F);
    m_forceIRQ = ((value >> 15) & 1) != 0;
    m_channelIRQEnable = ((value >> 16) & 0x7F);
    m_IRQEnable = ((value >> 23) & 1) != 0;

    // Writing 1 to a flag resets it
    const uint8_t ack = (value >> 24) & 0x3F;
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

    const int32_t increment = chan.GetStep() == Step::INCREMENT ? 4 : -4;
    uint32_t& address = chan.GetBase();

    std::optional<uint32_t> transferSize = chan.GetTransferSize();
    if (transferSize == std::nullopt)
    {
        assert(false && "Couldn't figure out DMA block transfer size");
        return;
    }

    uint32_t remainingSize = *transferSize;

    while (remainingSize > 0)
    {
        uint32_t curAddr = address & 0x1FFFFC;

        if (chan.GetDirection() == Direction::FROM_RAM)
        {
            //const uint32_t srcWord = m_ram.LoadWord(curAddr);
        }
        else    // Direction::TO_RAM
        {
            uint32_t srcWord;
            if (port == Port::OTC)
            {
                if (remainingSize == 1)
                {
                    // Last entry contains the end of table marker
                    srcWord = 0xFFFFFF;
                }
                else
                {
                    // Pointer to the previous entry
                    srcWord = (address - 4) & 0x1FFFFF;
                }
            }
            else
            {
                assert(false && "Unhandled DMA source port");
                return;
            }
            
            m_ram.Store<uint32_t>(curAddr, srcWord);
        }

        address += increment;
        --remainingSize;
    }

    chan.SetDone();
}

void DMA::DoLinkedListCopy(Port port)
{
    Channel& chan = GetChannel(port);

    uint32_t& address = chan.GetBase();

    if (chan.GetDirection() == Direction::TO_RAM)
    {
        assert(false && "Invalid DMA direction for linked list mode");
        return;
    }

    if (port != Port::GPU)
    {
        assert(false && "Attempted linked list DMA on incorrect port");
        return;
    }

    for(;;)
    {
        // In linked list mode, each entry starts with a
        // *header* word. The high byte contains the number
        // of words in the *packet* (not counting the header word)
        uint32_t header = m_ram.Load<uint32_t>(address);

        while (header & 0x800000)
        {
            uint32_t remainingSize = header >> 24;

            while (remainingSize > 0)
            {
                address = (address + 4) & 0x1FFFFC;
                const uint32_t command = m_ram.Load<uint32_t>(address);

                m_gpu.SetGP0(command);

                --remainingSize;
            }

            address = header & 0x1FFFFC;
        }
    }

    chan.SetDone();
}