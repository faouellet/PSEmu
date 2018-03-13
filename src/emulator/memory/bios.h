#ifndef BIOS_H
#define BIOS_H

#include <cassert>
#include <cstdint>
#include <string>
#include <type_traits>
#include <vector>

namespace PSEmu
{

class BIOS
{
public:
    BIOS();

    // It should not be possible to copy an instance of this class
    BIOS(const BIOS&) = delete;
    BIOS& operator=(const BIOS&) = delete;

    // But it should be possible to move it
    BIOS(BIOS&&) = default;
    BIOS& operator=(BIOS&&) = default;

public:
    bool Init(const std::string& absoluteFilePath);

    std::vector<uint8_t>& GetData();

public:
    template <typename TSize>
    TSize Load(uint32_t offset) const
    {
        static_assert(std::is_integral_v<TSize>);

        constexpr auto nbByteToLoad = sizeof(TSize);

        // TODO: Replace this assert by having some explicit template instantiations
        //       for byte, halfword and word?
        static_assert((nbByteToLoad == 1) || (nbByteToLoad == 2) || (nbByteToLoad == 4));

        TSize value = 0;

        // Don't forget the PSX is a little endian machine!
        for(int iByte = nbByteToLoad - 1; iByte >= 0; --iByte)
        {
            assert((offset + iByte) < m_data.size());
            value |= static_cast<uint32_t>(m_data[offset + iByte]) << (iByte * 8);
        }

        return value;
    }

private:
    std::vector<uint8_t> m_data;
};

} // end namespace PSEmu

#endif // BIOS_H