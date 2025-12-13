#pragma once
#include <vector>
#include <string>

struct Simulation{
    double ballVelocity;
    double firingAngle;
    double ballRadius;
    double ballMass;
    double gravitationalAcceleration;
    double windVelocity;
    double windAngle;
    double atmosfericDensity;
    double initialDistanceFromGround;
    std::vector<double> xAxisCoordinates;
    std::vector<double> yAxisCoordinates;
    std::string warning;
    std::vector<double> xAxisAdditionalTrajectory;
    std::vector<double> yAxisAdditionalTrajectory;
    bool hasTarget;
    double targetDistance;
};