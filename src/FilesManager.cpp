#include "../include/FilesManager.h"
#include <fstream>
#include <sstream>
#include <filesystem>

const std::filesystem::path ROOT_PATH = PROJECT_ROOT_DIR;

using namespace std;

vector<SpaceObject> FilesManager::getSpaceObjectsData()
{
	return spaceObjectsData;
}


vector<string> FilesManager::getSpaceObjectsNames()
{
	std::vector<string> spaceObjectNames;

	for (const SpaceObject& spaceObject : spaceObjectsData) {
		spaceObjectNames.push_back(spaceObject.name);
	}

	return spaceObjectNames;
}


void FilesManager::loadSpaceObjectsData()
{
	spaceObjectsData = {};

	ifstream file;
	filesystem::path path = ROOT_PATH / "data" / "space_objects_data.csv";
	
	file.open(path, ios::in);

	string line;
	getline(file, line); // used to skip columns' headers

	while (getline(file, line)) {
		stringstream ss(line);
		string columns[4];

		for (int i = 0; i < 4; ++i) {
			getline(ss, columns[i], ',');
		}

		SpaceObject spaceObject;

		spaceObject.name = columns[0];
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

	filesystem::path filePath = ROOT_PATH / dirname / (fileName + extension);
	file.open(filePath, ios::out | ios::binary | ios::trunc);
	
	// double ballVelocity = simulation->ballVelocity;

	file.write((char*)&simulation->ballVelocity, sizeof(float));
	file.write((char*)&simulation->horizontalAngle, sizeof(float));
	file.write((char*)&simulation->verticalAngle, sizeof(float));
	file.write((char*)&simulation->ballRadius, sizeof(float));
	file.write((char*)&simulation->ballMass, sizeof(float));
	file.write((char*)&simulation->gravitationalAcceleration, sizeof(float));
	file.write((char*)&simulation->windVelocity, sizeof(float));
	file.write((char*)&simulation->windHorizontalAngle, sizeof(float));
	file.write((char*)&simulation->windVerticalAngle, sizeof(float));
	file.write((char*)&simulation->atmosfericDensity, sizeof(float));
	file.write((char*)&simulation->initialDistanceFromGround, sizeof(float));

	size_t xVectorLength = simulation->xAxisCoordinates.size();
	size_t yVecotrLength = simulation->yAxisCoordinates.size();
	size_t zVecotrLength = simulation->zAxisCoordinates.size();

	file.write((char*)&xVectorLength, sizeof(size_t));
	file.write((char*)simulation->xAxisCoordinates.data(), sizeof(double) * xVectorLength);

	file.write((char*)&yVecotrLength, sizeof(size_t));
	file.write((char*)simulation->yAxisCoordinates.data(), sizeof(double) * yVecotrLength);

	file.write((char*)&zVecotrLength, sizeof(size_t));
	file.write((char*)simulation->zAxisCoordinates.data(), sizeof(double) * zVecotrLength);

	size_t warningLength = simulation->warning.size();
	file.write((char*)&warningLength, sizeof(size_t));
	if (warningLength > 0)
		file.write(simulation->warning.data(), warningLength);

	size_t xAdditionalVectorLength = simulation->xAxisAdditionalTrajectory.size();
	size_t yAdditionalVectorLength = simulation->yAxisAdditionalTrajectory.size();
	size_t zAdditionalVectorLength = simulation->zAxisAdditionalTrajectory.size();

	file.write((char*)&xAdditionalVectorLength, sizeof(size_t));
	file.write((char*)simulation->xAxisAdditionalTrajectory.data(), sizeof(double) * xAdditionalVectorLength);

	file.write((char*)&yAdditionalVectorLength, sizeof(size_t));
	file.write((char*)simulation->yAxisAdditionalTrajectory.data(), sizeof(double) * yAdditionalVectorLength);

	file.write((char*)&zAdditionalVectorLength, sizeof(size_t));
	file.write((char*)simulation->zAxisAdditionalTrajectory.data(), sizeof(double) * zAdditionalVectorLength);

	file.write((char*)&simulation->hasTarget, sizeof(bool));
	if (simulation->hasTarget) {
		file.write((char*)&simulation->targetDistance, sizeof(float));
		file.write((char*)&simulation->targetHorizontalAngle, sizeof(float));
	}
	
	file.close();
	delete simulation;
}


Simulation* FilesManager::readSimulationData(string fileName, string dirname, string extension) {
	ifstream file;

	filesystem::path filePath = ROOT_PATH / dirname / (fileName + extension);

	file.open(filePath, ios::in | ios::binary);

	Simulation* simulation = new Simulation();

	file.read(reinterpret_cast<char*>(&simulation->ballVelocity), sizeof(float));
	file.read(reinterpret_cast<char*>(&simulation->horizontalAngle), sizeof(float));
	file.read(reinterpret_cast<char*>(&simulation->verticalAngle), sizeof(float));
	file.read(reinterpret_cast<char*>(&simulation->ballRadius), sizeof(float));
	file.read(reinterpret_cast<char*>(&simulation->ballMass), sizeof(float));
	file.read(reinterpret_cast<char*>(&simulation->gravitationalAcceleration), sizeof(float));
	file.read(reinterpret_cast<char*>(&simulation->windVelocity), sizeof(float));
	file.read(reinterpret_cast<char*>(&simulation->windHorizontalAngle), sizeof(float));
	file.read(reinterpret_cast<char*>(&simulation->windVerticalAngle), sizeof(float));
	file.read(reinterpret_cast<char*>(&simulation->atmosfericDensity), sizeof(float));
	file.read(reinterpret_cast<char*>(&simulation->initialDistanceFromGround), sizeof(float));

	size_t xVectorLength, yVectorLength, zVectorLength, warningLength, xAddtionalVectorLength, yAdditinalVectorLength, zAdditionalVectorLength;
	file.read(reinterpret_cast<char*>(&xVectorLength), sizeof(size_t));
	simulation->xAxisCoordinates.resize(xVectorLength);
	file.read(reinterpret_cast<char*>(simulation->xAxisCoordinates.data()), sizeof(double) * xVectorLength);

	file.read(reinterpret_cast<char*>(&yVectorLength), sizeof(size_t));
	simulation->yAxisCoordinates.resize(yVectorLength);
	file.read(reinterpret_cast<char*>(simulation->yAxisCoordinates.data()), sizeof(double) * yVectorLength);

	file.read(reinterpret_cast<char*>(&zVectorLength), sizeof(size_t));
	simulation->zAxisCoordinates.resize(zVectorLength);
	file.read(reinterpret_cast<char*>(simulation->zAxisCoordinates.data()), sizeof(double) * zVectorLength);

	file.read(reinterpret_cast<char*>(&warningLength), sizeof(size_t));
	simulation->warning.resize(warningLength);
	if (warningLength > 0)
		file.read(&simulation->warning[0], warningLength);

	file.read(reinterpret_cast<char*>(&xAddtionalVectorLength), sizeof(size_t));
	simulation->xAxisAdditionalTrajectory.resize(xAddtionalVectorLength);
	file.read(reinterpret_cast<char*>(simulation->xAxisAdditionalTrajectory.data()), sizeof(double) * xAddtionalVectorLength);

	file.read(reinterpret_cast<char*>(&yAdditinalVectorLength), sizeof(size_t));
	simulation->yAxisAdditionalTrajectory.resize(yAdditinalVectorLength);
	file.read(reinterpret_cast<char*>(simulation->yAxisAdditionalTrajectory.data()), sizeof(double) * yAdditinalVectorLength);

	file.read(reinterpret_cast<char*>(&zAdditionalVectorLength), sizeof(size_t));
	simulation->zAxisAdditionalTrajectory.resize(zAdditionalVectorLength);
	file.read(reinterpret_cast<char*>(simulation->zAxisAdditionalTrajectory.data()), sizeof(double) * zAdditionalVectorLength);

	file.read(reinterpret_cast<char*>(&simulation->hasTarget), sizeof(bool));
	if (simulation->hasTarget) {
		file.read(reinterpret_cast<char*>(&simulation->targetDistance), sizeof(float));
		file.read(reinterpret_cast<char*>(&simulation->targetHorizontalAngle), sizeof(float));
	}
	else {
		simulation->targetDistance = 0.0f;
		simulation->targetHorizontalAngle = 0.0f;
	}

	file.close();
	return simulation;
}


vector<string> FilesManager::getSavedSimulationsNames(string dirname, string extension)
{
	vector<string> simulationsNames;

	filesystem::path path = ROOT_PATH / dirname;

	for (const auto& fileEntry : filesystem::directory_iterator(path)) {
		if (fileEntry.is_regular_file() && fileEntry.path().extension() == extension) {
			simulationsNames.push_back(fileEntry.path().stem().string());
		}
	}

	return simulationsNames;
}


bool FilesManager::checkFileExistence(string fileName, string dirname, string extension) {
	filesystem::path filePath = ROOT_PATH / dirname / (fileName + extension);
	return filesystem::exists(filePath);
}
