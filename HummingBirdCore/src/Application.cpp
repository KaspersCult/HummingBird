//
// Created by Kasper de Bruin on 01/11/2023.
//

#include "Application.h"
#include <backends/imgui_impl_glfw_vulkan_window.h>
#include <backends/debug/imgui_impl_glfw_vulkan_debug.h>

#include <UIWindows/MainMenuBar.h>
#include <UIWindows/WindowManager.h>

namespace HummingBirdCore {
  void Application::init() {

    ApplicationSpecification m_Specification;
    //App settings
    m_Specification.Name = "Hummingbird Core";
    m_Specification.WindowSettings.Width = 800;
    m_Specification.WindowSettings.Height = 600;
    m_Specification.WindowSettings.WindowResizeable = true;
    m_Specification.WindowSettings.WindowDecorated = true;
    m_Specification.WindowSettings.CenterWindow = true;
    m_Specification.WindowSettings.CreateDefaultDockSpace = true;

    //Titlebar settings
    m_Specification.TitleBarSettings.CustomTitleBar = true;
    m_Specification.TitleBarSettings.HasLogo = true;
    m_Specification.TitleBarSettings.LogoPath = "Assets/Textures/logo.png";
    m_Specification.TitleBarSettings.DrawTitleCentered = true;

    m_Specification.TitleBarSettings.MainMenuBarCallback = new std::function<void()>([&]() {
      UI::mainMenuBarCallback();
    });

    ImGui_ImplVKGlfw_setImplErrorCallback([&](int error, const char *description) {
      std::cout << "GLFWVULKANIMPL::errorCallback() error: " << error << " description: " << description << std::endl;
    });

    ImGui_ImplVKGlfw_init(m_Specification);

    run();
  }

  bool Application::run() {
    while (!glfwWindowShouldClose(ImGui_ImplVKGlfw_getMainWindowHandle())) {
      render();
    }
  }

  void Application::render() {
    ImGui_ImplVKGlfw_startRender();

    ImGui::ShowDemoWindow();

    HummingBirdCore::UI::WindowManager::getInstance()->render();

    ImguiGlfwVulkanDebugger::render();
    ImGui_ImplVKGlfw_endRender();
  }

  void Application::shutdown() {
    ImGui_ImplVKGlfw_shutdown();
  }
}