//
// Created by Kasper de Bruin on 01/11/2023.
//
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "Application.h"
#include "Log.h"

#ifdef WITHHUMMINGBIRDKASPERSPECIFIC
#include <HelloLibrary.h>
#include <Hellebrekers/VisualLinkLauncher.h>
#endif

namespace HummingBirdCore {

  Application::Application() {
    if (s_application != nullptr) {
      CORE_WARN("Application already exists!");
      return;
    }

    Log::Init();

    s_application = this;
    InitSDL();
    InitImGui();

    Themes::ThemeManager::SetTheme(Themes::THEMES::PHOCOSGREEN);

    m_backgroundTexture.load();
    Run();
  }

  Application::~Application() {

    HummingBirdCore::Sql::SqlManager::Shutdown();

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_GL_DeleteContext(m_gl_context);
    SDL_DestroyWindow(s_window);
    SDL_Quit();
  }

  void Application::InitSDL() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
      CORE_ERROR("Failed to initialize SDL: {0}", SDL_GetError());
      exit(-1);
    }

    SDL_version compiled;
    SDL_VERSION(&compiled);
    SDL_version linked;
    SDL_GetVersion(&linked);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

    SDL_GL_SetAttribute(
            SDL_GL_CONTEXT_PROFILE_MASK,
            SDL_GL_CONTEXT_PROFILE_CORE);

    SDL_GL_SetAttribute(// required on Mac OS
            SDL_GL_CONTEXT_FLAGS,
            SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    //    SDL_WINDOW_ALLOW_HIGHDPI

    auto mainWindowFlags = (SDL_WindowFlags) (SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    s_window = SDL_CreateWindow(
            "Humming Bird",
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            m_windowWidth,
            m_windowHeight,
            mainWindowFlags);
    // limit to which minimum size user can resize the window
    SDL_SetWindowMinimumSize(s_window, 800, 600);

    m_gl_context = SDL_GL_CreateContext(s_window);
    if (m_gl_context == nullptr) {
      CORE_ERROR("Failed to create a GL context: {0}", SDL_GetError());
      exit(-1);
    }
    SDL_GL_MakeCurrent(s_window, m_gl_context);

    // enable VSync
    SDL_GL_SetSwapInterval(1);

    if (!gladLoadGLLoader((GLADloadproc) SDL_GL_GetProcAddress)) {
      CORE_ERROR("Couldn't initialize glad");
      exit(-1);
    } else {
      CORE_INFO("glad initialized");
    }

    CORE_INFO("OpenGL from glad: {0}.{1}", GLVersion.major, GLVersion.minor);

    int sdlOpenGLmajor = 0, sdlOpenGLminor = 0;
    SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &sdlOpenGLmajor);
    SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &sdlOpenGLminor);
    CORE_INFO("OpenGL from SDL: {0}.{1}", sdlOpenGLmajor, sdlOpenGLminor);
    glViewport(0, 0, m_windowWidth, m_windowHeight);
  }

  void Application::InitImGui() {

    // setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void) io;

    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

    // setup platform/renderer bindings
    ImGui_ImplSDL2_InitForOpenGL(s_window, m_gl_context);
    ImGui_ImplOpenGL3_Init("#version 330");

    ImFontConfig config;
    config.OversampleH = 3;
    config.OversampleV = 3;
    config.PixelSnapH = true;

    io.FontDefault = io.Fonts->AddFontFromFileTTF(
            "Assets/Fonts/JetBrainsMono/JetBrainsMonoNerdFontPropo-Regular.ttf", 16.0f, &config);
  }

  void Application::Run() {
    while (!m_exit) {
      Render();
    }
  }

  void Application::SetupDockspace() {
    ImGuiID dockspaceId = ImGui::GetID("MyDockSpace");
    ImGui::DockSpace(dockspaceId, ImVec2(0, 0),
                     ImGuiDockNodeFlags_None | ImGuiDockNodeFlags_PassthruCentralNode);


    //File menu
    if (ImGui::BeginMenuBar()) {
      if (ImGui::BeginMenu("File")) {
        if (ImGui::MenuItem("New Project")) {
        }
        if (ImGui::MenuItem("Open Project")) {
        }
        if (ImGui::MenuItem("Save Project")) {
        }

        ImGui::Separator();
        if (ImGui::MenuItem("Log Out")) {
          Security::LoginManager::logout();
        }
        if (ImGui::MenuItem("Exit")) {
          m_exit = true;
        }
        ImGui::EndMenu();
      }
      //Tools menu
      if (ImGui::BeginMenu("Tools")) {
        if (ImGui::BeginMenu("General Tools")) {
          if (ImGui::MenuItem("Text Editor")) {
            AddWindow("Text Editor", std::make_shared<HummingBirdCore::GeneralTools::TextEditorWindow>());
          }
          ImGui::EndMenu();
        }

        if (ImGui::MenuItem("Terminal")) {
          //          AddWindow(Terminal::TerminalWindow::getT(), std::make_shared<Terminal::TerminalWindow>());
        }

        if (ImGui::BeginMenu("Networking")) {
          if (ImGui::MenuItem("Edit Hosts")) {
            //            AddWindow("Edit Hosts", std::make_shared<HummingBirdCore::System::EditHosts>());
            AddWindow("Edit Hosts", std::make_shared<HummingBirdCore::System::EditHostsWindow>());
          }
          ImGui::EndMenu();
        }
        ImGui::EndMenu();
      }
      //View menu
      if (ImGui::BeginMenu("View")) {
        if (ImGui::BeginMenu("Styles")) {
          if (ImGui::MenuItem("ThemeManager")) {
//            AddWindow("Theme Manager", std::make_shared<Themes::ThemeManager>());
          }
          if (ImGui::BeginMenu("Themes")) {
            if (ImGui::MenuItem("Maya")) {
              Themes::ThemeManager::SetTheme(Themes::THEMES::MAYA);
            }
            if (ImGui::MenuItem("Phocus Green")) {
              Themes::ThemeManager::SetTheme(Themes::THEMES::PHOCOSGREEN);
            }
            if (ImGui::MenuItem("Dark")) {
              Themes::ThemeManager::SetTheme(Themes::THEMES::DARK);
            }
            if (ImGui::MenuItem("Light")) {
              Themes::ThemeManager::SetTheme(Themes::THEMES::LIGHT);
            }
            if (ImGui::MenuItem("Classic")) {
              Themes::ThemeManager::SetTheme(Themes::THEMES::CLASSIC);
            }
            if (ImGui::MenuItem("Monochrome")) {
              Themes::ThemeManager::SetTheme(Themes::THEMES::MONOCHROME);
            }
            if (ImGui::MenuItem("The_0n3")) {
              Themes::ThemeManager::SetTheme(Themes::THEMES::THE_0N3);
            }
            if (ImGui::MenuItem("ModernDarkTheme")) {
              Themes::ThemeManager::SetTheme(Themes::THEMES::MODERNDARK);
            }
            if (ImGui::MenuItem("EmbraceTheDarkness")) {
              Themes::ThemeManager::SetTheme(Themes::THEMES::EMBRACETHEDARKNESS);
            }
            if (ImGui::MenuItem("DoughBkins_Black")) {
              Themes::ThemeManager::SetTheme(Themes::THEMES::DOUGHBKINS_BLACK);
            }
            if (ImGui::MenuItem("DoughBkins_White")) {
              Themes::ThemeManager::SetTheme(Themes::THEMES::DOUGHBKINS_WHITE);
            }
            ImGui::EndMenu();
          }
          ImGui::EndMenu();
        }
        ImGui::EndMenu();
      }
#ifdef WITHHUMMINGBIRDKASPERSPECIFIC
      if (ImGui::BeginMenu("Kasper Tools")) {
        if (ImGui::MenuItem("Hello World")) {
//          AddWindow("Hello World", std::make_shared<HummingBirdKasper::HelloLibrary>());
        }
        if(ImGui::BeginMenu("Visual Link")){
            if(ImGui::MenuItem("Visual Link Launcher")){
                AddWindow("Visual Link",  std::make_shared<HummingBirdKasper::VisualLink::VisualLinkLauncher>());
            }

            ImGui::EndMenu();
        }

        ImGui::EndMenu();
      }
#endif
      if(ImGui::BeginMenu("Third Party")){
          if(ImGui::BeginMenu("ImGui")){
                if(ImGui::MenuItem("Demo Window")){
                    m_showDemoWindow = true;
                }
                if(ImGui::MenuItem("Metrics Window")){
                    m_showMetricsWindow = true;
                }
                if(ImGui::MenuItem("Style Editor")){
                    m_showStyleEditor = true;
                }
                if(ImGui::MenuItem("About Window")){
                    m_showAboutWindow = true;
                }
                if(ImGui::MenuItem("Example App Documents")){
                    m_ShowExampleAppDocuments = true;
                }
                if(ImGui::MenuItem("Example App Console")){
                    m_ShowExampleAppConsole = true;
                }
                if(ImGui::MenuItem("Example App Log")){
                    m_ShowExampleAppLog = true;
                }
                if(ImGui::MenuItem("Example App Layout")){
                    m_ShowExampleAppLayout = true;
                }
//                if(ImGui::MenuItem("Example App Long Text")){
//                    m_ShowExampleAppLongText = true;
//                }
//                if(ImGui::MenuItem("Example App Auto Resize")){
//                    m_ShowExampleAppAutoResize = true;
//                }

              ImGui::EndMenu();
          }
          ImGui::EndMenu();
      }
      // You can add more menus here...
      ImGui::EndMenuBar();
    }
  }

  void Application::RenderUI() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame(s_window);
    ImGui::NewFrame();

    {
      HummingBirdCore::GeneralTools::FileDialog::Render();
    }

    {
      ImGuiWindowFlags mainWindowFlags =
              ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
              ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus |
              ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoBackground;

      ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
      ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
      ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

      ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);// Set the window size to the entire display size
      ImGui::SetNextWindowPos(ImVec2(0, 0));               // Set the window position to the top-left corner
      ImGui::Begin("FullscreenWindow", nullptr, mainWindowFlags);

      ImGui::PopStyleVar(3);
    }

    //Render background texture
    {
      GLuint textureID = m_backgroundTexture.getTextureID();// Replace with your actual texture ID
      ImVec2 texSize = ImVec2(
              m_windowWidth,
              m_windowHeight);
      ImGui::GetWindowDrawList()->AddImage(
              (void *) (intptr_t) textureID,
              ImVec2(ImGui::GetWindowPos()),
              ImVec2(ImGui::GetWindowPos().x + texSize.x, ImGui::GetWindowPos().y + texSize.y),
              ImVec2(0, 0), ImVec2(1, 1));
    }


    SetupDockspace();

    {
      if (Security::LoginManager::isLoggedIn()) {

//        bool m_showDemoWindow = false;
//        bool m_showMetricsWindow = false;
//        bool m_showStyleEditor = false;
//        bool m_showAboutWindow = false;
//        bool m_ShowExampleAppDocuments = false;
//        bool m_ShowExampleAppConsole = false;
//        bool m_ShowExampleAppLog = false;
//        bool m_ShowExampleAppLayout = false;
        if(m_showDemoWindow){
            ImGui::ShowDemoWindow(&m_showDemoWindow);
        }

        if(m_showMetricsWindow){
            ImGui::ShowMetricsWindow(&m_showMetricsWindow);
        }

//        if(m_showStyleEditor){
//            ImGui::ShowStyleEditor(&m_showStyleEditor);
//        }
//
        if(m_showAboutWindow){
            ImGui::ShowAboutWindow(&m_showAboutWindow);
        }

        if(m_ShowExampleAppDocuments){
            ImGui::ShowDemoWindow(&m_ShowExampleAppDocuments);
        }

        if(m_ShowExampleAppConsole){
            ImGui::ShowDemoWindow(&m_ShowExampleAppConsole);
        }

        if(m_ShowExampleAppLog){
            ImGui::ShowDemoWindow(&m_ShowExampleAppLog);
        }

        if(m_ShowExampleAppLayout){
            ImGui::ShowDemoWindow(&m_ShowExampleAppLayout);
        }

        for (auto &[name, window]: m_uiWindows) {
          if (window->isOpen()) {
            ImGui::Begin(name.c_str(), &window->m_isOpen, window->getFlags());
            window->render();
            ImGui::End();
          }
        }
      } else {
        m_loginWindow.render();
      }
    }


    ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
  }

  void Application::Render() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    SDL_Event event;

    while (SDL_PollEvent(&event)) {
      // without it you won't have keyboard input and other things
      ImGui_ImplSDL2_ProcessEvent(&event);
      // you might also want to check io.WantCaptureMouse and io.WantCaptureKeyboard
      // before processing events

      switch (event.type) {
        case SDL_QUIT:
          m_exit = true;
          break;

        case SDL_WINDOWEVENT:
          switch (event.window.event) {
            case SDL_WINDOWEVENT_RESIZED:
              m_windowWidth = event.window.data1;
              m_windowHeight = event.window.data2;
              glViewport(0, 0, m_windowWidth, m_windowHeight);
              break;
          }
          break;

        case SDL_KEYDOWN:
          switch (event.key.keysym.sym) {
            case SDLK_ESCAPE:
              m_exit = true;
              break;
          }
          break;
      }
    }

    RenderUI();


    SDL_GL_SwapWindow(s_window);
  }
}// namespace HummingBirdCore