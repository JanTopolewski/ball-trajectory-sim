#include "../include/TrajectoryCalculator.h"
#include <vector>
#include <cmath>
#include <numbers>
#include <functional>

using namespace std;

TrajectoryCalculator::TrajectoryCalculator()
	:tempHorizontalBallVelocity{0.0},
	tempVerticalBallVelocity{0.0},
	horizontalAcceleration{0.0},
	verticalAcceleration{0.0} {}

std::vector<double> TrajectoryCalculator::getXAxisCoordinates() const { return xAxisCoordinates; }
std::vector<double> TrajectoryCalculator::getYAxisCoordinates() const { return yAxisCoordinates; }


void TrajectoryCalculator::CalculateData(
	double ballVelocity,
	double firingAngle,
	double ballRadius,
	double ballMass,
	double gravitationalAcceleration,
	double windVelocity,
	double windAngle,
	double atmosphericDensity
) {
	double angleInRadians = firingAngle * numbers::pi / 180.0;
	tempHorizontalBallVelocity = ballVelocity * cos(angleInRadians);
	tempVerticalBallVelocity = ballVelocity * sin(angleInRadians);

	double timeStep = 0.01;

	function<void()> calculatingFunc;

	if (atmosphericDensity != 0.0) {//wtedy wystêpuje opór powietrza
		double k = 0.5 * atmosphericDensity * 0.47 * numbers::pi * ballRadius * ballRadius;

		if (windVelocity != 0.0) {
			double windAngleInRadians = windAngle * numbers::pi / 180.0;
			double horizontalWindVelocity = windVelocity * cos(windAngleInRadians);
			double verticalWindVelocity = windVelocity * sin(windAngleInRadians);

			if (gravitationalAcceleration != 0.0) {
				calculatingFunc = [this, k, gravitationalAcceleration, horizontalWindVelocity, verticalWindVelocity, ballMass]() {
					this->CalculateAccelerations(k, gravitationalAcceleration, horizontalWindVelocity, verticalWindVelocity, ballMass);
					};
			}
			else {

			}
		}
		else {
			if (gravitationalAcceleration != 0.0) {

			}
			else {

			}
		}
	}
	else {
		if (gravitationalAcceleration != 0.0) {
			calculatingFunc = [this, gravitationalAcceleration]() {
				this->CalculateAccelerations(gravitationalAcceleration);
				};
		}
		else {

		}
	}


	double x = 0.0, y = 0.0, kX[4], kY[4], kHorizontalVelocity[4], kVerticalVelocity[4], horizontalBallVelocity, verticalBallVelocity;
	
	do { //Runge-Kutta method
		horizontalBallVelocity = tempHorizontalBallVelocity;
		verticalBallVelocity = tempVerticalBallVelocity;

		calculatingFunc();
		kX[0] = timeStep * tempHorizontalBallVelocity;
		kY[0] = timeStep * tempVerticalBallVelocity;
		kHorizontalVelocity[0] = timeStep * horizontalAcceleration;
		kVerticalVelocity[0] = timeStep * verticalAcceleration;

		tempHorizontalBallVelocity = horizontalBallVelocity + kHorizontalVelocity[0] / 2;
		tempVerticalBallVelocity = verticalBallVelocity + kVerticalVelocity[0] / 2;

		calculatingFunc();
		kX[1] = timeStep * tempHorizontalBallVelocity;
		kY[1] = timeStep * tempVerticalBallVelocity;
		kHorizontalVelocity[1] = timeStep * horizontalAcceleration;
		kVerticalVelocity[1] = timeStep * verticalAcceleration;

		tempHorizontalBallVelocity = horizontalBallVelocity + kHorizontalVelocity[1] / 2;
		tempVerticalBallVelocity = verticalBallVelocity + kVerticalVelocity[1] / 2;

		calculatingFunc();
		kX[2] = timeStep * tempHorizontalBallVelocity;
		kY[2] = timeStep * tempVerticalBallVelocity;
		kHorizontalVelocity[2] = timeStep * horizontalAcceleration;
		kVerticalVelocity[2] = timeStep * verticalAcceleration;

		tempHorizontalBallVelocity = horizontalBallVelocity + kHorizontalVelocity[2];
		tempVerticalBallVelocity = verticalBallVelocity + kVerticalVelocity[2];

		calculatingFunc();
		kX[3] = timeStep * tempHorizontalBallVelocity;
		kY[3] = timeStep * tempVerticalBallVelocity;
		kHorizontalVelocity[3] = timeStep * horizontalAcceleration;
		kVerticalVelocity[3] = timeStep * verticalAcceleration;

		x += (kX[0] + 2 * kX[1] + 2 * kX[2] + kX[3]) / 6;
		y += (kY[0] + 2 * kY[1] + 2 * kY[2] + kY[3]) / 6;
		tempHorizontalBallVelocity = horizontalBallVelocity + (kHorizontalVelocity[0] + 2 * kHorizontalVelocity[1] + 2 * kHorizontalVelocity[2] + kHorizontalVelocity[3]) / 6;
		tempVerticalBallVelocity = verticalBallVelocity + (kVerticalVelocity[0] + 2 * kVerticalVelocity[1] + 2 * kVerticalVelocity[2] + kVerticalVelocity[3]) / 6;

		xAxisCoordinates.push_back(x);
		yAxisCoordinates.push_back(y);
	} while (y > 0.0);
}


void TrajectoryCalculator::CalculateAccelerations(double gravitationalAcceleration)
{
	horizontalAcceleration = 0;
	verticalAcceleration = -gravitationalAcceleration;
}


void TrajectoryCalculator::CalculateAccelerations(double k, double gravitationalAcceleration, double horizontalWindVelocity, double verticalWindVelocity, double ballMass)
{
	double horizontalVelocityDiff = tempHorizontalBallVelocity - horizontalWindVelocity;
	double verticalVelocityDiff = tempVerticalBallVelocity - verticalWindVelocity;

	double relativeVelocity = sqrt(horizontalVelocityDiff * horizontalVelocityDiff + verticalVelocityDiff * verticalVelocityDiff);

	horizontalAcceleration = -(k / ballMass) * relativeVelocity * horizontalVelocityDiff;
	verticalAcceleration = -gravitationalAcceleration - (k / ballMass) * relativeVelocity * verticalVelocityDiff;
}
