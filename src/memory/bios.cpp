#include "bios.h"

#include <cassert>
#include <fstream>
#include <iterator>

#include <iostream>

using namespace PSEmu;

BIOS::BIOS() = default;

bool BIOS::Init(const std::string& absoluteFilePath)
{
    std::ifstream biosStream{ absoluteFilePath };

    if(biosStream)
    {
        m_data.assign(std::istreambuf_iterator<char>{biosStream},
                      std::istreambuf_iterator<char>{});

        return true;
    }

    return false;
}

uint8_t BIOS::LoadByte(uint32_t offset) const
{
    return m_data[offset];
}

uint32_t BIOS::LoadWord(uint32_t offset) const
{
    std::cout << offset << std::endl;
    std::cout << m_data.size() << std::endl;
    assert(offset < m_data.size());

    uint32_t word = 0;

    // Don't forget the PSX is a little endian machine!
    for(int iByte = 3; iByte >= 0; --iByte)
    {
        assert((offset + iByte) < m_data.size());
        word |= static_cast<uint32_t>(m_data[offset + iByte]) << (iByte * 8);
    }

    return word;
}
