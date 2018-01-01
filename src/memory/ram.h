#ifndef RAM_H
#define RAM_H

#include <cstdint>
#include <vector>

namespace PSEmu
{

class RAM
{
public:
    RAM();

    // It should not be possible to copy an instance of this class
    RAM(const RAM&) = delete;
    RAM& operator=(const RAM&) = delete;

    // But it should be possible to move it
    RAM(RAM&&) = default;
    RAM& operator=(RAM&&) = default;

public:
    uint8_t  LoadByte(uint32_t offset) const;
    uint16_t LoadHalfWord(uint32_t offset) const;
    uint32_t LoadWord(uint32_t offset) const;

    void StoreByte(uint32_t offset, uint8_t value);
    void StoreHalfWord(uint32_t offset, uint16_t value);
    void StoreWord(uint32_t offset, uint32_t value);

private:
    std::vector<uint8_t> m_data;
};

}   // end namespace PSEmu

#endif // RAM_H