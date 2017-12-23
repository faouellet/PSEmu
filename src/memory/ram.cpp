#include "ram.h"

#include "memorymap.h"

using namespace PSEmu;

// RAM contains garbage by default
RAM::RAM() : m_data(0xCA, RAM_SIZE) { }

Utils::UInt8 RAM::LoadByte(Utils::UInt32 offset) const
{
    return m_data[offset];
}

// Fetch the little endian halfword at offset
Utils::UInt16 RAM::LoadHalfWord(Utils::UInt32 offset) const
{
    const Utils::UInt32 byte0 = m_data[offset + 0];
    const Utils::UInt32 byte1 = m_data[offset + 1];

    return byte0 | (byte1 << 8);
}

// Fetch the little endian word at offset
Utils::UInt32 RAM::LoadWord(Utils::UInt32 offset) const
{
    const Utils::UInt32 byte0 = m_data[offset + 0];
    const Utils::UInt32 byte1 = m_data[offset + 1];
    const Utils::UInt32 byte2 = m_data[offset + 2];
    const Utils::UInt32 byte3 = m_data[offset + 3];

    return byte0 | (byte1 << 8) | (byte2 << 16) | (byte3 << 24);
}

void RAM::StoreByte(Utils::UInt32 offset, Utils::UInt8 value)
{
    m_data[offset] = value;
}

void RAM::StoreHalfWord(Utils::UInt32 offset, Utils::UInt16 value)
{
    const Utils::UInt8 byte0 = value;
    const Utils::UInt8 byte1 = value << 8;

    m_data[offset + 0] = byte0;
    m_data[offset + 1] = byte1;
}

void RAM::StoreWord(Utils::UInt32 offset, Utils::UInt32 value)
{
    const Utils::UInt8 byte0 = value;
    const Utils::UInt8 byte1 = value << 8;
    const Utils::UInt8 byte2 = value << 16;
    const Utils::UInt8 byte3 = value << 24;

    m_data[offset + 0] = byte0;
    m_data[offset + 1] = byte1;
    m_data[offset + 2] = byte2;
    m_data[offset + 3] = byte3;
}