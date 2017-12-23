#ifndef BIOS_H
#define BIOS_H

#include "../utils/types.h"

#include <string>
#include <vector>

namespace PSEmu
{

class BIOS
{
public:
    // It should not be possible to copy an instance of this class
    BIOS(const BIOS&) = delete;
    BIOS& operator=(const BIOS&) = delete;

    // But it should be possible to move it
    BIOS(BIOS&&) = default;
    BIOS& operator=(BIOS&&) = default;

public:
    bool Init(const std::string& absoluteFilePath);

    Utils::UInt8  LoadByte(Utils::UInt32 offset) const;
    Utils::UInt32 LoadWord(Utils::UInt32 offset) const;

private:
    std::vector<Utils::Byte> m_data;
};

} // end namespace PSEmu

#endif // BIOS_H