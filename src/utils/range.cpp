#include "range.h"

using namespace Utils;

Range::Range(uint32_t start, uint32_t length) : m_start{ start }, m_length{ length } { }

std::optional<uint32_t> Range::Contains(uint32_t address) const
{
    if (m_start <= address && address < m_start + m_length)
    {
        return { address - m_start };
    }
    
    return std::nullopt;
}