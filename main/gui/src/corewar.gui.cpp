/// Corewar GUI using 'Dear ImGui'

#include "corewar.gui.hpp"

int main() {

/** IMGUI:SETUP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(1366, 768, "Corewar",
                                          nullptr, nullptr);
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

    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.IniFilename = "imgui.ini";
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(GLSL_VERSION);

/** COREWAR:GAME:SETUP: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    using namespace Corewar;
    static Game corewar_;       // running game of corewar

    char constexpr rock[]     = "rock.asm",
                   paper[]    = "paper.asm",
                   scissors[] = "scissors.asm";

    WarriorFiles filenames = { rock, scissors }; // , paper, scissors};

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

    /** COREWAR:GUI:INIT: */
    static ImGuiWindowFlags main_window_flags( 
        GUI::global_abs_flags
        // extra
        | ImGuiWindowFlags_NoTitleBar
        | ImGuiWindowFlags_NoBackground
        | ImGuiWindowFlags_NoBringToFrontOnFocus
        | ImGuiWindowFlags_NoInputs
        | ImGuiWindowFlags_NoDocking
    );
    GUI::Memory::init(&corewar_);

/** LOOP:START: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    while (!glfwWindowShouldClose(window)) {
        // Poll and handle events (inputs, window resize, etc.)
        glfwPollEvents();

        glClearColor(0, 0, 0, 0);
        glClear(GL_COLOR_BUFFER_BIT);

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        
    /** GAME:TURN:START: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
        
        corewar_.next_turn();

        if (corewar_.state() != Corewar::State::RUNNING)
        {   
            Player winner_ = corewar_.match_winner();
            if (winner_!= Player::NONE)
            {
                printf("\n MATCH Winner! . . . P%d --> '%s' \n", 
                        (int) winner_,
                        corewar_.warrior(winner_).name().c_str()
                );
            }
            printf("\n Corewar: NOT Running . . . Game Reset \n");
            corewar_.reset_game();
        }

        if (round_tracker != corewar_.round())
        {
            round_tracker = corewar_.round();
           
            Player winner_ = corewar_.round_winner();
            printf("\n Round Winner! . . . P%d \n", (int) winner_);
            GUI::Memory::reset_display();
        }

    /** GAME:TURN:END: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    /** FRAME:START: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

        /* Fullscreen */
        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->WorkPos,   ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(viewport->WorkSize, ImGuiCond_FirstUseEver);

        /* Main Window */
        ImGui::Begin("Corewar", NULL, main_window_flags);
        {
         /* System Displays */
            ImGui::BeginChild("System Displays");
            {
                GUI::Memory::update_cells();
                GUI::Memory::draw();


            }
            ImGui::EndChild();

        }
        ImGui::End();

    /** FRAME:END: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }
/** LOOP:END: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return EXIT_SUCCESS;
}
