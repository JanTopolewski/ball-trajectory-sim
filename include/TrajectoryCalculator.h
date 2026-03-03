#pragma once
#include <vector>
#include <functional>
#include <string>

using namespace std;

class TrajectoryCalculator {
	std::vector<double> xAxisCoordinates;
	std::vector<double> yAxisCoordinates;
	std::vector<double> zAxisCoordinates;
	std::string warning;

public:
	//Getters
	std::vector<double> getXAxisCoordinates() const;
	std::vector<double> getYAxisCoordinates() const;
	std::vector<double> getZAxisCoordinates() const;
	std::string getWarning() const;

	/**
	 * @brief
	 * This function manages calculations of ball's trajectory by using different methods(like RK4)
	 * depending on the particular case in order to make calculations as fast as possible. It saves results of calculations in
	 * xAxisCoordinates, yAxisCoordinates and zAxisCoordinates vectors and an additional information about problems with calculations(like a never
	 * ending case) to warning variable
	 */
	void CalculateData(
		double ballVelocity,
		double horizontalAngle,
		double verticalAngle,
		double ballRadius,
		double ballMass,
		double gravitationalAcceleration,
		double windVelocity,
		double windHorizontalAngle,
		double windVerticalAngle,
		double atmosphericDensity,
		double initialDistanceFromGround,
		int odeSolver
	);


	void RK4(
		function<void()>& calculatingFunc,
		double& vx,
		double& vy,
		double& vz,
		double& posX,
		double& posY,
		double& posZ,
		double& ax,
		double& ay,
		double& az,
		double& originalVX,
		double& originalVY,
		double& originalVZ,
		double(&kX)[4],
		double(&kY)[4],
		double(&kZ)[4],
		double(&kXVelocity)[4],
		double(&kYVelocity)[4],
		double(&kZVelocity)[4],
		double timeStep
	);


	void Euler(
		function<void()>& calculatingFunc,
		double& vx,
		double& vy,
		double& vz,
		double& posX,
		double& posY,
		double& posZ,
		double& ax,
		double& ay,
		double& az,
		double& originalVX,
		double& originalVY,
		double& originalVZ,
		double timeStep
	);


	void Verlet(
		function<void()>& calculatingFunc,
		double& vx,
		double& vy,
		double& vz,
		double& posX,
		double& posY,
		double& posZ,
		double& ax,
		double& ay,
		double& az,
		double& originalVX,
		double& originalVY,
		double& originalVZ,
		double timeStep
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
		double& ax,
		double& ay,
		double& az,
		double vx,
		double vy,
		double vz,
		double k,
		double ballMass
	);

	/**
	 * @brief
	 * These calculations are for air resistance and wind
	 */
	void CalculateAccelerations( // VI
		double& ax,
		double& ay,
		double& az,
		double vx,
		double vy,
		double vz,
		double k,
		double windVX,
		double windVY,
		double windVZ,
		double ballMass
	);

	/**
	 * @brief
	 * These calculations are for air resistance and gravity
	 */
	void CalculateAccelerations( // VII
		double& ax,
		double& ay,
		double& az,
		double vx,
		double vy,
		double vz,
		double k,
		double gravitationalAcceleration,
		double ballMass
	);

	/**
	 * @brief
	 * These calculations are for air resistance, wind and gravity
	 */
	void CalculateAccelerations( // VIII
		double& ax,
		double& ay,
		double& az,
		double vx,
		double vy,
		double vz,
		double k,
		double gravitationalAcceleration,
		double windVX,
		double windVY,
		double windVZ,
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
		double& kZ,
		double& kXVelocity,
		double& kYVelocity,
		double& kZVelocity,
		double timeStep,
		double vx,
		double vy,
		double vz,
		double& ax,
		double& ay,
		double& az
	);
};