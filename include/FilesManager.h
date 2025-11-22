#pragma once
#include <vector>
#include <string>
#include "SpaceObject.h"

class FilesManager {
	std::vector<SpaceObject> spaceObjectsData;
public:
	std::vector<SpaceObject> getSpaceObjectsData();
	std::vector<std::string> getSpaceObjectsNames();
	void LoadSpaceObjectsData();
};