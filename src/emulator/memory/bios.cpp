#include "bios.h"

#include <cassert>
#include <fstream>
#include <iterator>

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

std::vector<uint8_t>& BIOS::GetData() 
{
    return m_data; 
}
