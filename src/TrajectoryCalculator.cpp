#include "../include/TrajectoryCalculator.h"
#include <vector>
#include <cmath>
#include <numbers>
#include <functional>
#include <iostream>
#include <iomanip>

using namespace std;

std::vector<double> TrajectoryCalculator::getXAxisCoordinates() const { return xAxisCoordinates; }
std::vector<double> TrajectoryCalculator::getYAxisCoordinates() const { return yAxisCoordinates; }
string TrajectoryCalculator::getWarning() const { return warning; }


void TrajectoryCalculator::CalculateData(
	double ballVelocity,
	double firingAngle,
	double ballRadius,
	double ballMass,
	double gravitationalAcceleration,
	double windVelocity,
	double windAngle,
	double atmosphericDensity,
	double initialDistanceFromGround
) {
	// resetting vectors and warning message
	vector<double>().swap(xAxisCoordinates);
	vector<double>().swap(yAxisCoordinates);
	warning = "";

	// starting position
	xAxisCoordinates.push_back(0.0);
	yAxisCoordinates.push_back(0.0);

	// converting angle to radians and splitting velocity to horizontal and vertical
	double angleInRadians = firingAngle * numbers::pi / 180.0;
	double horizontalBallVelocity = ballVelocity * cos(angleInRadians);
	double verticalBallVelocity = ballVelocity * sin(angleInRadians);

	double timeStep = 0.001, horizontalAcceleration = 0.0, verticalAcceleration = 0.0, k, x = 0.0, y = ballRadius + initialDistanceFromGround;

	function<void()> calculatingFunc;

	if (atmosphericDensity != 0.0) {
		// air resistance
		k = 0.5 * atmosphericDensity * 0.47 * numbers::pi * ballRadius * ballRadius;

		if (windVelocity != 0.0) {
			// calculating wind velocity and angle
			double windAngleInRadians = windAngle * numbers::pi / 180.0;
			double horizontalWindVelocity = windVelocity * cos(windAngleInRadians);
			double verticalWindVelocity = windVelocity * sin(windAngleInRadians);

			if (gravitationalAcceleration != 0.0) {
				// section VIII: air resistance + wind + gravity
				calculatingFunc = [this, &horizontalAcceleration, &verticalAcceleration, &horizontalBallVelocity, &verticalBallVelocity, k, gravitationalAcceleration, horizontalWindVelocity, verticalWindVelocity, ballMass]() {
					this->CalculateAccelerations(horizontalAcceleration, verticalAcceleration, horizontalBallVelocity, verticalBallVelocity, k, gravitationalAcceleration, horizontalWindVelocity, verticalWindVelocity, ballMass);
				};
			}
			else {
				// section VI: air resistance + wind
				calculatingFunc = [this, &horizontalAcceleration, &verticalAcceleration, &horizontalBallVelocity, &verticalBallVelocity, k, horizontalWindVelocity, verticalWindVelocity, ballMass]() {
					this->CalculateAccelerations(horizontalAcceleration, verticalAcceleration, horizontalBallVelocity, verticalBallVelocity, k, horizontalWindVelocity, verticalWindVelocity, ballMass);
				};
			}
		}
		else {
			if (gravitationalAcceleration != 0.0) {
				// section VII: air resistance + gravity
				calculatingFunc = [this, &horizontalAcceleration, &verticalAcceleration, horizontalBallVelocity, verticalBallVelocity, k, gravitationalAcceleration, ballMass](){
					this->CalculateAccelerations(horizontalAcceleration, verticalAcceleration, horizontalBallVelocity, verticalBallVelocity, k, gravitationalAcceleration, ballMass);
				};
			}
			else {
				// section V: air resistance only
				calculatingFunc = [this, &horizontalAcceleration, &verticalAcceleration, horizontalBallVelocity, verticalBallVelocity, k, ballMass](){
					this->CalculateAccelerations(horizontalAcceleration, verticalAcceleration, horizontalBallVelocity, verticalBallVelocity, k, ballMass);
				};
			}
		}

		double kX[4], kY[4], kHorizontalVelocity[4], kVerticalVelocity[4], originalHorizontalBallVelocity, originalVerticalBallVelocity;
		size_t pointsNumber;
		int iterationsLimit = 10000; // required for some cases because they may go on forever

		do { //Runge-Kutta method
			originalHorizontalBallVelocity = horizontalBallVelocity;
			originalVerticalBallVelocity = verticalBallVelocity;

			RungeKuttaMethodPart(calculatingFunc, kX[0], kY[0], kHorizontalVelocity[0], kVerticalVelocity[0], timeStep, horizontalBallVelocity, verticalBallVelocity, horizontalAcceleration, verticalAcceleration);

			horizontalBallVelocity = originalHorizontalBallVelocity + kHorizontalVelocity[0] / 2;
			verticalBallVelocity = originalVerticalBallVelocity + kVerticalVelocity[0] / 2;

			RungeKuttaMethodPart(calculatingFunc, kX[1], kY[1], kHorizontalVelocity[1], kVerticalVelocity[1], timeStep, horizontalBallVelocity, verticalBallVelocity, horizontalAcceleration, verticalAcceleration);

			horizontalBallVelocity = originalHorizontalBallVelocity + kHorizontalVelocity[1] / 2;
			verticalBallVelocity = originalVerticalBallVelocity + kVerticalVelocity[1] / 2;

			RungeKuttaMethodPart(calculatingFunc, kX[2], kY[2], kHorizontalVelocity[2], kVerticalVelocity[2], timeStep, horizontalBallVelocity, verticalBallVelocity, horizontalAcceleration, verticalAcceleration);

			horizontalBallVelocity = originalHorizontalBallVelocity + kHorizontalVelocity[2];
			verticalBallVelocity = originalVerticalBallVelocity + kVerticalVelocity[2];

			RungeKuttaMethodPart(calculatingFunc, kX[3], kY[3], kHorizontalVelocity[3], kVerticalVelocity[3], timeStep, horizontalBallVelocity, verticalBallVelocity, horizontalAcceleration, verticalAcceleration);

			x += (kX[0] + 2 * kX[1] + 2 * kX[2] + kX[3]) / 6;
			y += (kY[0] + 2 * kY[1] + 2 * kY[2] + kY[3]) / 6;
			horizontalBallVelocity = originalHorizontalBallVelocity + (kHorizontalVelocity[0] + 2 * kHorizontalVelocity[1] + 2 * kHorizontalVelocity[2] + kHorizontalVelocity[3]) / 6;
			verticalBallVelocity = originalVerticalBallVelocity + (kVerticalVelocity[0] + 2 * kVerticalVelocity[1] + 2 * kVerticalVelocity[2] + kVerticalVelocity[3]) / 6;

			xAxisCoordinates.push_back(x);
			yAxisCoordinates.push_back(y);

			// actions to check whether there is an extreme case due to which the ball does not fall
			pointsNumber = xAxisCoordinates.size();

			// checking whether the double type inaccuracy affects the lack of speed change at a certain acceleration
			calculatingFunc();
			if (verticalBallVelocity >= 0.0 && verticalAcceleration != 0.0 && verticalBallVelocity == originalVerticalBallVelocity && horizontalAcceleration != 0.0 && horizontalBallVelocity == originalHorizontalBallVelocity) {
				iterationsLimit--;
			}
			cout << std::fixed << std::setprecision(31) << verticalAcceleration << " " << horizontalAcceleration << " " << verticalBallVelocity << " " << horizontalBallVelocity << endl;
		} while (y - ballRadius > 0.0 && !(pointsNumber >= 2 && xAxisCoordinates[pointsNumber - 2] == x && yAxisCoordinates[pointsNumber - 2] == y) && iterationsLimit > 0);

		if (verticalBallVelocity >= 0.0 && verticalAcceleration != 0.0 && verticalBallVelocity == originalVerticalBallVelocity && horizontalAcceleration != 0.0 && horizontalBallVelocity == originalHorizontalBallVelocity) {
			warning = "Symulacja została wstrzymana po czasie 10 sekund, gdyż lot kuli trwa wiecznie";
		}
	}
	else {
		if (gravitationalAcceleration != 0.0) {
			// section IV: no air resistance, gravity only
			double time = timeStep;
			do {
				x = horizontalBallVelocity * time;
				y = verticalBallVelocity * time - 0.5 * gravitationalAcceleration * time * time + ballRadius + initialDistanceFromGround;

				time += timeStep;

				xAxisCoordinates.push_back(x);
				yAxisCoordinates.push_back(y);
			} while (y - ballRadius > 0.0);
		}
		else {
			// section III: no air resistance, no gravity
			double time = timeStep;
			int iterationsLimit = 10000; // required because the case goes on forever
			warning = "Symulacja została wstrzymana po czasie 10 sekund, gdyż lot kuli trwa wiecznie";
			do {
				x = horizontalBallVelocity * time;
				y = verticalBallVelocity * time + ballRadius + initialDistanceFromGround;

				time += timeStep;
				iterationsLimit--;

				xAxisCoordinates.push_back(x);
				yAxisCoordinates.push_back(y);
			} while (iterationsLimit > 0);
		}
	}
}

/**
 * @brief
 * These calculations are for air resistance, wind and gravity
 */
void TrajectoryCalculator::CalculateAccelerations( // VIII
	double& horizontalAcceleration,
	double& verticalAcceleration,
	double horizontalBallVelocity,
	double verticalBallVelocity,
	double k,
	double gravitationalAcceleration,
	double horizontalWindVelocity,
	double verticalWindVelocity,
	double ballMass
) {
	double horizontalVelocityDiff = horizontalBallVelocity - horizontalWindVelocity;
	double verticalVelocityDiff = verticalBallVelocity - verticalWindVelocity;

	double relativeVelocity = sqrt(horizontalVelocityDiff * horizontalVelocityDiff + verticalVelocityDiff * verticalVelocityDiff);

	horizontalAcceleration = -(k / ballMass) * relativeVelocity * horizontalVelocityDiff;
	verticalAcceleration = -gravitationalAcceleration - (k / ballMass) * relativeVelocity * verticalVelocityDiff;
}

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
void TrajectoryCalculator::CalculateAccelerations( // V
	double& horizontalAcceleration,
	double& verticalAcceleration,
	double horizontalBallVelocity,
	double verticalBallVelocity,
	double k,
	double ballMass
) {
	double currentVelocity = sqrt(horizontalBallVelocity * horizontalBallVelocity + verticalBallVelocity * verticalBallVelocity);
	
	horizontalAcceleration = -(k / ballMass) * currentVelocity * horizontalBallVelocity;
	verticalAcceleration = -(k / ballMass) * currentVelocity * verticalBallVelocity;
}


/**
 * @brief
 * These calculations are for air resistance and gravity
 */
void TrajectoryCalculator::CalculateAccelerations( // VII
	double& horizontalAcceleration,
	double& verticalAcceleration,
	double horizontalBallVelocity,
	double verticalBallVelocity,
	double k,
	double gravitationalAcceleration,
	double ballMass
) {
	double currentVelocity = sqrt(horizontalBallVelocity * horizontalBallVelocity + verticalBallVelocity * verticalBallVelocity);
	
	horizontalAcceleration = -(k / ballMass) * currentVelocity * horizontalBallVelocity;
	verticalAcceleration = -gravitationalAcceleration -(k / ballMass) * currentVelocity * verticalBallVelocity;
}


/**
 * @brief
 * These calculations are for air resistance and wind
 */
void TrajectoryCalculator::CalculateAccelerations( // VI
	double& horizontalAcceleration,
	double& verticalAcceleration,
	double horizontalBallVelocity,
	double verticalBallVelocity,
	double k,
	double horizontalWindVelocity,
	double verticalWindVelocity,
	double ballMass
) {
	double horizontalVelocityDiff = horizontalBallVelocity - horizontalWindVelocity;
	double verticalVelocityDiff = verticalBallVelocity - verticalWindVelocity;

	double relativeVelocity = sqrt(horizontalVelocityDiff * horizontalVelocityDiff + verticalVelocityDiff * verticalVelocityDiff);

	horizontalAcceleration = -(k / ballMass) * relativeVelocity * horizontalVelocityDiff;
	verticalAcceleration = - (k / ballMass) * relativeVelocity * verticalVelocityDiff;
}


void TrajectoryCalculator::RungeKuttaMethodPart(
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
) {
	calculatingFunc();
	kX = timeStep * horizontalBallVelocity;
	kY = timeStep * verticalBallVelocity;
	kHorizontalVelocity = timeStep * horizontalAcceleration;
	kVerticalVelocity = timeStep * verticalAcceleration;
}
