#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "implot.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "../include/TrajectoryCalculator.h"
#include "../include/FilesManager.h"
#include "../include/Simulation.h"
#include "../include/Displaying.h"
#include "../include/SpaceObject.h"
#include <iostream>
#include <vector>
#include <string>
#include <filesystem>
#include <regex>
#include <algorithm>
#include <cmath>

using namespace std;

int main() {
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
    GLFWwindow* window = glfwCreateWindow(800, 800, "Trajectory Simulations", nullptr, nullptr);
    // If the window fails to create
    if (window == nullptr)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    int WINDOW_WIDTH, WINDOW_HEIGHT;
    glfwGetWindowSize(window, &WINDOW_WIDTH, &WINDOW_HEIGHT);
    
    // introduce the window to the current context
    glfwMakeContextCurrent(window);

    // load GLAD so it configures opengl
    gladLoadGL();

    // specify the viewport of OpenGL in the Window
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

    // initialize imgui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImPlot::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");


    // variables and object for displaying subwindows
    const int WELCOME_WINDOW_WIDTH = 800;
    const int WELCOME_WINDOW_HEIGHT = 600;

    const int CREATION_WINDOW_WIDTH = 800;
    const int CREATION_WINDOW_HEIGHT = 800;

    const int SETTINGS_WINDOW_WIDTH = 600;
    const int SETTINGS_WINDOW_HEIGHT = 600;

    Displaying displaying = Displaying::WelcomingMenu;


    // object for calculations
    TrajectoryCalculator calculator;

    // variables for input data
    float ballVelocity = 25.0f;
    float horizontalAngle = 90.0f;
    float verticalAngle = 45.0f;
    float ballRadius = 0.05f;
    float ballMass = 0.1f;
    float gravitationalAcceleration = 9.81f;
    float windVelocity = 2.0f;
    float windHorizontalAngle = 270.0f;
    float windVerticalAngle = 0.0f;
    float atmosphericDensity = 1.225f;
    float initialDistanceFromGround = 1.0f;
    float targetXDistance = 10.0f;
    float targetZDistance = 10.0f;
 
    bool hasTarget = true;
    bool windEnable = true;
    bool gravityEnable = true;
    bool atmosphereEnable = true;

    bool dataChanged = false;

    // variables for showing results
    vector<double> xAxis;
    vector<double> yAxis;
    vector<double> zAxis;
    string warning;

    // variables for animations in implot
    int plotFramesPerSecond = 1000;
    bool isPaused = false;
    float plotSpeedMultiplier = 1.0f;
    int secondsToPass = 5;
    static int currentIndex = 0;
    static double lastTime = ImGui::GetTime();
    bool axesSetting = false;

    double frameDelta = 0.0; // used for debugging plot fps

    // reading planet data from file
    //reading from file
    FilesManager* fileManager = new FilesManager();
    fileManager->loadSpaceObjectsData();

    vector<string> planets = fileManager->getSpaceObjectsNames();

    const char* defaultPlanetLabel = "Custom";
    vector<const char*> planetNamesCStr;

    planetNamesCStr.reserve(planets.size() + (size_t)sizeof(defaultPlanetLabel));
    planetNamesCStr.push_back(defaultPlanetLabel);

    for (const auto& planet : planets) {
        planetNamesCStr.push_back(planet.c_str());
    }
    static int currentPlanet = 0;

    // loading .csv file to some sort of array
    vector<SpaceObject> planetsData = fileManager->getSpaceObjectsData();

    // preparing ODE solvers list
    vector<string> solvers = { "RK4", "Euler", "Verlet" };

    vector<const char*> solversCStr;
    solversCStr.reserve(solvers.size());

    for (auto& solver : solvers) {
        solversCStr.push_back(solver.c_str());
    }
    static int currentSolver = 0;

    int chosenFile = 0;
    vector<string> fileNames;
    vector<const char*> fileNamesCStr;

    // settings
    float fontSizeMultiplier = 1.0f;
    float backgroundColor[4] = {0.07f, 0.13f, 0.17f, 1.0f};

    // render loop
    while (!glfwWindowShouldClose(window))
    {
        // rendering commands here
        glClearColor(backgroundColor[0], backgroundColor[1], backgroundColor[2], backgroundColor[3]); // background color
        glClear(GL_COLOR_BUFFER_BIT); // clean the back buffer and assign the new color to it

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        switch (displaying)
        {
            case Displaying::WelcomingMenu:
            {
                // Welcome window:
                ImGui::SetNextWindowSize(ImVec2(WELCOME_WINDOW_WIDTH, WELCOME_WINDOW_HEIGHT));
                ImGui::SetNextWindowPos(ImVec2(WINDOW_WIDTH / 2 - WELCOME_WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2 - WELCOME_WINDOW_HEIGHT / 2));
                if (ImGui::Begin("Welcome window", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse))
                {
                    auto greeting = "Hello user!";
                    auto welcome = "Welcome in our application!";
                    auto proposition = "Have fun shoooting the ball!";
                    ImGui::SetWindowFontScale(2.0f * fontSizeMultiplier);
                    ImGui::SetCursorPosX((WELCOME_WINDOW_WIDTH - ImGui::CalcTextSize(greeting).x) * 0.5f);
                    ImGui::Text(greeting);
                    ImGui::SetCursorPosX((WELCOME_WINDOW_WIDTH - ImGui::CalcTextSize(welcome).x) * 0.5f);
                    ImGui::Text(welcome);
                    ImGui::SetCursorPosX((WELCOME_WINDOW_WIDTH - ImGui::CalcTextSize(proposition).x) * 0.5f);
                    ImGui::Text(proposition);
                    ImGui::SetWindowFontScale(1.2f * fontSizeMultiplier);

                    ImGui::Spacing();
                    ImGui::Text(""); // just for spacing, otherwise there is no spacing visible

                    // Buttons
                    float buttonWidth = 220.0f;

                    ImGui::SetCursorPosX((WELCOME_WINDOW_WIDTH - buttonWidth) * 0.5f);
                    if (ImGui::Button("Create new simulation", ImVec2(buttonWidth, 0)))
                    {
                        displaying = Displaying::CreationMenu;
                    }

                    ImGui::SetCursorPosX((WELCOME_WINDOW_WIDTH - buttonWidth) * 0.5f);
                    if (ImGui::Button("Read from file", ImVec2(buttonWidth, 0)))
                    {
                        // get the possible filenames
                        fileNames = fileManager->getSavedSimulationsNames();

                        fileNamesCStr.clear();
                        for (const auto& file : fileNames) {
                            fileNamesCStr.push_back(file.c_str());
                        }

                        displaying = Displaying::ReadFileMenu;
                    }

                    ImGui::SetCursorPosX((WELCOME_WINDOW_WIDTH - buttonWidth) * 0.5f);
                    if (ImGui::Button("Settings", ImVec2(buttonWidth, 0)))
                    {
                        displaying = Displaying::SettingsMenu;
                    }

                    ImGui::SetCursorPosX((WELCOME_WINDOW_WIDTH - buttonWidth) * 0.5f);
                    if (ImGui::Button("Exit", ImVec2(buttonWidth, 0)))
                    {
                        glfwSetWindowShouldClose(window, GLFW_TRUE);
                    }

                    ImGui::SetWindowFontScale(fontSizeMultiplier); // reset the font size
                }ImGui::End();
                break;
            }
            case Displaying::ReadFileMenu:
            {
                // Read file window:
                ImGui::SetNextWindowSize(ImVec2(WELCOME_WINDOW_WIDTH, WELCOME_WINDOW_HEIGHT));
                ImGui::SetNextWindowPos(ImVec2(WINDOW_WIDTH / 2 - WELCOME_WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2 - WELCOME_WINDOW_HEIGHT / 2));
                if (ImGui::Begin("Choose a simulation to read from", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse))
                {
                    ImGui::SetWindowFontScale(1.2f * fontSizeMultiplier);
                    ImGui::Text("Choose a simulation from the list: ");

                    ImGui::SetWindowFontScale(fontSizeMultiplier);
                    ImGui::Combo(" ", &chosenFile, fileNamesCStr.data(), fileNamesCStr.size());

                    if (ImGui::Button("Select"))
                    {
                        Simulation* sim = fileManager->readSimulationData(fileNamesCStr[chosenFile]);
                        ballVelocity = sim->ballVelocity;
                        horizontalAngle = sim->horizontalAngle;
                        ballRadius = sim->ballRadius;
                        ballMass = sim->ballMass;
                        initialDistanceFromGround = sim->initialDistanceFromGround;
                        targetXDistance = sim->targetYDistance;
                        targetZDistance = sim->targetXDistance;
                        windVelocity = sim->windVelocity;
                        windHorizontalAngle = sim->windHorizontalAngle;
                        gravitationalAcceleration = sim->gravitationalAcceleration;
                        atmosphericDensity = sim->atmosfericDensity;
                        xAxis = sim->yAxisCoordinates; // The change in axis results from a formal change in orientation from a mathematical coordinate system to one in OpenGL
                        yAxis = sim->zAxisCoordinates;
                        zAxis = sim->xAxisCoordinates;
                        warning = sim->warning;
                        currentSolver = sim->odeSolver;

                        lastTime = ImGui::GetTime();
                        currentIndex = 0;
                        axesSetting = true;

                        displaying = Displaying::SimulationMenu;
                    }
                    if (ImGui::Button("Back"))
                    {
                        displaying = Displaying::WelcomingMenu;
                    }
                }ImGui::End();
                break;
            }
            case Displaying::SaveFileMenu:
            {
                ImGui::SetNextWindowSize(ImVec2(WELCOME_WINDOW_WIDTH, WELCOME_WINDOW_HEIGHT));
                ImGui::SetNextWindowPos(ImVec2(WINDOW_WIDTH / 2 - WELCOME_WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2 - WELCOME_WINDOW_HEIGHT / 2));
                if (ImGui::Begin("Save simulation", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse)) {
                    ImGui::SetWindowFontScale(fontSizeMultiplier);
                    static char buf[31] = "";
                    regex pattern("^[A-Za-z0-9_-]+$");

                    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
                    ImGui::InputTextWithHint("##FileName", "Simulation name (maximum 30 characters)", buf, IM_ARRAYSIZE(buf));
                    string filename(buf);

                    if (!regex_match(filename, pattern)) {
                        ImGui::PushTextWrapPos(0.0f);
                        ImGui::TextColored(ImVec4(1, 0, 0, 1), "The simulation name must consist only of english letters, numbers and the characters - and _");
                        ImGui::PopTextWrapPos();
                    }
                    else if (fileManager->checkFileExistence(filename)) {
                        ImGui::PushTextWrapPos(0.0f);
                        ImGui::TextColored(ImVec4(1, 0, 0, 1), "This simulation name is already used");
                        ImGui::PopTextWrapPos();
                    }

                    if (ImGui::Button("Save")) {
                        if (regex_match(filename, pattern) && !fileManager->checkFileExistence(filename)) {
                            Simulation* sim = new Simulation({ ballVelocity, horizontalAngle, verticalAngle, ballRadius, ballMass, gravitationalAcceleration, windVelocity, windHorizontalAngle, windVerticalAngle, atmosphericDensity, initialDistanceFromGround, zAxis, xAxis, yAxis, warning, {}, {}, {}, hasTarget, targetZDistance, targetXDistance, currentSolver });
                            fileManager->saveSimulationData(sim, filename);
                            displaying = Displaying::WelcomingMenu;
                            //memset(buf, 0, sizeof(buf));
                        }
                    }
                }ImGui::End();
                break;
            }
            case Displaying::CreationMenu:
            {
                // Creation window:
                ImGui::SetNextWindowSize(ImVec2(CREATION_WINDOW_WIDTH, CREATION_WINDOW_HEIGHT));
                ImGui::SetNextWindowPos(ImVec2(WINDOW_WIDTH / 2 - CREATION_WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2 - CREATION_WINDOW_HEIGHT / 2));
                if (ImGui::Begin("Create a new simulation", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_HorizontalScrollbar)) {
                    ImGui::SetWindowFontScale(1.2f * fontSizeMultiplier);
                    ImGui::Text("Enter values by adjusting sliders or by Ctrl+click to enter a specific number: ");

                    ImGui::SetWindowFontScale(fontSizeMultiplier);
                    ImGui::SliderFloat("Initial ball velocity", &ballVelocity, 0.1f, 200.0f, "%.7f", ImGuiSliderFlags_AlwaysClamp);
                    ImGui::SliderFloat("Horizontal firing angle", &horizontalAngle, 0.0f, 360.0f, "%.7f", ImGuiSliderFlags_AlwaysClamp);
                    ImGui::SliderFloat("Vertical firing angle", &verticalAngle, 0.0f, 90.0f, "%.7f", ImGuiSliderFlags_AlwaysClamp);
                    ImGui::SliderFloat("Ball radius", &ballRadius, 0.01f, 5.0f, "%.7f", ImGuiSliderFlags_AlwaysClamp);
                    ImGui::SliderFloat("Ball mass", &ballMass, 0.001f, 1000000.0f, "%.7f", ImGuiSliderFlags_AlwaysClamp);
                    if (ImGui::InputFloat("Initial distance from ground", &initialDistanceFromGround, 0.01f, 5.0f, "%.2f")) {
                        if (initialDistanceFromGround < 0.0f) initialDistanceFromGround = 0.0f;
                    }
                    ImGui::Checkbox("Enable target", &hasTarget);
                    if (!hasTarget) ImGui::BeginDisabled();
                    ImGui::InputFloat("Distance to target on the x-axis", &targetZDistance, 0.1f, 1.0f, "%.2f");
                    ImGui::InputFloat("Distance to target on the y-axis", &targetXDistance, 0.1f, 1.0f, "%.2f");
                    if (!hasTarget) ImGui::EndDisabled();

                    // Spacing
                    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + ImGui::GetTextLineHeightWithSpacing());

                    const bool planet_selected = currentPlanet != 0; // just for clarity
                    if (planet_selected)
                    {
                        SpaceObject planet = planetsData[currentPlanet - 1];

                        gravitationalAcceleration = (float)planet.gravitationalAcceleration;
                        atmosphericDensity = (float)planet.atmosphereDensity;
                    }

                    if (planet_selected) ImGui::BeginDisabled();
                    ImGui::Checkbox("Enable gravity", &gravityEnable);
                    if (!gravityEnable && !planet_selected) ImGui::BeginDisabled();
                    ImGui::SliderFloat("Gravitational acceleration", &gravitationalAcceleration, 0.0f, 24.0f, "%.7f", ImGuiSliderFlags_AlwaysClamp);
                    if (!gravityEnable || planet_selected) ImGui::EndDisabled();

                    if (atmosphericDensity == 0.0f) ImGui::BeginDisabled();
                    ImGui::Checkbox("Enable wind", &windEnable);
                    if (!windEnable && atmosphericDensity != 0.0f) ImGui::BeginDisabled();
                    ImGui::SliderFloat("Wind Velocity", &windVelocity, 0.0f, 80.0f, "%.7f", ImGuiSliderFlags_AlwaysClamp);
                    ImGui::SliderFloat("Horizontal wind angle", &windHorizontalAngle, 0.0f, 360.0f, "%.7f", ImGuiSliderFlags_AlwaysClamp);
                    ImGui::SliderFloat("Vertical wind angle", &windVerticalAngle, -90.0f, 90.0f, "%.7f", ImGuiSliderFlags_AlwaysClamp);
                    if (!windEnable || atmosphericDensity == 0.0f) ImGui::EndDisabled();

                    if (planet_selected) ImGui::BeginDisabled();
                    ImGui::Checkbox("Enable atmosphere", &atmosphereEnable);
                    if (!atmosphereEnable && !planet_selected) ImGui::BeginDisabled();
                    ImGui::SliderFloat("Atmosferic density", &atmosphericDensity, 0.0f, 65.0f, "%.7f", ImGuiSliderFlags_AlwaysClamp);
                    if (!atmosphereEnable || planet_selected) ImGui::EndDisabled();

                    if (!atmosphereEnable) ImGui::BeginDisabled();
                    ImGui::Combo("Select calculation method", &currentSolver, solversCStr.data(), solversCStr.size());
                    if (!atmosphereEnable) ImGui::EndDisabled();

                    // Spacing
                    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + ImGui::GetTextLineHeightWithSpacing());
                    
                    ImGui::PushTextWrapPos(0.0f);
                    ImGui::SetWindowFontScale(1.2f * fontSizeMultiplier);
                    ImGui::Text("You can also preset values according to specific space object (leave \"Custom\" if you want to adjust them for your own)");
                    ImGui::PopTextWrapPos();
                    ImGui::SetWindowFontScale(fontSizeMultiplier);
                    ImGui::Combo("Select space object", &currentPlanet, planetNamesCStr.data(), planetNamesCStr.size());

                    if (ImGui::Button("Create simulation"))
                    {
                        if (!atmosphereEnable) atmosphericDensity = 0.0f;
                        if (!windEnable || atmosphericDensity == 0.0f) {
                            windVelocity = 0.0f;
                        }
                        if (!gravityEnable) gravitationalAcceleration = 0.0f;
                        if (!hasTarget) {
                            targetXDistance = 0.0f;
                            targetZDistance = 0.0f;
                        }
                        calculator.CalculateData(
                            (double)ballVelocity,
                            (double)horizontalAngle,
                            (double)verticalAngle,
                            (double)ballRadius,
                            (double)ballMass,
                            (double)gravitationalAcceleration,
                            (double)windVelocity,
                            (double)windHorizontalAngle,
                            (double)windVerticalAngle,
                            (double)atmosphericDensity,
                            (double)initialDistanceFromGround,
                            currentSolver
                        );
                        
                        xAxis = calculator.getYAxisCoordinates();
                        yAxis = calculator.getZAxisCoordinates();
                        zAxis = calculator.getXAxisCoordinates();
                        warning = calculator.getWarning();
                        lastTime = ImGui::GetTime();
                        currentIndex = 0;
                        axesSetting = true;

                        displaying = Displaying::SimulationMenu;
                    }
                    if (ImGui::Button("Back"))
                    {
                        displaying = Displaying::WelcomingMenu;
                    }
                }ImGui::End();
                break;
            }
            case Displaying::SimulationMenu:
            {
                // needed for calculating the position of "Simulation control" window
                ImVec2 dataWindowPos;
                ImVec2 dataWindowSize;

                ImGui::SetNextWindowSize(ImVec2(WINDOW_WIDTH * 2 / 3, WINDOW_HEIGHT - 20));
                ImGui::SetNextWindowPos(ImVec2(ImGui::GetMainViewport()->Pos.x + 10, ImGui::GetMainViewport()->Pos.y + 10), ImGuiCond_FirstUseEver);
                if (ImGui::Begin("Trajectory"))
                {
                    ImVec2 cursor = ImGui::GetCursorPos();

                    ImGui::SetCursorPos(ImVec2(cursor.x + 50, cursor.y + 50));

                    double now = ImGui::GetTime();
                    bool animationFinished = currentIndex >= (int)xAxis.size();
                    frameDelta = now - lastTime;
                    int mult = (int)((frameDelta / (1/(double)plotFramesPerSecond)) * (double)plotSpeedMultiplier);
                    if (now - lastTime >= ((1/(double)plotFramesPerSecond) * (double)plotSpeedMultiplier) && !animationFinished && !isPaused)
                    {
                        currentIndex = min(currentIndex + mult, (int)xAxis.size());
                    }
                    lastTime = now;

                    auto [xMinTemp, xMaxTemp] = minmax_element(xAxis.begin(), xAxis.end());
                    auto [yMinTemp, yMaxTemp] = minmax_element(yAxis.begin(), yAxis.end());
                    double xMin = *xMinTemp;
                    double xMax = *xMaxTemp;
                    double yMin = *yMinTemp;
                    double yMax = *yMaxTemp;
                    if (yMin > 0.0) {
                        yMin = 0.0;
                    }

                    double xMargin = (xMax - xMin) * 0.1;
                    double yMargin = (yMax - yMin) * 0.1;

                    if (axesSetting) {
                        ImPlot::SetNextAxesLimits(xMin - xMargin, xMax + xMargin, yMin - yMargin, yMax + yMargin, ImPlotCond_Always);
                        axesSetting = false;
                    }
                    else {
                        ImPlot::SetNextAxesLimits(xMin - xMargin, xMax + xMargin, yMin - yMargin, yMax + yMargin, ImPlotCond_Once);
                    }

                    if (ImPlot::BeginPlot("Space", ImVec2(-1, -1), ImPlotFlags_Equal | ImPlotFlags_NoTitle | ImPlotFlags_NoLegend)) {
                        if (currentIndex > 1) {
                            ImPlot::SetNextLineStyle(ImVec4(1, 0, 0, 1), 2.0f);
                            ImPlot::PlotLine("Trajectory", xAxis.data(), yAxis.data(), currentIndex);

                            ImVec2 plotSize = ImPlot::GetPlotSize();

                            float pixelsPerUnitX = plotSize.x / (float)(ImPlot::GetPlotLimits().X.Max - ImPlot::GetPlotLimits().X.Min);
                            float markerSize = 2 * (float)ballRadius * pixelsPerUnitX;

                            ImPlot::SetNextMarkerStyle(ImPlotMarker_Circle, markerSize, ImVec4(1, 0, 0, 1), 0.0f, ImVec4(0, 0, 0, 0));
                            ImPlot::PlotScatter("Ball", &xAxis[currentIndex - 1], &yAxis[currentIndex - 1], 1);

                            if (hasTarget && !dataChanged) {
                                ImPlot::SetNextMarkerStyle(ImPlotMarker_Circle, 4.0f, ImVec4(1, 1, 0, 1), 0.0f, ImVec4(0, 0, 0, 0));
                                double targetX = (double)targetXDistance;
                                double targetY = 0.0;
                                ImPlot::PlotScatter("Target", &targetX, &targetY, 1);
                            }
                        }
                        ImPlot::EndPlot();
                    }

                    ImGui::SetCursorPos(ImVec2(cursor.x + 50, cursor.y));

                    string warningMessage = "Warning: " + warning;
                    if(warningMessage != "Warning: ") ImGui::TextColored(ImVec4(1, 0, 0, 1), warningMessage.c_str());

                    cursor = ImGui::GetCursorPos();
                    ImGui::SetCursorPos(ImVec2(cursor.x + 50, cursor.y));

                    if (hasTarget && !dataChanged && animationFinished)
                    {
                        if (sqrt(pow(xAxis.back() - targetXDistance, 2) + pow(zAxis.back() - targetZDistance, 2) + yAxis.back() * yAxis.back()) <= ballRadius && yAxis.back() - ballRadius <= 0)
                        {
                            ImGui::TextColored(ImVec4(0, 1, 0, 1), "The ball hit the target");
                        }
                        else
                        {
                            ImGui::TextColored(ImVec4(1, 0, 0, 1), "The ball missed the target");
                        }
                    }
                }
                ImGui::End();

                ImGui::SameLine();

                if (ImGui::Begin("Data", nullptr, ImGuiWindowFlags_HorizontalScrollbar))
                {
                    dataWindowPos = ImGui::GetWindowPos();
                    dataWindowSize = ImGui::GetWindowSize();

                    ImGui::SetWindowFontScale(1.2f * fontSizeMultiplier);
                    ImGui::Text("Enter values by adjusting sliders or by Ctrl+click to enter a specific number: ");

                    ImGui::SetWindowFontScale(fontSizeMultiplier);
                    if (ImGui::SliderFloat("Initial ball velocity", &ballVelocity, 0.1f, 200.0f, "%.7f", ImGuiSliderFlags_AlwaysClamp)) dataChanged = true;
                    if (ImGui::SliderFloat("Horizontal firing angle", &horizontalAngle, 0.0f, 360.0f, "%.7f", ImGuiSliderFlags_AlwaysClamp)) dataChanged = true;
                    if (ImGui::SliderFloat("Vertical firing angle", &verticalAngle, 0.0f, 90.0f, "%.7f", ImGuiSliderFlags_AlwaysClamp)) dataChanged = true;
                    if (ImGui::SliderFloat("Ball radius", &ballRadius, 0.01f, 5.0f, "%.7f", ImGuiSliderFlags_AlwaysClamp)) dataChanged = true;
                    if (ImGui::SliderFloat("Ball mass", &ballMass, 0.001f, 1000000.0f, "%.7f", ImGuiSliderFlags_AlwaysClamp)) dataChanged = true;
                    if (ImGui::InputFloat("Initial distance from ground", &initialDistanceFromGround, 0.01f, 5.0f, "%.2f")) {
                        if (initialDistanceFromGround < 0.0f) initialDistanceFromGround = 0.0f;
                        dataChanged = true;
                    }
                    if (ImGui::Checkbox("Enable target", &hasTarget)) dataChanged = true;
                    if (!hasTarget) ImGui::BeginDisabled();
                    if (ImGui::InputFloat("Distance to target on the x-axis", &targetZDistance, 0.1f, 1.0f, "%.2f")) dataChanged = true;
                    if (ImGui::InputFloat("Distance to target on the y-axis", &targetXDistance, 0.1f, 1.0f, "%.2f")) dataChanged = true;
                    if (!hasTarget) ImGui::EndDisabled();

                    // Spacing
                    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + ImGui::GetTextLineHeightWithSpacing());

                    if (currentPlanet != 0)
                    {
                        SpaceObject planet = planetsData[currentPlanet - 1];

                        gravitationalAcceleration = (float)planet.gravitationalAcceleration;
                        atmosphericDensity = (float)planet.atmosphereDensity;
                    }

                    if (currentPlanet != 0) ImGui::BeginDisabled();
                    if (ImGui::Checkbox("Enable gravity", &gravityEnable)) dataChanged = true;
                    if (!gravityEnable && currentPlanet == 0) ImGui::BeginDisabled();
                    if (ImGui::SliderFloat("Gravitational acceleration", &gravitationalAcceleration, 0.0f, 24.0f, "%.7f", ImGuiSliderFlags_AlwaysClamp)) dataChanged = true;
                    if (!gravityEnable || currentPlanet != 0) ImGui::EndDisabled();

                    if (atmosphericDensity == 0.0f) ImGui::BeginDisabled();
                    if (ImGui::Checkbox("Enable wind", &windEnable)) dataChanged = true;
                    if (!windEnable && atmosphericDensity != 0) ImGui::BeginDisabled();
                    if (ImGui::SliderFloat("Wind Velocity", &windVelocity, 0.0f, 80.0f, "%.7f", ImGuiSliderFlags_AlwaysClamp)) dataChanged = true;
                    if (ImGui::SliderFloat("Horizontal wind angle", &windHorizontalAngle, 0.0f, 360.0f, "%.7f", ImGuiSliderFlags_AlwaysClamp)) dataChanged = true;
                    if (ImGui::SliderFloat("Vertical wind angle", &windVerticalAngle, -90.0f, 90.0f, "%.7f", ImGuiSliderFlags_AlwaysClamp)) dataChanged = true;
                    if (!windEnable || atmosphericDensity == 0.0f) ImGui::EndDisabled();

                    if (currentPlanet != 0) ImGui::BeginDisabled();
                    if (ImGui::Checkbox("Enable atmosphere", &atmosphereEnable)) dataChanged = true;
                    if (!atmosphereEnable && currentPlanet == 0) ImGui::BeginDisabled();
                    if (ImGui::SliderFloat("Atmosferic density", &atmosphericDensity, 0.0f, 65.0f, "%.7f", ImGuiSliderFlags_AlwaysClamp)) dataChanged = true;
                    if (!atmosphereEnable || currentPlanet != 0) ImGui::EndDisabled();

                    if (!atmosphereEnable) ImGui::BeginDisabled();
                    if (ImGui::Combo("Select calculation method", &currentSolver, solversCStr.data(), solversCStr.size())) dataChanged = true;
                    if (!atmosphereEnable) ImGui::EndDisabled();

                    // Spacing
                    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + ImGui::GetTextLineHeightWithSpacing());

                    if (ImGui::Combo("Select space object", &currentPlanet, planetNamesCStr.data(), planetNamesCStr.size())) dataChanged = true;

                    if (ImGui::Button("Confirm"))
                    {
                        if (!atmosphereEnable) atmosphericDensity = 0.0f;
                        if (!windEnable || atmosphericDensity == 0.0f) {
                            windVelocity = 0.0f;
                        }
                        if (!gravityEnable) gravitationalAcceleration = 0.0f;
                        if (!hasTarget) {
                            targetXDistance = 0.0f;
                            targetZDistance = 0.0f;
                        }
                        calculator.CalculateData(
                            (double)ballVelocity,
                            (double)horizontalAngle,
                            (double)verticalAngle,
                            (double)ballRadius,
                            (double)ballMass,
                            (double)gravitationalAcceleration,
                            (double)windVelocity,
                            (double)windHorizontalAngle,
                            (double)windVerticalAngle,
                            (double)atmosphericDensity,
                            (double)initialDistanceFromGround,
                            currentSolver
                        );

                        xAxis = calculator.getYAxisCoordinates();
                        yAxis = calculator.getZAxisCoordinates();
                        zAxis = calculator.getXAxisCoordinates();
                        warning = calculator.getWarning();
                        dataChanged = false;
                        lastTime = ImGui::GetTime();
                        currentIndex = 0;

                        axesSetting = true;
                    }

                    ImGui::SameLine();
                    if (ImGui::Button("Save results")) {
                        if (dataChanged) {
                            calculator.CalculateData(
                                (double)ballVelocity,
                                (double)horizontalAngle,
                                (double)verticalAngle,
                                (double)ballRadius,
                                (double)ballMass,
                                (double)gravitationalAcceleration,
                                (double)windVelocity,
                                (double)windHorizontalAngle,
                                (double)windVerticalAngle,
                                (double)atmosphericDensity,
                                (double)initialDistanceFromGround,
                                currentSolver
                            );

                            xAxis = calculator.getYAxisCoordinates();
                            yAxis = calculator.getZAxisCoordinates();
                            zAxis = calculator.getXAxisCoordinates();
                            warning = calculator.getWarning();

                            dataChanged = false;
                        }
                        displaying = Displaying::SaveFileMenu;
                    }
                    if (ImGui::Button("Back"))
                    {
                        displaying = Displaying::WelcomingMenu;
                    }
                }
                ImGui::End();

                ImGui::SetNextWindowPos(ImVec2(dataWindowPos.x, dataWindowPos.y + dataWindowSize.y + 10), ImGuiCond_FirstUseEver);
                ImGui::SetNextWindowSize(ImVec2(dataWindowSize.x, 0), ImGuiCond_FirstUseEver);

                if (ImGui::Begin("Simulation control"))
                {
                    // ImGui::Text(to_string(frameDelta).c_str());
                    // play/pause
                    auto buttonPlayPauseText = "";
                    if (isPaused)
                        buttonPlayPauseText = "Play";
                    else
                        buttonPlayPauseText = "Pause";
                    if (ImGui::Button(buttonPlayPauseText))
                    {
                        isPaused = !isPaused;
                    }

                    // start/end
                    if (ImGui::Button("Start"))
                    {
                        currentIndex = 0;
                    }

                    int animationLength = static_cast<int>(xAxis.size());

                    if (ImGui::Button("End"))
                    {
                        currentIndex = animationLength;
                    }

                    // slider with animation
                    ImGui::SliderInt(" ", &currentIndex, 0, animationLength, "%d", ImGuiSliderFlags_AlwaysClamp);

                    // speed multiplier
                    ImGui::InputFloat("Speed of the animation", &plotSpeedMultiplier, 0.25f, 1.0f, "%.2f");

                    // +/- frames, seconds etc.
                    if (ImGui::Button("Next frame"))
                    {
                        currentIndex = min(currentIndex+1, animationLength);
                    }
                    if (ImGui::Button("Previous frame"))
                    {
                        currentIndex = max(currentIndex-1, 0);
                    }
                    if (ImGui::Button("Add second"))
                    {
                        currentIndex = min(currentIndex + plotFramesPerSecond, animationLength);
                    }
                    if (ImGui::Button("Subtract second"))
                    {
                        currentIndex = max(currentIndex - plotFramesPerSecond, 0);
                    }
                    ImGui::InputInt("Seconds", &secondsToPass);
                    if (ImGui::Button(("Add " + to_string(secondsToPass) + " seconds").c_str()))
                    {
                        currentIndex = min(currentIndex + plotFramesPerSecond * secondsToPass, animationLength);
                    }
                    if (ImGui::Button(("Subtract " + to_string(secondsToPass) + " seconds").c_str()))
                    {
                        currentIndex = max(currentIndex - plotFramesPerSecond * secondsToPass, 0);
                    }
                }
                ImGui::End();
                break;
            }
            case Displaying::SettingsMenu:
            {
                ImGui::SetNextWindowSize(ImVec2(SETTINGS_WINDOW_WIDTH, SETTINGS_WINDOW_HEIGHT));
                ImGui::SetNextWindowPos(ImVec2(WINDOW_WIDTH / 2 - (SETTINGS_WINDOW_WIDTH / 2), WINDOW_HEIGHT / 2 - (SETTINGS_WINDOW_HEIGHT / 2)));
                if (ImGui::Begin("Settigns", nullptr))
                {
                    ImGui::SetWindowFontScale(fontSizeMultiplier);

                    ImGui::InputFloat("Font size", &fontSizeMultiplier, 0.1f, 0, "%.1f");

                    ImGui::ColorEdit4("Background color", backgroundColor);

                    if (ImGui::Button("Back"))
                    {
                        displaying = Displaying::WelcomingMenu;
                    }
                }
                ImGui::End();
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
    ImPlot::DestroyContext();

    // delete window and glfw
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}