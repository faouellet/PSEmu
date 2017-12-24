#include "commandbuffer.h"

using namespace PSEmu;

void CommandBuffer::Clear()
{
    m_buffer = {};
    m_len = 0;
}

void CommandBuffer::PushWord(Utils::UInt32 word)
{
    // TODO: Add validations!
    m_buffer[m_len] = word;
    ++m_len;
}

Utils::UInt32 CommandBuffer::operator[](Utils::UInt32 index) const
{
    return m_buffer[index];
}
