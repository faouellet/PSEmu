#include "src/emulator/cpu/r3000a.h"
#include "src/emulator/memory/bios.h"
#include "src/emulator/video/gpu.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

using namespace PSEmu;

int main()
{
    // Initialize GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create GLFW window
    GLFWwindow* window = glfwCreateWindow(800, 600, "PSEmu", nullptr, nullptr);
    if (window == nullptr)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, 
                                   [](GLFWwindow*, int width, int height)
                                   { 
                                       glViewport(0, 0, width, height); 
                                   });

    // Load OpenGL function pointers using glad
    // This needs to be done after we created the GLFW window
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return false;
    }   

    Renderer renderer{window};
    GPU gpu{std::move(renderer)};

    BIOS bios;
    bios.Init("SCPH1001.BIN");
    Interconnect interconnect{std::move(bios), std::move(gpu)};
    Debugger debugger;
    R3000A cpu{std::move(interconnect), std::move(debugger)};

    //for(;;)
    //{
    //    cpu.Step();
    //}

    while(!glfwWindowShouldClose(window))
    {
        // Input processing
        if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(window, true);
        }

        // Rendering
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glfwSwapBuffers(window);
        glfwPollEvents();    
    }

    glfwTerminate();
    return 0;
}