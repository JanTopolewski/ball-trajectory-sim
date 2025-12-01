#pragma once
#include <vector>
#include <string>
#include "SpaceObject.h"
#include "Simulation.h"

#ifdef WIN32
#define OS_SEP '\\'
#else
#define OS_SEP '/'
#endif


class FilesManager {
	std::vector<SpaceObject> spaceObjectsData;
public:
	/**
	 * @brief
	 * Allows getting all currently loaded data about space objects
	 * 
	 * @return
	 * Vector with loaded objects' data
	 */
	std::vector<SpaceObject> getSpaceObjectsData();

	/**
	 * @brief
	 * Allows getting all names of space objects from currently loaded data
	 *
	 * @return
	 * Vector with names of space objects as strings
	 */
	std::vector<std::string> getSpaceObjectsNames();

	/**
	 * @brief
	 * Loads all data from space_objects_data.csv file into a private vector
	 */
	void loadSpaceObjectsData();

	/**
	 * @brief
	 * Saves simulation data into the binary file and destroys the object
	 * 
	 * @param simulation simulation object containing all the simulation data
	 * @param fileName a fileName to save without any prefix or the extension
	 */
	void saveSimulationData(Simulation* simulation, std::string fileName, std::string dirname = "data", std::string extension = ".bin");

	/**
	 * @brief
	 * Reads simulation data from the binary file and returns it
	 *
	 * @param fileName a fileName to save without any prefix or the extension
	 * @param dirname a directory in which the function should look for the data
	 * @param extension an extension that files with data have
	 * 
	 * @return
	 * structure of type Simulation* with simulation data from particular file
	 */
	Simulation* readSimulationData(std::string fileName, std::string dirname = "data", std::string extension = ".bin");

	/**
	 * @brief
	 * Allows getting all names of files with simulations' data without their extensions
	 *
	 * @param dirname a directory in which the function should look for files with data
	 * @param extension an extension that files with data have
	 *
	 * @return
	 * Vector that contains all names of files with simulations' data as strings without their extensions
	 */
	std::vector<std::string> getSavedSimulationsNames(std::string dirname = "data", std::string extension = ".bin");
};