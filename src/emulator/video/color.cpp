#include "color.h"

using namespace PSEmu;

Color::Color(GLbyte r, GLbyte g, GLbyte b) : m_r{ r }, m_g{ g }, m_b{ b } { }

Color Color::CreateFromGP0(uint32_t value)
{
    return Color{ static_cast<GLbyte>(value), 
                  static_cast<GLbyte>(value >> 8),
                  static_cast<GLbyte>(value >> 16) 
                };
}

