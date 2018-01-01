#ifndef RANGE_H
#define RANGE_H

#include <cstdint>
#include <optional>

namespace Utils
{

class Range
{
public:
    Range(uint32_t start, uint32_t length);

public:
    std::optional<uint32_t> Contains(uint32_t address) const;

private:
    uint32_t m_start;
    uint32_t m_length;
};

}   // end namespace Utils

#endif // RANGE_H