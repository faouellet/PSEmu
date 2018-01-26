#ifndef COMMAND_BUFFER_H
#define COMMAND_BUFFER_H

#include <array>
#include <cstdint>

namespace PSEmu
{

class CommandBuffer
{
public:
    CommandBuffer();

public:
    void Clear();
    void PushWord(uint32_t word);
    uint32_t operator[](uint32_t index) const;

private:
    // Command buffer. The longest possible command is GP0(0x3E)
    // which takes 12 parameters
    std::array<uint32_t, 12> m_buffer;

    // Number of words queued in buffer.
    uint8_t m_len;
};

}   // end namespace PSEmu

#endif // COMMAND_BUFFER_H