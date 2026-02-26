#include "../include/TrajectoryCalculator.h"
#include <vector>
#include <cmath>
#include <numbers>
#include <functional>
#include <limits>

using namespace std;

std::vector<double> TrajectoryCalculator::getXAxisCoordinates() const { return xAxisCoordinates; }
std::vector<double> TrajectoryCalculator::getYAxisCoordinates() const { return yAxisCoordinates; }
std::vector<double> TrajectoryCalculator::getZAxisCoordinates() const { return zAxisCoordinates; }
string TrajectoryCalculator::getWarning() const { return warning; }


void TrajectoryCalculator::CalculateData(
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
	double initialDistanceFromGround
) {
	// resetting vectors and warning message
	vector<double>().swap(xAxisCoordinates);
	vector<double>().swap(yAxisCoordinates);
	vector<double>().swap(zAxisCoordinates);
	warning = "";

	// starting position
	xAxisCoordinates.push_back(0.0);
	yAxisCoordinates.push_back(0.0);
	zAxisCoordinates.push_back(ballRadius + initialDistanceFromGround);

	// converting angles to radians and splitting velocity into the x, y and z axes
	double horizontalAngleInRadians = horizontalAngle * numbers::pi / 180.0;
	double verticalAngleInRadians = verticalAngle * numbers::pi / 180.0;
	double horizontalBallVelocity = ballVelocity * cos(verticalAngleInRadians);
	double vz = ballVelocity * sin(verticalAngleInRadians);
	double vx = horizontalBallVelocity * cos(horizontalAngleInRadians);
	double vy = horizontalBallVelocity * sin(horizontalAngleInRadians);

	double timeStep = 0.001, ax = 0.0, ay = 0.0, az = 0.0, k, posX = 0.0, posY = 0.0, posZ = ballRadius + initialDistanceFromGround;
	bool edgeCase = false;

	function<void()> calculatingFunc;

	if (atmosphericDensity != 0.0) {
		// air resistance
		k = 0.5 * atmosphericDensity * 0.47 * numbers::pi * ballRadius * ballRadius;

		if (ballRadius / ballMass > 1.0) {
			warning = "Unrealistic mass/radius ratios - simulation may be numerically unstable.\n";
		}

		if (windVelocity != 0.0) {
			// calculating wind velocities and angles in radians
			double windHorizontalAngleInRadians = windHorizontalAngle * numbers::pi / 180.0;
			double windVerticalAngleInRadians = windVerticalAngle * numbers::pi / 180.0;
			double horizontalWindVelocity = windVelocity * cos(windVerticalAngleInRadians);
			double windVZ = windVelocity * sin(windVerticalAngleInRadians);
			double windVX = horizontalWindVelocity * cos(windHorizontalAngleInRadians);
			double windVY = horizontalWindVelocity * sin(windHorizontalAngleInRadians);

			if (gravitationalAcceleration != 0.0) {
				// section VIII: air resistance + wind + gravity
				calculatingFunc = [this, &ax, &ay, &az, &vx, &vy, &vz, k, gravitationalAcceleration, windVX, windVY, windVZ, ballMass]() {
					this->CalculateAccelerations(ax, ay, az, vx, vy, vz, k, gravitationalAcceleration, windVX, windVY, windVZ, ballMass);
					};
			}
			else {
				// section VI: air resistance + wind
				calculatingFunc = [this, &ax, &ay, &az, &vx, &vy, &vz, k, windVX, windVY, windVZ, ballMass]() {
					this->CalculateAccelerations(ax, ay, az, vx, vy, vz, k, windVX, windVY, windVZ, ballMass);
					};
			}
		}
		else {
			if (gravitationalAcceleration != 0.0) {
				// section VII: air resistance + gravity
				calculatingFunc = [this, &ax, &ay, &az, &vx, &vy, &vz, k, gravitationalAcceleration, ballMass]() {
					this->CalculateAccelerations(ax, ay, az, vx, vy, vz, k, gravitationalAcceleration, ballMass);
					};
			}
			else {
				// section V: air resistance only
				calculatingFunc = [this, &ax, &ay, &az, &vx, &vy, &vz, k, ballMass]() {
					this->CalculateAccelerations(ax, ay, az, vx, vy, vz, k, ballMass);
					};

				edgeCase = true;
			}
		}

		double kX[4], kY[4], kZ[4], kXVelocity[4], kYVelocity[4], kZVelocity[4], originalVX, originalVY, originalVZ;
		size_t pointsNumber;
		int iterationsLimit = 500000, iterationsNumberForStopping = 2000; // required for some cases because they may go on forever

		do { //Runge-Kutta method
			originalVX = vx;
			originalVY = vy;
			originalVZ = vz;

			RungeKuttaMethodPart(calculatingFunc, kX[0], kY[0], kZ[0], kXVelocity[0], kYVelocity[0], kZVelocity[0], timeStep, vx, vy, vz, ax, ay, az);

			vx = originalVX + kXVelocity[0] / 2;
			vy = originalVY + kYVelocity[0] / 2;
			vz = originalVZ + kZVelocity[0] / 2;

			RungeKuttaMethodPart(calculatingFunc, kX[1], kY[1], kZ[0], kXVelocity[1], kYVelocity[1], kZVelocity[1], timeStep, vx, vy, vz, ax, ay, az);

			vx = originalVX + kXVelocity[1] / 2;
			vy = originalVY + kYVelocity[1] / 2;
			vz = originalVZ + kZVelocity[1] / 2;

			RungeKuttaMethodPart(calculatingFunc, kX[2], kY[2], kZ[2], kXVelocity[2], kYVelocity[2], kZVelocity[2], timeStep, vx, vy, vz, ax, ay, az);

			vx = originalVX + kXVelocity[2];
			vy = originalVY + kYVelocity[2];
			vz = originalVZ + kZVelocity[2];

			RungeKuttaMethodPart(calculatingFunc, kX[3], kY[3], kZ[3], kXVelocity[3], kYVelocity[3], kZVelocity[3], timeStep, vx, vy, vz, ax, ay, az);

			posX += (kX[0] + 2 * kX[1] + 2 * kX[2] + kX[3]) / 6;
			posY += (kY[0] + 2 * kY[1] + 2 * kY[2] + kY[3]) / 6;
			posZ += (kZ[0] + 2 * kZ[1] + 2 * kZ[2] + kZ[3]) / 6;

			vx = originalVX + (kXVelocity[0] + 2 * kXVelocity[1] + 2 * kXVelocity[2] + kXVelocity[3]) / 6;
			vy = originalVY + (kYVelocity[0] + 2 * kYVelocity[1] + 2 * kYVelocity[2] + kYVelocity[3]) / 6;
			vz = originalVZ + (kZVelocity[0] + 2 * kZVelocity[1] + 2 * kZVelocity[2] + kZVelocity[3]) / 6;

			if (isnan(posX) || isnan(posY) || isnan(posZ) || isnan(vx) || isnan(vy) || isnan(vz) || isinf(vx) || isinf(vy) || isinf(vz)) {
				warning += "The simulation was stopped due to the ball's flight properties going beyond the valid simulation range";
				break;
			}
			else if (isinf(posX) || isinf(posY) || isinf(posZ)) {
				if (isinf(posX) && posX < 0) {
					xAxisCoordinates.push_back(numeric_limits<double>::lowest());
				}
				else if (isinf(posX) && posX > 0) {
					xAxisCoordinates.push_back(numeric_limits<double>::max());
				}
				else {
					xAxisCoordinates.push_back(posX);
				}

				if (isinf(posY) && posY < 0) {
					yAxisCoordinates.push_back(numeric_limits<double>::lowest());
				}
				else if (isinf(posY) && posY > 0) {
					yAxisCoordinates.push_back(numeric_limits<double>::max());
				}
				else {
					yAxisCoordinates.push_back(posY);
				}

				if (isinf(posZ) && posZ < 0) {
					zAxisCoordinates.push_back(numeric_limits<double>::lowest());
				}
				else if (isinf(posZ) && posZ > 0) {
					zAxisCoordinates.push_back(numeric_limits<double>::max());
				}
				else {
					zAxisCoordinates.push_back(posZ);
				}

				warning += "The simulation was stopped due to the ball's trajectory going beyond the valid simulation range";
				break;
			}
			else {
				xAxisCoordinates.push_back(posX);
				yAxisCoordinates.push_back(posY);
				zAxisCoordinates.push_back(posZ);
			}

			// actions to check whether there is an extreme case due to which the ball does not fall
			pointsNumber = xAxisCoordinates.size();
			iterationsLimit--;

			// checking whether the double type inaccuracy affects the lack of speed change at a certain acceleration
			calculatingFunc();
			if (vz >= 0.0 && az != 0.0 && vz == originalVZ && ax != 0.0 && vx == originalVX && ay != 0.0 && vy == originalVY) {
				iterationsNumberForStopping--;
			}

			if (edgeCase && (vz < 0.0 || vx < 0.0))
				break;
		} while (posZ - ballRadius > 0.0 && !(pointsNumber >= 2 && xAxisCoordinates[pointsNumber - 2] == posX && yAxisCoordinates[pointsNumber - 2] == posY && zAxisCoordinates[pointsNumber - 2] == posZ) && iterationsNumberForStopping > 0 && iterationsLimit > 0);

		if (iterationsNumberForStopping == 0) {
			warning += "The simulation was paused after another 2 seconds of flight from detection, as the ball's flight takes forever";
		}
		else if (iterationsLimit == 0) {
			warning += "The simulation was stopped after 500 seconds of ball flight due to exceeding the maximum allowed flight time";
		}
	}
	else {
		if (gravitationalAcceleration != 0.0) {
			// section IV: no air resistance, gravity only
			double time = timeStep;
			do {
				posX = vx * time;
				posY = vz * time;
				posZ = vz * time - 0.5 * gravitationalAcceleration * time * time + ballRadius + initialDistanceFromGround;

				time += timeStep;

				xAxisCoordinates.push_back(posX);
				yAxisCoordinates.push_back(posY);
				zAxisCoordinates.push_back(posZ);
			} while (posZ - ballRadius > 0.0);
		}
		else {
			// section III: no air resistance, no gravity
			double time = timeStep;
			int iterationsLimit = 10000; // required because the case goes on forever
			warning = "The simulation was paused after 10 seconds of flight because the ball's flight takes forever";
			do {
				posX = vx * time;
				posY = vy * time;
				posZ = vz * time + ballRadius + initialDistanceFromGround;

				time += timeStep;
				iterationsLimit--;

				xAxisCoordinates.push_back(posX);
				yAxisCoordinates.push_back(posY);
				zAxisCoordinates.push_back(posZ);
			} while (iterationsLimit > 0);
		}
	}
}


void TrajectoryCalculator::CalculateAccelerations( // V
	double& ax,
	double& ay,
	double& az,
	double vx,
	double vy,
	double vz,
	double k,
	double ballMass
) {
	double currentVelocity = sqrt(vx * vx + vy * vy + vz * vz);

	ax = -(k / ballMass) * currentVelocity * vx;
	ay = -(k / ballMass) * currentVelocity * vy;
	az = -(k / ballMass) * currentVelocity * vz;
}


void TrajectoryCalculator::CalculateAccelerations( // VI
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
) {
	double vxDiff = vx - windVX;
	double vyDiff = vy - windVY;
	double vzDiff = vz - windVZ;

	double relativeVelocity = sqrt(vxDiff * vxDiff + vyDiff * vyDiff + vzDiff * vzDiff);

	ax = -(k / ballMass) * relativeVelocity * vxDiff;
	ay = -(k / ballMass) * relativeVelocity * vyDiff;
	az = -(k / ballMass) * relativeVelocity * vzDiff;
}


void TrajectoryCalculator::CalculateAccelerations( // VII
	double& ax,
	double& ay,
	double& az,
	double vx,
	double vy,
	double vz,
	double k,
	double gravitationalAcceleration,
	double ballMass
) {
	double currentVelocity = sqrt(vx * vx + vy * vy + vz * vz);

	ax = -(k / ballMass) * currentVelocity * vx;
	ay = -(k / ballMass) * currentVelocity * vy;
	az = -gravitationalAcceleration - (k / ballMass) * currentVelocity * vz;
}


void TrajectoryCalculator::CalculateAccelerations( // VIII
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
) {
	double vxDiff = vx - windVX;
	double vyDiff = vy - windVY;
	double vzDiff = vz - windVZ;

	double relativeVelocity = sqrt(vxDiff * vxDiff + vyDiff * vyDiff + vzDiff * vzDiff);

	ax = -(k / ballMass) * relativeVelocity * vxDiff;
	ay = -(k / ballMass) * relativeVelocity * vyDiff;
	az = -gravitationalAcceleration - (k / ballMass) * relativeVelocity * vzDiff;
}


void TrajectoryCalculator::RungeKuttaMethodPart(
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
) {
	calculatingFunc();
	kX = timeStep * vx;
	kY = timeStep * vy;
	kZ = timeStep * vz;
	kXVelocity = timeStep * ax;
	kYVelocity = timeStep * ay;
	kZVelocity = timeStep * az;
}
