#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "../include/TrajectoryCalculator.h"
#include "../include/FilesManager.h"
#include "../include/Simulation.h"
#include "../include/Displaying.h"
#include <iostream>
#include <vector>
#include <filesystem>

using namespace std;

int main() {
    TrajectoryCalculator calculator;
    /*calculator.CalculateData(50.0, 45.0, 0.05, 2.0, 9.81, 5.0, 180.0, 1.225, 0.0);
    vector<double> xAxis = calculator.getXAxisCoordinates();
    vector<double> yAxis = calculator.getYAxisCoordinates();

    for (size_t i = 0; i < xAxis.size(); ++i) {
        cout << i << ". " << xAxis[i] << " - " << yAxis[i] << endl;
    }
    
    string resultWarning = calculator.getWarning();

    calculator.CalculateData(25.0, 45.0, 0.05, 0.1, 9.81, 0.0, 0.0, 0.0, 0.0);
    vector<double> xAxis2 = calculator.getXAxisCoordinates();
    vector<double> yAxis2 = calculator.getYAxisCoordinates();

    for (size_t i = 0; i < xAxis2.size(); ++i) {
        cout << i << ". " << xAxis2[i] << " - " << yAxis2[i] << endl;
    }*/

    //cout << "Do you want to save? (Y/n)";
    //char answer;
    //cin >> answer;
    //if (answer == 'y')
    //{
    //    Simulation *simulation = new Simulation({50.0, 45.0, 0.05, 2.0, 9.81, 5.0, 180.0, 1.225, 0.0, xAxis, yAxis, resultWarning});
    //    cout << "Name a file: ";
    //    string fileName;
    //    cin >> fileName;
    //    if(filesystem::exists("data/"+fileName+".bin"))
    //    {
    //        cout << "Do you want to overwrite it? (Y/n)";
    //        char answer;
    //        cin >> answer;
    //        if (answer == 'y')
    //        {
    //            FilesManager *fileManager = new FilesManager();
    //            fileManager->saveSimulationData(simulation, fileName);
    //            delete fileManager;
    //        }
    //    }
    //    else
    //    {
    //        FilesManager *fileManager = new FilesManager();
    //        fileManager->saveSimulationData(simulation, fileName);
    //        delete fileManager;
    //    }
    //    // delete simulation;
    //}

    glfwInit();
    // set the opengl version 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    // set the glfw core profile, so only for the modern functions
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Set the window to be maximized with control buttons
    glfwWindowHint(GLFW_DECORATED, GLFW_TRUE);
    glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);


    // Create the window 800x800
    GLFWwindow* window = glfwCreateWindow(800, 800, "Trajectory Simulations", NULL, NULL);
    // If the window fails to create
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    int WINDOW_WIDTH, WINDOW_HEIGHT;
    glfwGetWindowSize(window, &WINDOW_WIDTH, &WINDOW_HEIGHT);
    cout << WINDOW_WIDTH << " x " << WINDOW_HEIGHT << endl;
    
    // introduce the window to the current context
    glfwMakeContextCurrent(window);

    // load GLAD so it configures opengl
    gladLoadGL();

    // specify the viewport of OpenGL in the Window
    glViewport(0, 0, 800, 800);

    // initialize imgui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    const int WELCOME_WINDOW_WIDTH = 400;
    const int WELCOME_WINDOW_HEIGHT = 300;

    Displaying displaying = Displaying::WelcomingMenu;

    // variables for input data
    double ballVelocity = 25.0;
    double firingAngle = 45.0;
    double ballRadius = 0.05;
    double ballMass = 0.1;
    double gravitationalAcceleration = 9.81;
    double windVelocity = 2;
    double windAngle = 180.0;
    double atmosphericDensity = 1.225;
    double initialDistanceFromGround = 1.0;

    bool windEnable = true;
    bool gravityEnable = true;
    bool atmosphereEnable = true;


    // render loop
    while (!glfwWindowShouldClose(window))
    {

        // rendering commands here
        glClearColor(0.07f, 0.13f, 0.17f, 1.0f); // background color
        glClear(GL_COLOR_BUFFER_BIT); // clean the back buffer and assign the new color to it

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        //if (!io.WantCaptureMouse) // for ignoring mouse inputs, when hovering over the imgui subwindow
        //{
        //    // your input functions here
        //}
        switch (displaying)
        {
            case Displaying::WelcomingMenu:
            {
                // Welcome window:
                ImGui::SetNextWindowSize(ImVec2(WELCOME_WINDOW_WIDTH, WELCOME_WINDOW_HEIGHT));
                ImGui::SetNextWindowPos(ImVec2(WINDOW_WIDTH / 2 - WELCOME_WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2 - WELCOME_WINDOW_HEIGHT / 2));
                if (ImGui::Begin("Welcome window", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse)) {
                    // Question
                    const char* text = "Create a new simulation or read from file?";
                    float textWidth = ImGui::CalcTextSize(text).x;
                    ImGui::SetCursorPosX((WELCOME_WINDOW_WIDTH - textWidth) * 0.5f);
                    ImGui::SetCursorPosY(WELCOME_WINDOW_HEIGHT / 2 - ImGui::CalcTextSize(text).y);

                    ImGui::Text(text);

                    // Buttons
                    float buttonWidth = 120.0f;
                    float spacing = 10.0f;
                    float totalWidth = (buttonWidth * 2) + spacing;
                    ImGui::SetCursorPosX((WELCOME_WINDOW_WIDTH - totalWidth) * 0.5f);

                    if (ImGui::Button("Create new", ImVec2(buttonWidth, 0)))
                    {
                        cout << "created new" << endl;
                        displaying = Displaying::CreationMenu;
                    }
                    ImGui::SameLine(0, spacing);
                    if (ImGui::Button("Read from file", ImVec2(buttonWidth, 0)))
                    {
                        cout << "reading from file" << endl;
                        displaying = Displaying::SimulationMenu;
                    }
                }ImGui::End();
                break;
            }
            case Displaying::CreationMenu:
            {

                break;
            }
            case Displaying::SimulationMenu:
            {
                break;
            }
        }

        // render the imgui elements
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window); // swap back buffer with the front one
        glfwPollEvents(); // take care of all glfw events
    }

    // delete all imgui instances
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    // delete window and glfw
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}