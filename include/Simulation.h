#pragma once
#include <vector>

struct Simulation{
    double ballVelocity;
    double firingAngle;
    double ballRadius;
    double ballMass;
    double gravitationalAcceleration;
    double windVelocity;
    double windAngle;
    double atmosfericDensity;
    std::vector<double> xAxisCoordinates;
    std::vector<double> yAxisCoordinates;
};