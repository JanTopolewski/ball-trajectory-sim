#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "../include/TrajectoryCalculator.h"
#include "../include/FilesManager.h"
#include "../include/Simulation.h"
#include "../include/Displaying.h"
#include "../include/SpaceObject.h"
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

    const int CREATION_WINDOW_WIDTH = 800;
    const int CREATION_WINDOW_HEIGHT = 800;

    // variables for input data
    float ballVelocity = 25.0;
    float firingAngle = 45.0;
    float ballRadius = 0.05;
    float ballMass = 0.1;
    float gravitationalAcceleration = 9.81;
    float windVelocity = 2;
    float windAngle = 180.0;
    float atmosphericDensity = 1.225;
    float initialDistanceFromGround = 1.0;

    bool windEnable = true;
    bool gravityEnable = true;
    bool atmosphereEnable = true;

    // reading planet data from file
    //reading from file
    FilesManager* fileManager = new FilesManager();
    fileManager->loadSpaceObjectsData();

    vector<string> planets = fileManager->getSpaceObjectsNames();

    // Convert vector<string> to const char* array 
    const char* label = "Custom";
    vector<const char*> planetNamesCStr;

    planetNamesCStr.reserve(planets.size() + (size_t)sizeof(label));
    planetNamesCStr.push_back(label);

    for (const auto& planet : planets) {
        cout << planet << endl;
        planetNamesCStr.push_back(planet.c_str());
    }
    static int currentPlanet = 0;

    // loading .csv file to some sort of array
    vector<SpaceObject> planetsData = fileManager->getSpaceObjectsData();


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
                // Creation window:
                ImGui::SetNextWindowSize(ImVec2(CREATION_WINDOW_WIDTH, CREATION_WINDOW_HEIGHT));
                ImGui::SetNextWindowPos(ImVec2(WINDOW_WIDTH / 2 - CREATION_WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2 - CREATION_WINDOW_HEIGHT / 2));
                if (ImGui::Begin("Create a new simulation", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse)) {

                    ImGui::Text("Enter values by adjusting sliders or by Ctrl+click to enter a specific number: ");

                    ImGui::SliderFloat("Initial ball velocity", &ballVelocity, 0.1f, 200.0f, "%.7f", ImGuiSliderFlags_AlwaysClamp);
                    ImGui::SliderFloat("Firing angle", &firingAngle, 0.0f, 90.0f, "%.7f", ImGuiSliderFlags_AlwaysClamp);
                    ImGui::SliderFloat("Ball radius", &ballRadius, 0.01f, 5.0f, "%.7f", ImGuiSliderFlags_AlwaysClamp);
                    ImGui::SliderFloat("Ball mass", &ballMass, 0.001f, 1000000.0f, "%.7f", ImGuiSliderFlags_AlwaysClamp);
                    ImGui::InputFloat("Initial distance from ground", &initialDistanceFromGround, 0.01f, 5.0f, "%.2f");

                    // Spacing
                    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + ImGui::GetTextLineHeightWithSpacing());

                    if (currentPlanet != 0)
                    {
                        SpaceObject planet = planetsData[currentPlanet - 1];

                        gravitationalAcceleration = (float)planet.gravitationalAcceleration;
                        atmosphericDensity = (float)planet.atmosphereDensity;
                    }

                    if (currentPlanet != 0) ImGui::BeginDisabled();
                    ImGui::Checkbox("Enable gravity", &gravityEnable);
                    if (!gravityEnable && currentPlanet == 0) ImGui::BeginDisabled();
                    ImGui::SliderFloat("Gravitational acceleration", &gravitationalAcceleration, 0.0f, 24.0f, "%.7f", ImGuiSliderFlags_AlwaysClamp);
                    if (!gravityEnable || currentPlanet != 0) ImGui::EndDisabled();

                    ImGui::Checkbox("Enable wind", &windEnable);
                    if (!windEnable) ImGui::BeginDisabled();
                    ImGui::SliderFloat("Wind Velocity", &windVelocity, 0.0f, 80.0f, "%.7f", ImGuiSliderFlags_AlwaysClamp);
                    ImGui::SliderFloat("Wind angle", &windAngle, 0.0f, 360.0f, "%.7f", ImGuiSliderFlags_AlwaysClamp);
                    if (!windEnable) ImGui::EndDisabled();

                    if (currentPlanet != 0) ImGui::BeginDisabled();
                    ImGui::Checkbox("Enable atmosphere", &atmosphereEnable);
                    if (!atmosphereEnable && currentPlanet == 0) ImGui::BeginDisabled();
                    ImGui::SliderFloat("Atmosferic density", &atmosphericDensity, 0.0f, 65.0f, "%.7f", ImGuiSliderFlags_AlwaysClamp);
                    if (!atmosphereEnable || currentPlanet != 0) ImGui::EndDisabled();

                    // Spacing
                    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + ImGui::GetTextLineHeightWithSpacing());
                    
                    ImGui::Text("You can also preset values according to specific planet (leave \"Custom\" if you want to adjust them for your own)");
                    ImGui::Combo("Select space object", &currentPlanet, planetNamesCStr.data(), planetNamesCStr.size());

                    if (ImGui::Button("Create simulation"))
                    {
                        displaying = Displaying::SimulationMenu;
                    }

                }ImGui::End();
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

    delete fileManager;

    // delete all imgui instances
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    // delete window and glfw
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}