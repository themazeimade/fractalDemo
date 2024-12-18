#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "implot.h"
#include <functional>

class imguiManager {

  std::function<void(void)> gui;

public:
  imguiManager() = default;
  ~imguiManager() = default;

  void cleanup() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImPlot::DestroyContext();
    ImGui::DestroyContext();
  }

  void setGUI(std::function<void(void)> gui_) {
    gui = gui_;
  }

  void init(GLFWwindow *myWindow) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImPlot::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |=
        ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io.ConfigFlags |=
        ImGuiConfigFlags_NavEnableGamepad; // Enable Gamepad Controls
    /*io.ConfigFlags |= ImGuiConfigFlags_DockingEnable; // IF using Docking
     * Branch*/

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(
        myWindow, true); // Second param install_callback=true will install
                         // GLFW callbacks and chain to existing ones.
    ImGui_ImplOpenGL3_Init();
  }

  void set() {
    // (Your code calls glfwPollEvents())
    // ...
    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    gui();
  }
  void draw() {
    // Rendering
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
  }
};

