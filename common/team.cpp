#include "team.h"

Team::Team(char color) {
	teamColor = color;
	winStatus = WIN_STATUS_NEUTRAL;
	metalCount = 0;
	plasticCount = 0;
	baseHealth = 100; //TODO pointer to base and getHealth()? or have base set baseHealth?
	minionCount = 0;
	towerCount = 0;
}

void Team::incMinion() { minionCount++; }
void Team::decMinion() { minionCount--; }
void Team::incTower() { towerCount++; }
void Team::decTower() { towerCount--; }

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

void Team::setData(TeamData data) {
	this->teamColor = data.teamColor;
	this->metalCount = data.metalCount;
	this->plasticCount = data.plasticCount;
	this->baseHealth = data.baseHealth;
	this->minionCount = data.minionCount;
	this->towerCount = data.towerCount;
}

int Team::writeData(char buf[], int index) {
	TeamData data;
	data.teamColor = this->teamColor;
	data.metalCount = this->metalCount;
	data.plasticCount = this->plasticCount;
	data.baseHealth = this->baseHealth;
	data.minionCount = this->minionCount;
	data.towerCount = this->towerCount;
	((TeamData*)(buf + index))[0] = data;
	return sizeof(TeamData);
}

void Team::print() {
	std::cout << "team color: " << teamColor << " metal: " << metalCount << " plastic: " << plasticCount << " baseHealth: " << baseHealth << " minionCount: " << minionCount << " towerCount: " << towerCount << "\n";
}