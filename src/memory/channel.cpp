#include "channel.h"

#include <cassert>

using namespace PSEmu;

Channel::Channel() = default;

uint32_t Channel::GetControl() const
{
    uint32_t control{};

    control |= static_cast<uint32_t>(m_direction) << 0;
    control |= static_cast<uint32_t>(m_step) << 1;
    control |= static_cast<uint32_t>(m_chop) << 8;
    control |= static_cast<uint32_t>(m_sync) << 9;
    control |= static_cast<uint32_t>(m_chopDMASize) << 16;
    control |= static_cast<uint32_t>(m_chopCPUSize) << 20;
    control |= static_cast<uint32_t>(m_enable) << 24;
    control |= static_cast<uint32_t>(m_trigger) << 28;
    control |= static_cast<uint32_t>(m_unknown) << 29;

    return control;
}

void Channel::SetControl(uint32_t value)
{
    m_direction = ((value & 1) != 0) ? Direction::FROM_RAM : Direction::TO_RAM;
    m_step = (((value >> 1) & 1) != 0) ? Step::DECREMENT : Step::INCREMENT;
    m_chop = (((value >> 8) & 1) != 0);

    switch ((value >> 9) & 3)
    {
        case 0:
            m_sync = Sync::MANUAL;
            break;
        case 1:
            m_sync = Sync::REQUEST;
            break;
        case 2:
            m_sync = Sync::LINKED_LIST;
            break;
        default:
            assert(false && "Unknown DMA sync mode");
    }

    m_chopDMASize = ((value >> 16) & 7);
    m_chopCPUSize = ((value >> 20) & 7);
    m_enable = (((value >> 24) & 1) != 0);
    m_trigger = (((value >> 28) & 1) != 0);
    m_unknown = (((value >> 29) & 3) != 0);
}

uint32_t Channel::GetBase() const
{
    return m_base;
}

uint32_t& Channel::GetBase()
{
    return m_base;
}

void Channel::SetBase(uint32_t value)
{
    m_base = value & 0xFFFFFF;
}

uint32_t Channel::GetBlockControl() const
{
    const uint32_t bs = m_blockSize;
    const uint32_t bc = m_blockCount;

    return (bc << 16) | bs;
}

void Channel::SetBlockControl(uint32_t value)
{
    m_blockSize = value;
    m_blockCount = (value >> 16);
}

bool Channel::IsActive() const
{
    // In manual sync mode, the CPU must set the *trigger* bit
    // to start the transfer
    const bool trigger = m_sync == Sync::MANUAL ? m_trigger : true;

    return m_enable && trigger;
}

void Channel::SetDone()
{
    m_enable = false;
    m_trigger = false;
    // TODO: Need to set the correct value for the other fields
    // (in particular interrupts)
}

Direction Channel::GetDirection() const
{
    return m_direction;
}

Step Channel::GetStep() const
{
    return m_step;
}

Sync Channel::GetSync() const
{
    return m_sync;
}

std::optional<uint32_t> Channel::GetTransferSize() const
{
    const uint32_t bs = m_blockSize;
    const uint32_t bc = m_blockCount;

    switch (m_sync)
    {
        case Sync::MANUAL:
            // For manual mode, only the block size is used
            return { bs };
        case Sync::REQUEST:
            // In DMA request mode, we must transfer 'bc' blocks
            return { bc * bs };
        case Sync::LINKED_LIST:
            // In linked list mode, the size is not known ahead of time:
            // we stop when we encounter the 'end of list' marker (0xFFFFFF)
            return std::nullopt;
        default:
            assert(false);
            return std::nullopt;
    }
}
