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
	
	/**
	 * @brief
	 * This function manages calculations of ball's trajectory by using different methods(like RK4)
	 * depending on the particular case in order to make calculations as fast as possible. It saves results of calculations in
	 * xAxisCoordinates and yAxisCoordinates vectors and an additional information about problems with calculations(like a never
	 * ending case) to warning variable
	 */
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

	/**
	 * @brief
	 * These are the calculations for only the air resistance only (atmosfericDensity)
	 *
	 *
	 * This is from the 5th section from the specification
	 *
	 * Little correction: in the spec there is -g in vertical acceleration formula, but it should not be there
	 *
	 * The code here is correct
	 */
	void CalculateAccelerations( // V
		double& horizontalAcceleration,
		double& verticalAcceleration,
		double horizontalBallVelocity,
		double verticalBallVelocity,
		double k,
		double ballMass
	);

	/**
	 * @brief
	 * These calculations are for air resistance and wind
	 */
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

	/**
	 * @brief
	 * These calculations are for air resistance and gravity
	 */
	void CalculateAccelerations( // VII
		double& horizontalAcceleration,
		double& verticalAcceleration,
		double horizontalBallVelocity,
		double verticalBallVelocity,
		double k,
		double gravitationalAcceleration,
		double ballMass
	);

	/**
	 * @brief
	 * These calculations are for air resistance, wind and gravity
	 */
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

	/**
	 * @brief
	 * These calculations are for calculating position and velocity changes in each step of RK4
	 */
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