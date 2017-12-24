#ifndef COMMAND_BUFFER_H
#define COMMAND_BUFFER_H

#include "../utils/types.h"

#include <array>

namespace PSEmu
{

class CommandBuffer
{
public:
    void Clear();
    void PushWord(Utils::UInt32 word);
    Utils::UInt32 operator[](Utils::UInt32 index) const;

private:
    // Command buffer. The longest possible command is GP0(0x3E)
    // which takes 12 parameters
    std::array<Utils::UInt32, 12> m_buffer;

    // Number of words queued in buffer.
    Utils::UInt8 m_len;
};

}   // end namespace PSEmu

#endif // COMMAND_BUFFER_H