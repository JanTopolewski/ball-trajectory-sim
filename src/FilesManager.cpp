#include "../include/FilesManager.h"
#include <fstream>
#include <sstream>
#include "FilesManager.h"

#ifdef WIN32
#define OS_SEP '\\'
#else
#define OS_SEP '/'
#endif

using namespace std;

vector<SpaceObject> FilesManager::getSpaceObjectsData()
{
	return spaceObjectsData;
}


vector<string> FilesManager::getSpaceObjectsNames()
{
	std::vector<string> spaceObjectNames;

	for (const SpaceObject& spaceObject : spaceObjectsData) {
		spaceObjectNames.push_back(spaceObject.polishName);
	}

	return spaceObjectNames;
}


void FilesManager::loadSpaceObjectsData()
{
	spaceObjectsData = {};

	ifstream file;
	file.open("data/space_objects_data.csv", ios::in);

	string line;
	getline(file, line); // used to skip columns' headers

	while (getline(file, line)) {
		stringstream ss(line);
		string columns[4];

		for (int i = 0; i < 4; ++i) {
			getline(ss, columns[i], ',');
		}

		SpaceObject spaceObject;

		spaceObject.polishName = columns[0];
		spaceObject.gravitationalAcceleration = stod(columns[1]);
		spaceObject.atmosphereDensity = stod(columns[2]);
		spaceObject.compositionType = columns[3];

		spaceObjectsData.push_back(spaceObject);
	}

	file.close();
}

void FilesManager::saveSimulationData(Simulation *simulation, string fileName, string dirname = "dane", string extension = ".bin")
{
	ofstream file;
	string filePath = dirname+OS_SEP+fileName+extension;
	file.open(filePath, ios::out | ios::binary | ios::trunc);
	
	// double ballVelocity = simulation->ballVelocity;

	file.write((char*)&simulation->ballVelocity, sizeof(double));
	file.write((char*)&simulation->firingAngle, sizeof(double));
	file.write((char*)&simulation->ballRadius, sizeof(double));
	file.write((char*)&simulation->ballMass, sizeof(double));
	file.write((char*)&simulation->gravitationalAcceleration, sizeof(double));
	file.write((char*)&simulation->windVelocity, sizeof(double));
	file.write((char*)&simulation->windAngle, sizeof(double));
	file.write((char*)&simulation->atmosfericDensity, sizeof(double));

	int xVectorLength = simulation->xAxisCoordinates.size();
	int yVecotrLength = simulation->yAxisCoordinates.size();

	file.write((char*)&xVectorLength, sizeof(int));
	file.write((char*)simulation->xAxisCoordinates.data(), sizeof(double) * xVectorLength);

	file.write((char*)&yVecotrLength, sizeof(int));
	file.write((char*)simulation->yAxisCoordinates.data(), sizeof(double) * yVecotrLength);
	
	file.close();
	delete simulation;
}
