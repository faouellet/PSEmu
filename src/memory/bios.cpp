#include "bios.h"

#include <fstream>
#include <iterator>

using namespace PSEmu;

bool BIOS::Init(const std::string& absoluteFilePath)
{
    std::ifstream biosStream{ absoluteFilePath };

    if(biosStream)
    {
        m_data.assign(std::istreambuf_iterator<Utils::Byte>{biosStream},
                      std::istreambuf_iterator<Utils::Byte>{});

        return true;
    }

    return false;
}

Utils::UInt8 BIOS::LoadByte(Utils::UInt32 offset) const
{
    return m_data[offset];
}

Utils::UInt32 BIOS::LoadWord(Utils::UInt32 offset) const
{
    Utils::UInt32 word;

    // Don't forget the PSX is a little endian machine!
    for(int iByte = 3; iByte > 0; --iByte)
    {
        word = ((word << (iByte * 8)) | m_data[offset + iByte]);
    }

    return word;
}
