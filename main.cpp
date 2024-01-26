#include "queueing_system.h"
#include "statistics.h"
#include "queueing_system_gui.h"
#include "queueing_system_research.h"

#include <imgui.h>
#include <implot.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <GLFW/glfw3.h>

//#include <vld.h>
#include <iostream>
#include <memory>

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

int main()
{
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return 1;

    const char* glsl_version{ "#version 460" };
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window{ glfwCreateWindow(1280, 720,
        u8"Система массового обслуживания", nullptr, nullptr) };
    if (window == nullptr)
        return 1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImPlot::CreateContext();
    ImGuiIO& io{ ImGui::GetIO() }; (void)io;
    ImGui::StyleColorsClassic();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    auto font{ io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\Arial.ttf",
        20.0f, nullptr, io.Fonts->GetGlyphRangesCyrillic()) };
    IM_ASSERT(font != nullptr);
    auto clear_color{ ImVec4(0.45f, 0.55f, 0.60f, 1.00f) };

    namespace QS = QueueingSystem;
    namespace QSGui = QueueingSystemGui;

    auto systemConf{ std::make_unique<QS::SystemConfiguration>() };
    auto system{ std::make_unique<QS::QueueingSystem>(*systemConf)};

    auto systemStatus{ std::make_unique<QS::SystemStatus>(system->getSystemStatus()) };
    QS::USystemFinalStats systemFinalStats{};

    bool showResultsWindow{};

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        QSGui::configuration(*system, *systemConf, *systemStatus);

        QSGui::controls(*system, showResultsWindow, systemFinalStats);

        QSGui::stepStatistics(*systemStatus);

        if (showResultsWindow)
            QSGui::finalStatistics(*systemFinalStats, showResultsWindow);


        ImGui::Render();
        int display_w;
        int display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w,
            clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImPlot::DestroyContext();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
