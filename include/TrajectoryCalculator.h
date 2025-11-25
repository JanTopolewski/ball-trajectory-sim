#pragma once
#include <vector>
#include <functional>

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

	void CalculateAccelerations(//t� funkcje przeci��amy do sytuacji z oporem powietrza // VIII
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

	void CalculateAccelerations(//t� funkcje przeci��amy do sytuacji z oporem powietrza // VI
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