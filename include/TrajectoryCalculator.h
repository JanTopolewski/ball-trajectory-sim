#pragma once
#include <vector>

using namespace std;

class TrajectoryCalculator {
	std::vector<double> xAxisCoordinates;
	std::vector<double> yAxisCoordinates;

public:
	//Getters
	std::vector<double> getXAxisCoordinates() const;
	std::vector<double> getYAxisCoordinates() const;
	

	void CalculateData(
		double ballVelocity,
		double firingAngle,
		double ballRadius,
		double ballMass,
		double gravitationalAcceleration,
		double windVelocity,
		double windAngle,
		double atmosphericDensity
	);

	void CalculateAccelerations(double& horizontalAcceleration, double& verticalAcceleration, double gravitationalAcceleration);
	void CalculateAccelerations(
		double& horizontalAcceleration,
		double& verticalAcceleration,
		double horizontalBallVelocity,
		double verticalBallvelocity,
		double k,
		double gravitationalAcceleration,
		double horizontalWindVelocity,
		double verticalWindVelocity,
		double ballMass
	);
};