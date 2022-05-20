/// Corewar GUI using 'Dear ImGui'

#include "corewar.gui.hpp"

int main() {

/** GLFW:GLAD:SETUP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    /* Init Window */
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(1366, 768, "Corewar", nullptr, nullptr);

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

    /* Set Config */ // must be set before backend is  initialised
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    io.IniFilename = "imgui.ini";
    io.ConfigFlags |= Corewar::GUI::global_config_flags;

    //io.ConfigViewportsNoAutoMerge = true;
    //io.ConfigViewportsNoTaskBarIcon = true;

    /* Tweaks for viewport */
    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }
    /* Init Backend */
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(GLSL_VERSION);

/** COREWAR:GAME:SETUP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    using namespace Corewar;
    static Game corewar_;

    char constexpr rock[]        = "rock.asm",      // test warriors
                   paper[]       = "paper.asm",
                   scissors[]    = "scissors.asm",
                   imp[]         = "imp.asm";

    WarriorFiles filenames = 
    { 
        rock,
        rock,
        rock,
        paper,
        paper,
        paper,
        scissors,
        scissors,
        scissors,
        scissors,
        // imp,
    };

    // check game status
    switch (corewar_.new_game(filenames))
    {
        case State::ERR_INI:       // bad/missing 'corewar.ini'
            printf("\n Error: bad/missing 'corewar.ini' (EXIT_FAILURE)");
            return EXIT_FAILURE;
        
        case State::ERR_WARRIORS:  // missing warrior
            printf("\n Error: selected warriors missing or contain invalid assembly (EXIT_FAILURE)");
            return EXIT_FAILURE;
    }
    static int round_tracker = corewar_.round();    // used to notify GUI

/** COREWAR:GUI:INIT: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    /* Main Viewport */
    static ImGuiWindowFlags main_window_flags ( 
        GUI::global_window_flags
        // extra
        | ImGuiWindowFlags_NoTitleBar
        | ImGuiWindowFlags_NoBringToFrontOnFocus
        | ImGuiWindowFlags_NoNavFocus
        | ImGuiWindowFlags_NoBackground
    );

    /* OS */
    GUI::Memory::init(&corewar_);

/** LOOP:START: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    while (!glfwWindowShouldClose(window)) {

        
    /** GAME:TURN:START: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
        
        corewar_.next_turn();

        if (corewar_.state() != Corewar::State::RUNNING)
        {   
            Player winner_ = corewar_.match_winner();
            if (winner_!= Player::NONE)
            {
                printf("\n MATCH Winner! . . . %s \n", corewar_.player_to_str(winner_));
            }
            printf("\n Corewar: NOT Running . . . Game Reset \n");
            corewar_.reset_game();
        }

        if (round_tracker != corewar_.round())
        {
            round_tracker = corewar_.round();
           
            Player winner_ = corewar_.round_winner();
            printf("\n Round Winner! . . . %s \n", corewar_.player_to_str(winner_));
            GUI::Memory::reset_display();
        }

    /** GAME:TURN:END: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();

        ImGui::NewFrame();
    /** FRAME:START: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


    //  /* Fullscreen */
        static ImGuiViewport const *viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->Pos);
        ImGui::SetNextWindowSize(viewport->Size);
        ImGui::SetNextWindowViewport(viewport->ID);

        static int main_push = 2;
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

    //  /* Main Display */
        ImGui::Begin("Corewar", NULL, main_window_flags);       ImGui::PopStyleVar(main_push);
        {
         /* Set Multi-Viewport */
         /* Set Docking */
            if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
            {
               ImGuiID dockspace_id = ImGui::GetID("Main Dockspace");
               ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), GUI::global_dockspace_flags);
               ImGuiDockNode* Node  = ImGui::DockBuilderGetNode( dockspace_id );
               Node->LocalFlags    |= GUI::global_dockspace_flags_private_override;
            }
         /** COREWAR:GUI: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
            GUI::Memory::update_cells();
            GUI::Memory::draw();
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
