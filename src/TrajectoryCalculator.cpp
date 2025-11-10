#include "../include/TrajectoryCalculator.h"
#include <vector>
#include <cmath>
#include <numbers>

using namespace std;

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
	horizontalBallVelocity = ballVelocity * cos(angleInRadians);
	verticalBallVelocity = ballVelocity * sin(angleInRadians);

	if (atmosphericDensity != 0.0) {//wtedy wystêpuje opór powietrza
		double k = 0.5 * atmosphericDensity * 0.47 * numbers::pi * ballRadius * ballRadius;
	}
}


void TrajectoryCalculator::CalculateDataForPoint(double gravitationalAcceleration)
{
	horizontalAcceleration = 0;
	verticalAcceleration = -gravitationalAcceleration;
}


void TrajectoryCalculator::CalculateDataForPoint(double k, double gravitationalAcceleration, double horizontalWindVelocity, double verticalWindVelocity, double ballMass)
{
	double horizontalVelocityDiff = horizontalBallVelocity - horizontalWindVelocity;
	double verticalVelocityDiff = verticalBallVelocity - verticalWindVelocity;

	double relativeVelocity = sqrt(horizontalVelocityDiff * horizontalVelocityDiff + verticalVelocityDiff * verticalVelocityDiff);

	horizontalAcceleration = -(k / ballMass) * relativeVelocity * horizontalVelocityDiff;
	verticalAcceleration = -gravitationalAcceleration - (k / ballMass) * relativeVelocity * verticalVelocityDiff;
}
