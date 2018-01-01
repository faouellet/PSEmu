#ifndef BIOS_H
#define BIOS_H

#include <cstdint>
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

    uint8_t  LoadByte(uint32_t offset) const;
    uint32_t LoadWord(uint32_t offset) const;

private:
    std::vector<uint8_t> m_data;
};

} // end namespace PSEmu

#endif // BIOS_H