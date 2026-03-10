#include <iostream>
#include <vector>
#include <string>
#include <filesystem>
#include <regex>
#include <algorithm>
#include <cmath>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
// #include "imgui_internal.h"

#include "implot3d.h"
// #include "implot3d_internal.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../include/TrajectoryCalculator.h"
#include "../include/FilesManager.h"
#include "../include/Simulation.h"
#include "../include/Displaying.h"
#include "../include/SpaceObject.h"
#include "../include/BallMath.h"
#include "../include/EBO.h"
#include "../include/shaderClass.h"
#include "../include/VAO.h"
#include "../include/VBO.h"
#include "../include/Camera.h"

using namespace std;

// // Vertices coordinates
// GLfloat vertices[] =
// { //     COORDINATES     /        COLORS      /
// 	-0.5f, 0.0f,  0.5f,     0.83f, 0.70f, 0.44f,
// 	-0.5f, 0.0f, -0.5f,     0.83f, 0.70f, 0.44f,
// 	 0.5f, 0.0f, -0.5f,     0.83f, 0.70f, 0.44f,
// 	 0.5f, 0.0f,  0.5f,     0.83f, 0.70f, 0.44f,
// 	 0.0f, 0.8f,  0.0f,     0.92f, 0.86f, 0.76f,
// };
//
// // Indices for vertices order
// GLuint indices[] =
// {
// 	0, 1, 2,
// 	0, 2, 3,
// 	0, 1, 4,
// 	1, 2, 4,
// 	2, 3, 4,
// 	3, 0, 4
// };

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
    ImPlot3D::CreateContext();
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
	double accumulatedTime = 0.0;

    // reading planet data from file
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
    float backgroundColor[4] = { 0.07f, 0.13f, 0.17f, 1.0f };

    // Trajectory
    std::vector<GLfloat> trajectoryVertices;
    float trajectoryColor[4] = { 1.0f, 0.0f, 0.0f, 1.0f };
	VAO vao_trajectory;
	VBO vbo_trajectory;

    // Creating a sphere
    int longitude_points = 72;
    int latitude_points = 36;
    float sphere_radius = 1.0;

    // 2 poles + rings of vertices
    int vertices_amount = (2 + longitude_points * (latitude_points - 1)) * 3;
    // Top fan + middle quads + bottom fan
    int indices_amount = (longitude_points * 3 * 2 + (latitude_points - 2) * longitude_points * 6);

    std::vector<GLfloat> vertices(vertices_amount);
    std::vector<GLuint> indices(indices_amount);

    // North pole
    vertices[0] = 0;
    vertices[1] = static_cast<GLfloat>(sphere_radius);
    vertices[2] = 0;

    // South pole
    int south_pole_idx = vertices_amount / 3 - 1;
    vertices[vertices_amount - 3] = 0;
    vertices[vertices_amount - 2] = static_cast<GLfloat>(-sphere_radius);
    vertices[vertices_amount - 1] = 0;

    int vert_idx = 3;
    int ind_idx = 0;
    int point_idx = 1;

    // Generate latitude rings (excluding poles)
    for (int i = 1; i < latitude_points; i++) {
        int latitude = i * 5;

        for (int j = 0; j < longitude_points; j++) {
            int longitude = j * 5;

            std::vector<double> coords = ballMath::get_coords(sphere_radius, latitude, longitude);
            vertices[vert_idx++] = static_cast<GLfloat>(coords[1]);
            vertices[vert_idx++] = static_cast<GLfloat>(coords[2]);
            vertices[vert_idx++] = static_cast<GLfloat>(coords[0]);

            // Top cap triangles
            if (i == 1) {
                indices[ind_idx++] = 0;
                indices[ind_idx++] = point_idx + j;
                indices[ind_idx++] = point_idx + (j + 1) % longitude_points;
            }

            // Middle quads
            if (i < latitude_points - 1) {
                int curr = point_idx + j;
                int next = point_idx + (j + 1) % longitude_points;
                int below_curr = curr + longitude_points;
                int below_next = next + longitude_points;

                indices[ind_idx++] = curr;
                indices[ind_idx++] = below_curr;
                indices[ind_idx++] = below_next;

                indices[ind_idx++] = curr;
                indices[ind_idx++] = below_next;
                indices[ind_idx++] = next;
            }

            // Bottom cap triangles
            if (i == latitude_points - 1) {
                indices[ind_idx++] = south_pole_idx;
                indices[ind_idx++] = point_idx + (j + 1) % longitude_points;
                indices[ind_idx++] = point_idx + j;
            }
        }
        point_idx += longitude_points;
    }

    // rendering sphere
    float rendered_sphere_color[4] = { 1.0f, 0.0f, 0.0f, 1.0f };


    // terrain
    GLfloat terrain_vertices[24]
    {   // vertices             // colors
        -1.0f, 0.0f, -1.0f,     1.0f, 1.0f, 0.0f,
        -1.0f, 0.0f,  1.0f,     0.0f, 0.8f, 1.0f,
        1.0f,  0.0f, -1.0f,     0.1f, 0.9f, 0.0f,
        1.0f,  0.0f,  1.0f,     0.2f, 1.0f, 0.0f
    };
    GLuint terrain_indices[6] = {
        0, 1, 2,
        1, 2, 3
    };

    float terrain_color[4] = { 0.0f, 1.0f, 0.0f, 1.0f };


    // Creates shader object using shaders default.vert and default.frag
    Shader shaderProgram((string(PROJECT_ROOT_DIR) + "/Shaders/default.vert").c_str(), (string(PROJECT_ROOT_DIR) + "/Shaders/default.frag").c_str());
    Shader terrainShader((string(PROJECT_ROOT_DIR) + "/Shaders/terrain.vert").c_str(), (string(PROJECT_ROOT_DIR) + "/Shaders/terrain.frag").c_str());

    // Generates Vertex Array Object and binds it
    VAO vao_sphere, vao_terrain;
    vao_sphere.Bind();

    // Generate Vertex Buffer Object and Element Buffer Object linking the vertices and indices
    VBO vbo1(vertices.data(), vertices.size() * sizeof(GLfloat));
    EBO ebo1(indices.data(), indices.size() * sizeof(GLuint));

    // links VBO attributes such as coordinates and colors to VAO
    vao_sphere.LinkAttrib(vbo1, 0, 3, GL_FLOAT, 3 * sizeof(float), (void*)0);
    // vao_sphere.LinkAttrib(vbo1, 1, 3, GL_FLOAT, 8 * sizeof(float), reinterpret_cast<void *>(3 * sizeof(float)));
    // vao_sphere.LinkAttrib(vbo1, 2, 2, GL_FLOAT, 8 * sizeof(float), reinterpret_cast<void *>(6 * sizeof(float)));
    // unbind all to prevent accidentally modifying the buffers
    vao_sphere.Unbind();
    vbo1.Unbind();
    ebo1.Unbind();

    vao_terrain.Bind();
    VBO vbo_terrain(terrain_vertices, sizeof(terrain_vertices));
    EBO ebo_terrain(terrain_indices, sizeof(terrain_indices));

    vao_terrain.LinkAttrib(vbo_terrain, 0, 3, GL_FLOAT, 6 * sizeof(float), nullptr);
    vao_terrain.LinkAttrib(vbo_terrain, 1, 3, GL_FLOAT, 6 * sizeof(float), reinterpret_cast<void*>(3 * sizeof(float)));
    vao_terrain.Unbind();
    vbo_terrain.Unbind();
    ebo_terrain.Unbind();

    vbo_trajectory = VBO(nullptr, 500000 * 3 * sizeof(GLfloat), GL_DYNAMIC_DRAW);
    vao_trajectory.Bind();
    vbo_trajectory.Bind();
	vao_trajectory.LinkAttrib(vbo_trajectory, 0, 3, GL_FLOAT, 3 * sizeof(float), (void*)0);
    vao_trajectory.Unbind();
    vbo_trajectory.Unbind();

    // apply color to sphere
    shaderProgram.Activate();
    glUniform1f(glGetUniformLocation(shaderProgram.ID, "size"), sphere_radius);
    glUniform4f(glGetUniformLocation(shaderProgram.ID, "color"), rendered_sphere_color[0], rendered_sphere_color[1], rendered_sphere_color[2], rendered_sphere_color[3]);

    // Create a model matrix to position the sphere
    glm::mat4 model = glm::mat4(1.0f);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));

    terrainShader.Activate();
    glUniform1f(glGetUniformLocation(terrainShader.ID, "size"), 10.0f);
    // glUniform4f(glGetUniformLocation(terrainShader.ID, "color"), terrain_color[0], terrain_color[1], terrain_color[2], terrain_color[3]);

    glm::mat4 terrainModel = glm::mat4(1.0f);
    glUniformMatrix4fv(glGetUniformLocation(terrainShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(terrainModel));

    // Lines for the axis
    GLfloat yAxisVertices[6] = {
        0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f
    };

    GLfloat xAxisVertices[6] = {
        0.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f
    };

    GLfloat zAxisVertices[6] = {
        0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f
    };

    Shader lineShader((string(PROJECT_ROOT_DIR) + "/Shaders/default.vert").c_str(), (string(PROJECT_ROOT_DIR) + "/Shaders/default.frag").c_str());
    VAO vao_yAxis, vao_xAxis, vao_zAxis;

    VBO vbo_yAxis(yAxisVertices, sizeof(yAxisVertices));
    VBO vbo_xAxis(xAxisVertices, sizeof(xAxisVertices));
    VBO vbo_zAxis(zAxisVertices, sizeof(zAxisVertices));

    vao_yAxis.Bind();
    vbo_yAxis.Bind();
    vao_yAxis.LinkAttrib(vbo_yAxis, 0, 3, GL_FLOAT, 3 * sizeof(float), nullptr);
    vao_yAxis.Unbind();
    vbo_yAxis.Unbind();

    vao_xAxis.Bind();
    vbo_xAxis.Bind();
    vao_xAxis.LinkAttrib(vbo_xAxis, 0, 3, GL_FLOAT, 3 * sizeof(float), nullptr);
    vao_xAxis.Unbind();
    vbo_xAxis.Unbind();

    vao_zAxis.Bind();
    vbo_zAxis.Bind();
    vao_zAxis.LinkAttrib(vbo_zAxis, 0, 3, GL_FLOAT, 3 * sizeof(float), nullptr);
    vao_zAxis.Unbind();
    vbo_zAxis.Unbind();




    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Also clear depth buffer in render loop

    Camera camera(WINDOW_WIDTH, WINDOW_HEIGHT, glm::vec3(0.0f, 2.0f, 10.0f));

    // render loop
    while (!glfwWindowShouldClose(window))
    {
        // rendering commands here
        glClearColor(backgroundColor[0], backgroundColor[1], backgroundColor[2], backgroundColor[3]); // background color
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clean the back buffer and assign the new color to it

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();




        if (!io.WantCaptureMouse)
        {
            camera.Inputs(window);
        }

        shaderProgram.Activate();
        camera.Matrix(45.0f, 0.1f, 1000.0f, shaderProgram, "camMatrix");
        terrainShader.Activate();
        camera.Matrix(45.0f, 0.1f, 1000.0f, terrainShader, "camMatrix");

        // tell opengl, that we want to use our shader program
        shaderProgram.Activate();

        // Draw axes with size 10.0f
        glUniform1f(glGetUniformLocation(shaderProgram.ID, "size"), 10.0f);

        // Y axis (green)
        glUniform4f(glGetUniformLocation(shaderProgram.ID, "color"), 0.0f, 1.0f, 0.0f, 1.0f);
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram.ID, "model"), 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));
        vao_yAxis.Bind();
        glDrawArrays(GL_LINES, 0, 2);
        vao_yAxis.Unbind();

        // X axis (red)
        glUniform4f(glGetUniformLocation(shaderProgram.ID, "color"), 1.0f, 0.0f, 0.0f, 1.0f);
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram.ID, "model"), 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));
        vao_xAxis.Bind();
        glDrawArrays(GL_LINES, 0, 2);
        vao_xAxis.Unbind();

        // Z axis (blue)
        glUniform4f(glGetUniformLocation(shaderProgram.ID, "color"), 0.0f, 0.0f, 1.0f, 1.0f);
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram.ID, "model"), 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));
        vao_zAxis.Bind();
        glDrawArrays(GL_LINES, 0, 2);
        vao_zAxis.Unbind();

        double now = ImGui::GetTime();
        bool animationFinished = currentIndex >= (int)xAxis.size();
        frameDelta = now - lastTime;
        lastTime = now;

        if (!isPaused)
        {
            double step = 1.0 / (double)plotFramesPerSecond / (double)plotSpeedMultiplier;
            accumulatedTime += frameDelta;

			int steps = (int)(accumulatedTime / step);
            if (steps > 0 && !animationFinished && !isPaused)
            {
                currentIndex = min(currentIndex + steps, (int)xAxis.size());
                accumulatedTime -= steps * step;
            }

            if (animationFinished) 
            {
                accumulatedTime = 0.0;
            }
        }
        else {
			accumulatedTime = 0.0;
        }

        glUniform1f(glGetUniformLocation(shaderProgram.ID, "size"), 1.0f);

        if (!xAxis.empty() && !yAxis.empty() && !zAxis.empty())
        {
			int trailCount = clamp(currentIndex, 0, (int)xAxis.size());
            if (trailCount > 0) 
            {
				trajectoryVertices.resize(static_cast<size_t>(trailCount) * 3);
                for (int i = 0; i < trailCount; ++i) 
                {
					trajectoryVertices[3 * i] = static_cast<GLfloat>(xAxis[i]);
					trajectoryVertices[3 * i + 1] = static_cast<GLfloat>(yAxis[i]);
					trajectoryVertices[3 * i + 2] = static_cast<GLfloat>(zAxis[i]);
                }

				vbo_trajectory.Bind();
				glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(500000 * 3 * sizeof(GLfloat)), nullptr, GL_DYNAMIC_DRAW);
				glBufferSubData(GL_ARRAY_BUFFER, 0, static_cast<GLsizeiptr>(trajectoryVertices.size() * sizeof(GLfloat)), trajectoryVertices.data());
				vbo_trajectory.Unbind();

                glUniform4f(glGetUniformLocation(shaderProgram.ID, "color"), trajectoryColor[0], trajectoryColor[1], trajectoryColor[2], trajectoryColor[3]);
				glUniformMatrix4fv(glGetUniformLocation(shaderProgram.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));

				glBindVertexArray(vao_trajectory.ID);
				glDrawArrays(GL_LINE_STRIP, 0, trailCount);
				glBindVertexArray(0);
            }
		}

        int sampleIdx = 0;
        if (xAxis.size() > 0)
        {
            sampleIdx = clamp(currentIndex - 1, 0, (int)xAxis.size() - 1);
            glUniformMatrix4fv(glGetUniformLocation(shaderProgram.ID, "model"), 1, GL_FALSE, glm::value_ptr(glm::translate(model, glm::vec3((float)xAxis[sampleIdx], (float)yAxis[sampleIdx], (float)zAxis[sampleIdx]))));
        }

        shaderProgram.Activate();
        glUniform1f(glGetUniformLocation(shaderProgram.ID, "size"), sphere_radius);
        glUniform4f(glGetUniformLocation(shaderProgram.ID, "color"), rendered_sphere_color[0], rendered_sphere_color[1], rendered_sphere_color[2], rendered_sphere_color[3]);

        vao_sphere.Bind(); // bind the VAO so OpenGl knows to use it
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
        vao_sphere.Unbind();

        terrainShader.Activate();
        glm::mat4 terrainModel = glm::mat4(1.0f);
        glUniformMatrix4fv(glGetUniformLocation(terrainShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(terrainModel));

        vao_terrain.Bind();
        glDrawElements(GL_TRIANGLES, sizeof(terrain_indices) / sizeof(int), GL_UNSIGNED_INT, 0);
        vao_terrain.Unbind();

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

					terrainShader.Activate();
                    glUniform1f(glGetUniformLocation(terrainShader.ID, "size"), std::max(
                        static_cast<float>(std::abs(*std::max_element(xAxis.begin(), xAxis.end(), [](double a, double b) { return std::abs(a) < std::abs(b); }))),
                        static_cast<float>(std::abs(*std::max_element(zAxis.begin(), zAxis.end(), [](double a, double b) { return std::abs(a) < std::abs(b); })))
                    ) + 5.0f);

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
                ImGui::SliderFloat("Ball mass", &ballMass, 0.001f, 100.0f, "%.7f", ImGuiSliderFlags_AlwaysClamp);
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

                    terrainShader.Activate();
                    glUniform1f(glGetUniformLocation(terrainShader.ID, "size"), std::max(
                        static_cast<float>(std::abs(*std::max_element(xAxis.begin(), xAxis.end(), [](double a, double b) { return std::abs(a) < std::abs(b); }))),
                        static_cast<float>(std::abs(*std::max_element(zAxis.begin(), zAxis.end(), [](double a, double b) { return std::abs(a) < std::abs(b); })))
                    ) + 5.0f);

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
            ImGui::SetNextWindowSize(ImVec2(WINDOW_WIDTH * 2 / 3, WINDOW_HEIGHT - 20));
            ImGui::SetNextWindowPos(ImVec2(ImGui::GetMainViewport()->Pos.x + 10, ImGui::GetMainViewport()->Pos.y + 10), ImGuiCond_FirstUseEver);
            if (ImGui::Begin("Trajectory"))
            {
                ImVec2 cursor = ImGui::GetCursorPos();

                ImGui::SetCursorPos(ImVec2(cursor.x + 50, cursor.y + 50));

                auto [xMinTemp, xMaxTemp] = minmax_element(xAxis.begin(), xAxis.end());
                auto [yMinTemp, yMaxTemp] = minmax_element(yAxis.begin(), yAxis.end());
                auto [zMinTemp, zMaxTemp] = minmax_element(zAxis.begin(), zAxis.end());
                double xMin = *xMinTemp;
                double xMax = *xMaxTemp;
                double yMin = *yMinTemp;
                double yMax = *yMaxTemp;
                double zMin = *zMinTemp;
                double zMax = *zMaxTemp;

				double maxRange = max({ xMax - xMin, yMax - yMin, zMax - zMin });

                double xMargin = maxRange - (xMax - xMin) * 1.1 + 2.0;
                double yMargin = maxRange - (yMax - yMin) * 1.1 + 2.0;
                double zMargin = maxRange - (zMax - zMin) * 1.1 + 2.0;

                if (ImPlot3D::BeginPlot("Space", ImVec2(-1, -1), ImPlot3DFlags_Equal | ImPlot3DFlags_NoTitle | ImPlot3DFlags_NoLegend)) {
					ImPlot3D::SetupAxes("X", "Y", "Z");
                    if (currentIndex > 1) {
                        if (axesSetting) {
                            ImPlot3D::SetupAxesLimits(zMin - zMargin, zMax + zMargin, xMin - xMargin, xMax + xMargin, yMin - yMargin, yMax + yMargin, ImPlot3DCond_Always);
                            axesSetting = false;
                        }
                        else {
                            ImPlot3D::SetupAxesLimits(zMin - zMargin, zMax + zMargin, xMin - xMargin, xMax + xMargin, yMin - yMargin, yMax + yMargin, ImPlot3DCond_Once);
                        }

                        ImPlot3D::PushStyleColor(ImPlot3DCol_Line, ImVec4(1, 0, 0, 1));
                        ImPlot3D::PlotLine("Trajectory", zAxis.data(), xAxis.data(), yAxis.data(), currentIndex);

                        ImPlot3D::PopStyleColor();

                        ImPlot3D::PushStyleColor(ImPlot3DCol_MarkerFill, ImVec4(1, 0, 0, 1));
                        ImPlot3D::PlotScatter("Ball", &zAxis[currentIndex - 1], &xAxis[currentIndex - 1], &yAxis[currentIndex - 1], 1);

                        ImPlot3D::PopStyleColor();

                        if (hasTarget && !dataChanged) {
                            ImPlot3D::PushStyleColor(ImPlot3DCol_MarkerFill, ImVec4(1, 1, 0, 1));
                            double targetX = (double)targetXDistance;
                            double targetY = 0.0;
							double targetZ = (double)targetZDistance;
                            ImPlot3D::PlotScatter("Target", &targetZ, &targetX, &targetY, 1);
                            ImPlot3D::PopStyleColor();
                        }

                    }
                    ImPlot3D::EndPlot();
                }

                ImGui::SetCursorPos(ImVec2(cursor.x + 50, cursor.y));

                string warningMessage = "Warning: " + warning;
                if (warningMessage != "Warning: ") ImGui::TextColored(ImVec4(1, 0, 0, 1), warningMessage.c_str());

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

            ImGui::SetNextWindowSize(ImVec2((WINDOW_WIDTH - 30) * 1 / 3, (WINDOW_HEIGHT - 30) * 3 / 4), ImGuiCond_FirstUseEver);
            ImGui::SetNextWindowPos(ImVec2(WINDOW_WIDTH * 2 / 3 + 20, 10), ImGuiCond_FirstUseEver);
            if (ImGui::Begin("Data", nullptr, ImGuiWindowFlags_HorizontalScrollbar))
            {
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

                    terrainShader.Activate();
                    glUniform1f(glGetUniformLocation(terrainShader.ID, "size"), std::max(
                        static_cast<float>(std::abs(*std::max_element(xAxis.begin(), xAxis.end(), [](double a, double b) { return std::abs(a) < std::abs(b); }))),
                        static_cast<float>(std::abs(*std::max_element(zAxis.begin(), zAxis.end(), [](double a, double b) { return std::abs(a) < std::abs(b); })))
                    ) + 5.0f);
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

            ImGui::SetNextWindowSize(ImVec2((WINDOW_WIDTH - 30) * 1 / 3, (WINDOW_HEIGHT - 30) * 1 / 4), ImGuiCond_FirstUseEver);
            ImGui::SetNextWindowPos(ImVec2(WINDOW_WIDTH * 2 / 3 + 20, WINDOW_HEIGHT * 3 / 4 + 20), ImGuiCond_FirstUseEver);
            if (ImGui::Begin("Simulation control"))
            {
                int animationLength = static_cast<int>(xAxis.size());

                // speed multiplier
                ImGui::InputFloat("Speed of the animation", &plotSpeedMultiplier, 0.25f, 1.0f, "%.2f");

                // slider with animation
                ImGui::SliderInt(" ", &currentIndex, 0, animationLength, "%d", ImGuiSliderFlags_AlwaysClamp);

                ImGui::Spacing();

                // start
                if (ImGui::Button("Start"))
                {
                    currentIndex = 0;
                }

                ImGui::SameLine();

                if (ImGui::Button("Previous frame"))
                {
                    currentIndex = max(currentIndex - 1, 0);
                }

                ImGui::SameLine();

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

                ImGui::SameLine();

                if (ImGui::Button("Next frame"))
                {
                    currentIndex = min(currentIndex + 1, animationLength);
                }

                ImGui::SameLine();

                // end
                if (ImGui::Button("End"))
                {
                    currentIndex = animationLength;
                }

                ImGui::Spacing();

                // +/- frames, seconds etc.
                if (ImGui::Button(("Subtract " + to_string(secondsToPass) + " seconds").c_str()))
                {
                    currentIndex = max(currentIndex - plotFramesPerSecond * secondsToPass, 0);
                }
                ImGui::SameLine();
                if (ImGui::Button("Subtract second"))
                {
                    currentIndex = max(currentIndex - plotFramesPerSecond, 0);
                }
                ImGui::SameLine();
                if (ImGui::Button("Add second"))
                {
                    currentIndex = min(currentIndex + plotFramesPerSecond, animationLength);
                }
                ImGui::SameLine();
                if (ImGui::Button(("Add " + to_string(secondsToPass) + " seconds").c_str()))
                {
                    currentIndex = min(currentIndex + plotFramesPerSecond * secondsToPass, animationLength);
                }


                ImGui::InputInt("Seconds", &secondsToPass);


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

                ImGui::SliderFloat("Ball size", &sphere_radius, 0.1f, 10.0f);

                ImGui::ColorEdit4("Sphere color", rendered_sphere_color);

                if (ImGui::Button("Back"))
                {
                    displaying = Displaying::WelcomingMenu;
                }
            }
            ImGui::End();
            break;
        }
        }

        // shaderProgram.Activate();
        // glUniform1f(glGetUniformLocation(shaderProgram.ID, "size"), sphere_radius);
        // glUniform4f(glGetUniformLocation(shaderProgram.ID, "color"), rendered_sphere_color[0], rendered_sphere_color[1], rendered_sphere_color[2], rendered_sphere_color[3]);

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
    ImPlot3D::DestroyContext();

    // remove all objects
    vao_sphere.Delete();
    vbo1.Delete();
    ebo1.Delete();
    shaderProgram.Delete();
    vao_terrain.Delete();
    vbo_terrain.Delete();
    ebo_terrain.Delete();
    terrainShader.Delete();
    vbo_trajectory.Delete();
    vao_trajectory.Delete();
    vao_yAxis.Delete();
    vao_xAxis.Delete();
    vao_zAxis.Delete();

    // delete window and glfw
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}