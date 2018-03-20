#ifndef COLOR_H
#define COLOR_H

#include <glad/glad.h>
#include <cstdint>

namespace PSEmu
{

class Color
{
public:
    Color(GLbyte r, GLbyte g, GLbyte b);
    static Color CreateFromGP0(uint32_t value);

private:
    GLbyte m_r;
    GLbyte m_g;
    GLbyte m_b;
};

}

#endif // COLOR_H