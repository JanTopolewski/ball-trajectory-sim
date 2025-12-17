#pragma once
#include <vector>
#include <string>

struct Simulation{
    float ballVelocity;
    float firingAngle;
    float ballRadius;
    float ballMass;
    float gravitationalAcceleration;
    float windVelocity;
    float windAngle;
    float atmosfericDensity;
    float initialDistanceFromGround;
    std::vector<double> xAxisCoordinates;
    std::vector<double> yAxisCoordinates;
    std::string warning;
    std::vector<double> xAxisAdditionalTrajectory;
    std::vector<double> yAxisAdditionalTrajectory;
    bool hasTarget;
    float targetDistance;
};