#pragma once
#include <vector>
#include <string>
#include "SpaceObject.h"

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
};