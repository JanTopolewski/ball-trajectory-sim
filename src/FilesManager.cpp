#include "../include/FilesManager.h"
#include <fstream>
#include <sstream>
#include <filesystem>

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
	file.open(string("data") + OS_SEP + "space_objects_data.csv", ios::in);

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

void FilesManager::saveSimulationData(Simulation *simulation, string fileName, string dirname, string extension)
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

	size_t xVectorLength = simulation->xAxisCoordinates.size();
	size_t yVecotrLength = simulation->yAxisCoordinates.size();

	file.write((char*)&xVectorLength, sizeof(size_t));
	file.write((char*)simulation->xAxisCoordinates.data(), sizeof(double) * xVectorLength);

	file.write((char*)&yVecotrLength, sizeof(size_t));
	file.write((char*)simulation->yAxisCoordinates.data(), sizeof(double) * yVecotrLength);

	size_t warningLength = simulation->warning.size();
	file.write((char*)&warningLength, sizeof(size_t));
	file.write(simulation->warning.data(), warningLength);
	
	file.close();
	delete simulation;
}


Simulation* FilesManager::readSimulationData(string fileName, string dirname, string extension) {
	ifstream file;
	string filePath = dirname + OS_SEP + fileName + extension;
	file.open(filePath, ios::in | ios::binary);

	Simulation* simulation = new Simulation();

	file.read(reinterpret_cast<char*>(&simulation->ballVelocity), sizeof(double));
	file.read(reinterpret_cast<char*>(&simulation->firingAngle), sizeof(double));
	file.read(reinterpret_cast<char*>(&simulation->ballRadius), sizeof(double));
	file.read(reinterpret_cast<char*>(&simulation->ballMass), sizeof(double));
	file.read(reinterpret_cast<char*>(&simulation->gravitationalAcceleration), sizeof(double));
	file.read(reinterpret_cast<char*>(&simulation->windVelocity), sizeof(double));
	file.read(reinterpret_cast<char*>(&simulation->windAngle), sizeof(double));
	file.read(reinterpret_cast<char*>(&simulation->atmosfericDensity), sizeof(double));

	size_t xVectorLength, yVectorLength, warningLength;
	file.read(reinterpret_cast<char*>(&xVectorLength), sizeof(size_t));
	simulation->xAxisCoordinates.resize(xVectorLength);
	file.read(reinterpret_cast<char*>(simulation->xAxisCoordinates.data()), sizeof(double) * xVectorLength);

	file.read(reinterpret_cast<char*>(&yVectorLength), sizeof(size_t));
	simulation->yAxisCoordinates.resize(yVectorLength);
	file.read(reinterpret_cast<char*>(simulation->yAxisCoordinates.data()), sizeof(double) * yVectorLength);

	file.read(reinterpret_cast<char*>(&warningLength), sizeof(size_t));
	simulation->warning.resize(warningLength);
	file.read(&simulation->warning[0], warningLength);

	file.close();
	return simulation;
}


vector<string> FilesManager::getSavedSimulationsNames(string dirname, string extension)
{
	vector<string> simulationsNames;

	for (const auto& fileEntry : filesystem::directory_iterator(dirname)) {
		if (fileEntry.is_regular_file() && fileEntry.path().extension() == extension) {
			simulationsNames.push_back(fileEntry.path().stem().string());
		}
	}

	return simulationsNames;
}
