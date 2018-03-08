#ifndef RENDERER_H
#define RENDERER_H

class GLFWwindow;

namespace PSEmu
{

class Renderer
{
public:
    Renderer(GLFWwindow* window);

    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;

    Renderer(Renderer&&) = default;
    Renderer& operator=(Renderer&&) = default;

private:
    GLFWwindow* m_window;
};

}   // end namespace PSEmu

#endif // RENDERER_H