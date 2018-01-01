#include "commandbuffer.h"

using namespace PSEmu;

void CommandBuffer::Clear()
{
    m_buffer = {};
    m_len = 0;
}

void CommandBuffer::PushWord(uint32_t word)
{
    // TODO: Add validations!
    m_buffer[m_len] = word;
    ++m_len;
}

uint32_t CommandBuffer::operator[](uint32_t index) const
{
    return m_buffer[index];
}
