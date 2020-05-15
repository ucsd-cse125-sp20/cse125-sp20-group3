#include "team.h"

Team::Team(std::string objFilename) {
    Team::Team();
}

Team::Team() {
	winStatus = WIN_STATUS_NEUTRAL;
	metalCount = 0;
	plasticCount = 0;
    baseHealth = 100;
    unitCount = 0;
    towerCount = 0;
}

void Team::update() {
    //send the base health, unitCount and towerCount
}

void Team::incTower() { towerCount++; }
void Team::decTower() { towerCount--; }
void Team::incUnit() { unitCount++; }
void Team::decUnit() { unitCount--; }

bool Team::checkResources(char entityType) {
	switch (entityType) {
	case LASER_TYPE:
		if (metalCount >= LASER_METAL_REQ && plasticCount >= LASER_PLASTIC_REQ) {
			metalCount -= LASER_METAL_REQ;
			plasticCount -= LASER_PLASTIC_REQ;
			return true;
		}
		return false;
	case CLAW_TYPE:
		if (metalCount >= CLAW_METAL_REQ && plasticCount >= CLAW_PLASTIC_REQ) {
			metalCount -= CLAW_METAL_REQ;
			plasticCount -= CLAW_PLASTIC_REQ;
			return true;
		}
		return false;
	case SUPER_MINION_TYPE:
		if (metalCount >= SUPER_MINION_METAL_REQ && plasticCount >= SUPER_MINION_PLASTIC_REQ) {
			metalCount -= SUPER_MINION_METAL_REQ;
			plasticCount -= SUPER_MINION_PLASTIC_REQ;
			return true;
		}
		return false;
	default:
		std::cout << "Invalid entityType of " << entityType << " passed to team->checkResources!\n";
		return false;
	}
}
