#ifndef RAM_H
#define RAM_H

#include <cassert>
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

    template <typename TSize>
    void Store(uint32_t offset, TSize value)
    {
        static_assert(std::is_integral_v<TSize>);
        
        constexpr auto nbBytesToStore = sizeof(TSize);

        // TODO: Replace this assert by having some explicit template instantiations
        //       for byte, halfword and word?
        static_assert((nbBytesToStore == 1) || (nbBytesToStore == 2) || (nbBytesToStore == 4));

        // Don't forget the PSX is a little endian machine!
        for(unsigned iByte = 0; iByte < nbBytesToStore; ++iByte)
        {
            assert((offset + iByte) < m_data.size());
            m_data[offset + iByte] = static_cast<uint8_t>(value >> (iByte * 8));
        }
    }

private:
    std::vector<uint8_t> m_data;
};

}   // end namespace PSEmu

#endif // RAM_H