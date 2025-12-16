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
    ImPlot::CreateContext();
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
    float distanceFromAim = 20.0f;
 
    bool hasTarget = true;
    bool windEnable = true;
    bool gravityEnable = true;
    bool atmosphereEnable = true;

    bool dataChanged = false;

    // variables for showing results
    vector<double> xAxis;
    vector<double> yAxis;
    string warning;

    // variables for animations in implot
    static int currentIndex = 0;
    static double lastTime = ImGui::GetTime();
    bool axesSetting = false;

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

    int chosenFile = 0;
    vector<string> fileNames;
    vector<const char*> fileNamesCStr;

    // render loop
    while (!glfwWindowShouldClose(window))
    {

        // rendering commands here
        glClearColor(0.07f, 0.13f, 0.17f, 1.0f); // background color
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
                        // get the possible filenames
                        fileNames = fileManager->getSavedSimulationsNames();

                        fileNamesCStr.clear();
                        for (const auto& file : fileNames) {
                            cout << file << endl;
                            fileNamesCStr.push_back(file.c_str());
                        }

                        displaying = Displaying::ReadFileMenu;
                    }
                }ImGui::End();
                break;
            }
            case Displaying::ReadFileMenu:
            {
                // Read file window:
                ImGui::SetNextWindowSize(ImVec2(WELCOME_WINDOW_WIDTH, WELCOME_WINDOW_HEIGHT));
                ImGui::SetNextWindowPos(ImVec2(WINDOW_WIDTH / 2 - WELCOME_WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2 - WELCOME_WINDOW_HEIGHT / 2));
                if (ImGui::Begin("Choose a simulation to read from", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse)) 
                {
                    ImGui::Text("Choose a simulation from the list: ");
                    
                    ImGui::Combo(" ", &chosenFile, fileNamesCStr.data(), fileNamesCStr.size());

                    if (ImGui::Button("Select"))
                    {
                        Simulation* sim = fileManager->readSimulationData(fileNamesCStr[chosenFile]);
                        ballVelocity = sim->ballVelocity;
                        firingAngle = sim->firingAngle;
                        ballRadius = sim->ballRadius;
                        ballMass = sim->ballMass;
                        initialDistanceFromGround = sim->initialDistanceFromGround;
                        distanceFromAim = sim->targetDistance;
                        windVelocity = sim->windVelocity;
                        windAngle = sim->windAngle;
                        gravitationalAcceleration = sim->gravitationalAcceleration;
                        atmosphericDensity = sim->atmosfericDensity;
                        xAxis = sim->xAxisCoordinates;
                        yAxis = sim->yAxisCoordinates;
                        warning = sim->warning;

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
                if (ImGui::Begin("Save simulation", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse)) {
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
                            Simulation* sim = new Simulation({ ballVelocity, firingAngle, ballRadius, ballMass, gravitationalAcceleration, windVelocity, windAngle, atmosphericDensity, initialDistanceFromGround, xAxis, yAxis, warning, {}, {}, hasTarget, distanceFromAim });
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
                if (ImGui::Begin("Create a new simulation", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse)) {

                    ImGui::Text("Enter values by adjusting sliders or by Ctrl+click to enter a specific number: ");

                    ImGui::SliderFloat("Initial ball velocity", &ballVelocity, 0.1f, 200.0f, "%.7f", ImGuiSliderFlags_AlwaysClamp);
                    ImGui::SliderFloat("Firing angle", &firingAngle, 0.0f, 90.0f, "%.7f", ImGuiSliderFlags_AlwaysClamp);
                    ImGui::SliderFloat("Ball radius", &ballRadius, 0.01f, 5.0f, "%.7f", ImGuiSliderFlags_AlwaysClamp);
                    ImGui::SliderFloat("Ball mass", &ballMass, 0.001f, 1000000.0f, "%.7f", ImGuiSliderFlags_AlwaysClamp);
                    if (ImGui::InputFloat("Initial distance from ground", &initialDistanceFromGround, 0.01f, 5.0f, "%.2f")) {
                        if (initialDistanceFromGround < 0.0f) initialDistanceFromGround = 0.0f;
                    }
                    ImGui::Checkbox("Enable target", &hasTarget);
                    if (!hasTarget) ImGui::BeginDisabled();
                    ImGui::InputFloat("Distance from the aim", &distanceFromAim, 0.1f, 1.0f, "%.2f");
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
                    ImGui::Checkbox("Enable gravity", &gravityEnable);
                    if (!gravityEnable && currentPlanet == 0) ImGui::BeginDisabled();
                    ImGui::SliderFloat("Gravitational acceleration", &gravitationalAcceleration, 0.0f, 24.0f, "%.7f", ImGuiSliderFlags_AlwaysClamp);
                    if (!gravityEnable || currentPlanet != 0) ImGui::EndDisabled();

                    if (atmosphericDensity == 0.0f) ImGui::BeginDisabled();
                    ImGui::Checkbox("Enable wind", &windEnable);
                    if (!windEnable) ImGui::BeginDisabled();
                    ImGui::SliderFloat("Wind Velocity", &windVelocity, 0.0f, 80.0f, "%.7f", ImGuiSliderFlags_AlwaysClamp);
                    ImGui::SliderFloat("Wind angle", &windAngle, 0.0f, 360.0f, "%.7f", ImGuiSliderFlags_AlwaysClamp);
                    if (!windEnable || atmosphericDensity == 0.0f) ImGui::EndDisabled();

                    if (currentPlanet != 0) ImGui::BeginDisabled();
                    ImGui::Checkbox("Enable atmosphere", &atmosphereEnable);
                    if (!atmosphereEnable && currentPlanet == 0) ImGui::BeginDisabled();
                    ImGui::SliderFloat("Atmosferic density", &atmosphericDensity, 0.0f, 65.0f, "%.7f", ImGuiSliderFlags_AlwaysClamp);
                    if (!atmosphereEnable || currentPlanet != 0) ImGui::EndDisabled();

                    // Spacing
                    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + ImGui::GetTextLineHeightWithSpacing());
                    
                    ImGui::PushTextWrapPos(0.0f);
                    ImGui::Text("You can also preset values according to specific space object (leave \"Custom\" if you want to adjust them for your own)");
                    ImGui::PopTextWrapPos();
                    ImGui::Combo("Select space object", &currentPlanet, planetNamesCStr.data(), planetNamesCStr.size());

                    if (ImGui::Button("Create simulation"))
                    {
                        if (!atmosphereEnable) atmosphericDensity = 0.0f;
                        if (!windEnable || atmosphericDensity == 0.0f) {
                            windVelocity = 0.0f;
                        }
                        if (!gravityEnable) gravitationalAcceleration = 0.0f;
                        if (!hasTarget) distanceFromAim = 0.0f;
                        calculator.CalculateData((double)ballVelocity, (double)firingAngle, (double)ballRadius, (double)ballMass, (double)gravitationalAcceleration, (double)windVelocity, (double)windAngle, (double)atmosphericDensity, (double)initialDistanceFromGround);
                        
                        xAxis = calculator.getXAxisCoordinates();
                        yAxis = calculator.getYAxisCoordinates();
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
                ImGui::SetNextWindowSize(ImGui::GetMainViewport()->Size);
                ImGui::SetNextWindowPos(ImGui::GetMainViewport()->Pos);
                if (ImGui::Begin("Simulation", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse)) {
                    ImGui::BeginChild("Trajectory", ImVec2(ImGui::GetMainViewport()->Size.x * 0.6, 0), true);

                    ImVec2 cursor = ImGui::GetCursorPos();

                    ImGui::SetCursorPos(ImVec2(cursor.x + 50, cursor.y + 50));

                    double now = ImGui::GetTime();
                    if (now - lastTime >= 0.001 && currentIndex < (int)xAxis.size()) {
                        lastTime = now;
                        currentIndex++;
                    }

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
                                double targetX = (double)distanceFromAim;
                                double targetY = 0.0;
                                ImPlot::PlotScatter("Target", &targetX, &targetY, 1);
                            }
                        }

                    }ImPlot::EndPlot();

                    ImGui::SetCursorPos(ImVec2(cursor.x + 50, cursor.y));

                    string warningMessage = "Warning: " + warning;
                    if(warningMessage != "Warning: ") ImGui::TextColored(ImVec4(1, 0, 0, 1), warningMessage.c_str());

                    cursor = ImGui::GetCursorPos();
                    ImGui::SetCursorPos(ImVec2(cursor.x + 50, cursor.y));

                    if (hasTarget && !dataChanged) {
                        if (abs(xAxis.back() - distanceFromAim) <= ballRadius) {
                            ImGui::TextColored(ImVec4(0, 1, 0, 1), "The ball hit the target");
                        }
                        else {
                            ImGui::TextColored(ImVec4(1, 0, 0, 1), "The ball missed the target");
                        }
                    }
                    ImGui::EndChild();

                    ImGui::SameLine();

                    ImGui::BeginChild("Data", ImVec2(0, 0), true);

                    ImGui::Text("Enter values by adjusting sliders or by Ctrl+click to enter a specific number: ");

                    if (ImGui::SliderFloat("Initial ball velocity", &ballVelocity, 0.1f, 200.0f, "%.7f", ImGuiSliderFlags_AlwaysClamp)) dataChanged = true;
                    if (ImGui::SliderFloat("Firing angle", &firingAngle, 0.0f, 90.0f, "%.7f", ImGuiSliderFlags_AlwaysClamp)) dataChanged = true;
                    if (ImGui::SliderFloat("Ball radius", &ballRadius, 0.01f, 5.0f, "%.7f", ImGuiSliderFlags_AlwaysClamp)) dataChanged = true;
                    if (ImGui::SliderFloat("Ball mass", &ballMass, 0.001f, 1000000.0f, "%.7f", ImGuiSliderFlags_AlwaysClamp)) dataChanged = true;
                    if (ImGui::InputFloat("Initial distance from ground", &initialDistanceFromGround, 0.01f, 5.0f, "%.2f")) {
                        if (initialDistanceFromGround < 0.0f) initialDistanceFromGround = 0.0f;
                        dataChanged = true;
                    }
                    if (ImGui::Checkbox("Enable target", &hasTarget)) dataChanged = true;
                    if (!hasTarget) ImGui::BeginDisabled();
                    if (ImGui::InputFloat("Distance from the aim", &distanceFromAim, 0.1f, 1.0f, "%.2f")) dataChanged = true;
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
                    if (!windEnable) ImGui::BeginDisabled();
                    if (ImGui::SliderFloat("Wind Velocity", &windVelocity, 0.0f, 80.0f, "%.7f", ImGuiSliderFlags_AlwaysClamp)) dataChanged = true;
                    if (ImGui::SliderFloat("Wind angle", &windAngle, 0.0f, 360.0f, "%.7f", ImGuiSliderFlags_AlwaysClamp)) dataChanged = true;
                    if (!windEnable || atmosphericDensity == 0.0f) ImGui::EndDisabled();

                    if (currentPlanet != 0) ImGui::BeginDisabled();
                    if (ImGui::Checkbox("Enable atmosphere", &atmosphereEnable)) dataChanged = true;
                    if (!atmosphereEnable && currentPlanet == 0) ImGui::BeginDisabled();
                    if (ImGui::SliderFloat("Atmosferic density", &atmosphericDensity, 0.0f, 65.0f, "%.7f", ImGuiSliderFlags_AlwaysClamp)) dataChanged = true;
                    if (!atmosphereEnable || currentPlanet != 0) ImGui::EndDisabled();

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
                        if (!hasTarget) distanceFromAim = 0.0f;
                        calculator.CalculateData((double)ballVelocity, (double)firingAngle, (double)ballRadius, (double)ballMass, (double)gravitationalAcceleration, (double)windVelocity, (double)windAngle, (double)atmosphericDensity, (double)initialDistanceFromGround);
                    
                        xAxis = calculator.getXAxisCoordinates();
                        yAxis = calculator.getYAxisCoordinates();
                        warning = calculator.getWarning();
                        dataChanged = false;
                        lastTime = ImGui::GetTime();
                        currentIndex = 0;

                        axesSetting = true;
                    }

                    ImGui::SameLine();
                    if (ImGui::Button("Save results")) {
                        if (dataChanged) {
                            calculator.CalculateData((double)ballVelocity, (double)firingAngle, (double)ballRadius, (double)ballMass, (double)gravitationalAcceleration, (double)windVelocity, (double)windAngle, (double)atmosphericDensity, (double)initialDistanceFromGround);

                            xAxis = calculator.getXAxisCoordinates();
                            yAxis = calculator.getYAxisCoordinates();
                            warning = calculator.getWarning();

                            dataChanged = false;
                        }
                        displaying = Displaying::SaveFileMenu;
                    }
                    if (ImGui::Button("Back"))
                    {
                        displaying = Displaying::WelcomingMenu;
                    }

                    ImGui::EndChild();
                }ImGui::End();
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