#ifndef POSITION_H
#define POSITION_H

#include <glad/glad.h>
#include <cstdint>

namespace PSEmu
{

class Position
{
public:
    Position(GLshort x, GLshort y);

    static Position CreateFromGP0(uint32_t value);

private:
    GLshort m_x;
    GLshort m_y;
};

}

#endif // POSITION_H