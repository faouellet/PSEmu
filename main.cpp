#include "src/cpu/r3000a.h"
#include "src/memory/bios.h"

#include "3rd party/imgui/imgui.h"
#include "3rd party/glad/include/glad/glad.h"   // This is ugly

#include <GLFW/glfw3.h>

#include <iostream>

using namespace PSEmu;

namespace
{

void FrameBufferSizeCB(GLFWwindow*, int width, int height)
{
    glViewport(0, 0, width, height);
}  

void ProcessInput(GLFWwindow* window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }
}

}   // end anonymous namespace

int main()
{
    // GLFW: initialize and configure
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

    // GLFW: window creation
    GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", nullptr, nullptr);
    if (window == nullptr)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // GLAD: load all OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }   

    glViewport(0, 0, 800, 600);

    glfwSetFramebufferSizeCallback(window, FrameBufferSizeCB);  
    
    // Render loop
    while(!glfwWindowShouldClose(window))
    {
        // Deal with input
        ProcessInput(window);

        // Rendering commands
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // GLFW: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        glfwPollEvents();    
        glfwSwapBuffers(window);
    }

    // GLFW: terminate, clearing all previously allocated GLFW resources.
    glfwTerminate();
    return 0;

    //BIOS bios;
    //bios.Init("SCPH1001.BIN");

    //Interconnect interconnect{std::move(bios)};
    //R3000A cpu{std::move(interconnect)};

    //for(;;)
    //{
    //    cpu.Step();
    //}

    //return 0;
}