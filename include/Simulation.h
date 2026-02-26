#pragma once
#include <vector>
#include <string>

struct Simulation{
    float ballVelocity;
    float horizontalAngle;
    float verticalAngle;
    float ballRadius;
    float ballMass;
    float gravitationalAcceleration;
    float windVelocity;
    float windHorizontalAngle;
    float windVerticalAngle;
    float atmosfericDensity;
    float initialDistanceFromGround;
    std::vector<double> xAxisCoordinates;
    std::vector<double> yAxisCoordinates;
    std::vector<double> zAxisCoordinates;
    std::string warning;
    std::vector<double> xAxisAdditionalTrajectory;
    std::vector<double> yAxisAdditionalTrajectory;
    std::vector<double> zAxisAdditionalTrajectory;
    bool hasTarget;
    float targetDistance;
    float targetHorizontalAngle;
};