#pragma once
#include <vector>
#include <functional>
#include <string>

using namespace std;

class TrajectoryCalculator {
	std::vector<double> xAxisCoordinates;
	std::vector<double> yAxisCoordinates;
	std::string warning;

public:
	//Getters
	std::vector<double> getXAxisCoordinates() const;
	std::vector<double> getYAxisCoordinates() const;
	std::string getWarning() const;
	

	void CalculateData(
		double ballVelocity,
		double firingAngle,
		double ballRadius,
		double ballMass,
		double gravitationalAcceleration,
		double windVelocity,
		double windAngle,
		double atmosphericDensity,
		double initialDistanceFromGround
	);

	void CalculateAccelerations( // VIII
		double& horizontalAcceleration,
		double& verticalAcceleration,
		double horizontalBallVelocity,
		double verticalBallVelocity,
		double k,
		double gravitationalAcceleration,
		double horizontalWindVelocity,
		double verticalWindVelocity,
		double ballMass
	);

	void CalculateAccelerations( // V
		double& horizontalAcceleration,
		double& verticalAcceleration,
		double horizontalBallVelocity,
		double verticalBallVelocity,
		double k,
		double ballMass
	);

	void CalculateAccelerations( // VII
		double& horizontalAcceleration,
		double& verticalAcceleration,
		double horizontalBallVelocity,
		double verticalBallVelocity,
		double k,
		double gravitationalAcceleration,
		double ballMass
	);

	void CalculateAccelerations( // VI
		double& horizontalAcceleration,
		double& verticalAcceleration,
		double horizontalBallVelocity,
		double verticalBallVelocity,
		double k,
		double horizontalWindVelocity,
		double verticalWindVelocity,
		double ballMass
	);

	void RungeKuttaMethodPart(
		function<void()> calculatingFunc,
		double& kX,
		double& kY,
		double& kHorizontalVelocity,
		double& kVerticalVelocity,
		double timeStep,
		double horizontalBallVelocity,
		double verticalBallVelocity,
		double& horizontalAcceleration,
		double& verticalAcceleration
	);
};