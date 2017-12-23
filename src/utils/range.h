#ifndef RANGE_H
#define RANGE_H

#include "types.h"

#include <optional>

namespace Utils
{

class Range
{
public:
    Range(UInt32 start, UInt32 length);

public:
    std::optional<UInt32> Contains(UInt32 address) const;

private:
    UInt32 m_start;
    UInt32 m_length;
};

}   // end namespace Utils

#endif // RANGE_H