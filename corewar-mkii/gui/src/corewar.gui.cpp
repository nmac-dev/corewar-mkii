/// Corewar GUI using 'Dear ImGui'

#include "imgui_required.hpp"
#include "control_panel.gui.hpp"

int main() {
/** GLFW:GLAD:SETUP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    /* Init Window */
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(1440.f, 556.f, "Corewar MKII", nullptr, nullptr);

    /* Errors */
    if (window == nullptr) {
        std::cout << "Could not create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        std::cout << "Could not initialize GLAD" << std::endl;
        return -1;
    }

/** IMGUI:SETUP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    /* Set Config */ // must be set before backend is initialised
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    io.IniFilename = ".gui.ini";
    io.ConfigFlags |= Corewar::GUI::GLOBAL_CONFIG_FLAGS;

    /* Tweaks for Viewport */
    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }
    /* Init Backend */
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(GLSL_VERSION);

/** COREWAR:GUI:SETUP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    /* Game Instance */
    using namespace Corewar;
    static Game COREWAR_GAME;

    /* Main Viewport */
    static ImGuiWindowFlags main_window_flags ( 
        GUI::GLOBAL_WINDOW_FLAGS
        // extra
        | ImGuiWindowFlags_NoTitleBar
        | ImGuiWindowFlags_NoBringToFrontOnFocus
        | ImGuiWindowFlags_NoNavFocus
        | ImGuiWindowFlags_NoBackground
    );

/** LOOP:START: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    while (!glfwWindowShouldClose(window))
    { 
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();

        ImGui::NewFrame();
    /** FRAME:START: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

     /* Fullscreen */
        static ImGuiViewport const *viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->Pos);
        ImGui::SetNextWindowSize(viewport->Size);
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::SetNextWindowBgAlpha(0.0f);

        static int main_push = 2;
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

     /* Main Display */
        ImGui::Begin("Corewar", NULL, main_window_flags);       ImGui::PopStyleVar(main_push);
        {
         /* Set Docking */
            if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
            {
               ImGuiID dockspace_id = ImGui::GetID("Main Dockspace");
               ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), GUI::GLOBAL_DOCKSPACE_FLAGS);
               ImGuiDockNode* Node  = ImGui::DockBuilderGetNode( dockspace_id );
               Node->LocalFlags    |= GUI::GLOBAL_DOCKSPACE_FLAGS_PRIVATE;
               
               io.IniFilename = NULL; // stop '.ini' overwrite once loaded
            }

         /** COREWAR:GUI:DRAW:START: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
            if (GUI::ControlPanel::init_flag() == false)
            {
                GUI::ControlPanel::init(&COREWAR_GAME);
            }
            /* Draw GUI */
            GUI::ControlPanel::draw();
         /** COREWAR:GUI:DRAW:END: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
        }
        ImGui::End();

    /** FRAME:END: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
        ImGui::Render();

        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0, 0, 0, 0);       // set transparent so...
        glClear(GL_COLOR_BUFFER_BIT);   // when window is moved it will not trail

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        /* Render Multi-Viewport */
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            GLFWwindow *backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }

        /* Swap Framebuffer */
        glfwSwapBuffers(window);
    }
/** LOOP:END: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    /* Free */
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return EXIT_SUCCESS;
}
