#pragma once
#include <vector>

using namespace std;

class TrajectoryCalculator {
	std::vector<double> xAxisCoordinates;
	std::vector<double> yAxisCoordinates;
	double tempHorizontalBallVelocity;
	double tempVerticalBallVelocity;
	double horizontalAcceleration;
	double verticalAcceleration;

public:
	TrajectoryCalculator();
	TrajectoryCalculator(const TrajectoryCalculator& originalCalculator);

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

	void CalculateAccelerations(double gravitationalAcceleration);
	void CalculateAccelerations(
		double k,
		double gravitationalAcceleration,
		double horizontalWindVelocity,
		double verticalWindVelocity,
		double ballMass
	);
};