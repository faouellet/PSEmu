#include "position.h"

using namespace PSEmu;

Position::Position(GLshort x, GLshort y) : m_x{ x }, m_y{ y } { }

Position Position::CreateFromGP0(uint32_t value)
{
    return Position{ static_cast<int16_t>(value), static_cast<int16_t>(value >> 16) };
}
