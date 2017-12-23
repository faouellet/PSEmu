#include "range.h"

using namespace Utils;

Range::Range(UInt32 start, UInt32 length) : m_start{ start }, m_length{ length } { }

std::optional<UInt32> Range::Contains(UInt32 address) const
{
    if (m_start <= address && address < m_start + m_length)
    {
        return { address - m_start };
    }
    
    return std::nullopt;
}