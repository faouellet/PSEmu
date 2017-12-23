#ifndef RAM_H
#define RAM_H

#include "../utils/types.h"

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
    Utils::UInt8  LoadByte(Utils::UInt32 offset) const;
    Utils::UInt16 LoadHalfWord(Utils::UInt32 offset) const;
    Utils::UInt32 LoadWord(Utils::UInt32 offset) const;

    void StoreByte(Utils::UInt32 offset, Utils::UInt8 value);
    void StoreHalfWord(Utils::UInt32 offset, Utils::UInt16 value);
    void StoreWord(Utils::UInt32 offset, Utils::UInt32 value);

private:
    std::vector<Utils::Byte> m_data;
};

}   // end namespace PSEmu

#endif // RAM_H