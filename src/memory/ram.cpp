#include "ram.h"

#include "memorymap.h"

using namespace PSEmu;

// RAM contains garbage by default
RAM::RAM() : m_data(0xCA, RAM_SIZE) { }

uint8_t RAM::LoadByte(uint32_t offset) const
{
    return m_data[offset];
}

// Fetch the little endian halfword at offset
uint16_t RAM::LoadHalfWord(uint32_t offset) const
{
    const uint32_t byte0 = m_data[offset + 0];
    const uint32_t byte1 = m_data[offset + 1];

    return byte0 | (byte1 << 8);
}

// Fetch the little endian word at offset
uint32_t RAM::LoadWord(uint32_t offset) const
{
    const uint32_t byte0 = m_data[offset + 0];
    const uint32_t byte1 = m_data[offset + 1];
    const uint32_t byte2 = m_data[offset + 2];
    const uint32_t byte3 = m_data[offset + 3];

    return byte0 | (byte1 << 8) | (byte2 << 16) | (byte3 << 24);
}

void RAM::StoreByte(uint32_t offset, uint8_t value)
{
    m_data[offset] = value;
}

void RAM::StoreHalfWord(uint32_t offset, uint16_t value)
{
    const uint8_t byte0 = value;
    const uint8_t byte1 = value << 8;

    m_data[offset + 0] = byte0;
    m_data[offset + 1] = byte1;
}

void RAM::StoreWord(uint32_t offset, uint32_t value)
{
    const uint8_t byte0 = value;
    const uint8_t byte1 = value << 8;
    const uint8_t byte2 = value << 16;
    const uint8_t byte3 = value << 24;

    m_data[offset + 0] = byte0;
    m_data[offset + 1] = byte1;
    m_data[offset + 2] = byte2;
    m_data[offset + 3] = byte3;
}