#pragma once
#include <vector>
#include <cmath>

using namespace std;

class TrajectoryCalculator {
	std::vector<double> xAxisCoordinates;
	std::vector<double> yAxisCoordinates;


public:
	TrajectoryCalculator(const TrajectoryCalculator& originalCalculator);

	//Getters
	std::vector<double> getXAxisCoordinates() const;
	std::vector<double> getYAxisCoordinates() const;
	

	void CalculateData(
		double BallVelocity,
		double firingAngle,
		double ballRadius,
		double ballMass,
		double gravitationalAcceleration,
		double WindVelocity,
		double windAngle,
		double atmosphericDensity
	);

	void RungeKuttaMethod();
	void CalculateDataForPoint()
};