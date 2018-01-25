#include "src/cpu/r3000a.h"
#include "src/memory/bios.h"

// TODO: These includes below are atrocious
#include "3rd party/imgui/imgui.h"
#include "imgui_impl_glfw_gl3.h"
#include "3rd party/glad/include/glad/glad.h"

// TODO: Need custom features
#include "memorydlg.h"

#include <GLFW/glfw3.h>

#include <iostream>

using namespace PSEmu;

static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error %d: %s\n", error, description);
}

int main()
{
        // Setup window
    glfwSetErrorCallback(error_callback);
    if (!glfwInit())
        return 1;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#if __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    GLFWwindow* window = glfwCreateWindow(1280, 720, "ImGui OpenGL3 example", NULL, NULL);
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Setup ImGui binding
    ImGui_ImplGlfwGL3_Init(window, true);

    // Setup style
    ImGui::StyleColorsClassic();

    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    MemoryEditor mem_edit_1;
    BIOS bios;
    bios.Init("SCPH1001.BIN");

    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        ImGui_ImplGlfwGL3_NewFrame();

        mem_edit_1.DrawWindow("Memory Editor", bios.GetData().data(), bios.GetData().size(), 0x0000);

        // Rendering
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui::Render();
        glfwSwapBuffers(window);
    }

    // Cleanup
    ImGui_ImplGlfwGL3_Shutdown();
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