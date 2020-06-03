#include "team.h"

Team::Team(char color) {
	teamColor = color;
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

int Team::getPlasticCount(){ return this->plasticCount; }
int Team::getMetalCount(){ return this->metalCount; }
void Team::setBaseHealth(int health) { this->baseHealth = health; }
int Team::getBaseHealth(){ return this->baseHealth; }

bool Team::checkResources(char entityType) {
	switch (entityType) {
	case LASER_TYPE:
		if (metalCount >= LASER_METAL_REQ && plasticCount >= LASER_PLASTIC_REQ) {
			return true;
		}
		return false;
	case CLAW_TYPE:
		if (metalCount >= CLAW_METAL_REQ && plasticCount >= CLAW_PLASTIC_REQ) {
			return true;
		}
		return false;
	case SUPER_MINION_TYPE:
		if (metalCount >= SUPER_MINION_METAL_REQ && plasticCount >= SUPER_MINION_PLASTIC_REQ) {
			return true;
		}
		return false;
	default:
		std::cout << "Invalid entityType of " << entityType << " passed to team->checkResources!\n";
		return false;
	}
}

void Team::buildEntity(char entityType){
	switch (entityType) {
	case LASER_TYPE:
		if (metalCount >= LASER_METAL_REQ && plasticCount >= LASER_PLASTIC_REQ) {
			metalCount -= LASER_METAL_REQ;
			plasticCount -= LASER_PLASTIC_REQ;
		}
		break;
	case CLAW_TYPE:
		if (metalCount >= CLAW_METAL_REQ && plasticCount >= CLAW_PLASTIC_REQ) {
			metalCount -= CLAW_METAL_REQ;
			plasticCount -= CLAW_PLASTIC_REQ;
		}
		break;
	case SUPER_MINION_TYPE:
		if (metalCount >= SUPER_MINION_METAL_REQ && plasticCount >= SUPER_MINION_PLASTIC_REQ) {
			metalCount -= SUPER_MINION_METAL_REQ;
			plasticCount -= SUPER_MINION_PLASTIC_REQ;
		}
		break;
	default:
		std::cout << "Invalid entityType of " << entityType << " passed to team->buildEntity!\n";
	}
}

void Team::addResource(char type, int amount) {
	std::cout << teamColor << " team added " << amount << " of " << type << "\n";
	if (type == METAL_RES_TYPE) metalCount += amount;
	else if (type == PLASTIC_RES_TYPE) plasticCount += amount;
	else std::cout << teamColor << " team tried to add resource type " << type << "\n";
}

void Team::setData(TeamData data) {
	this->teamColor = data.teamColor;
	this->metalCount = data.metalCount;
	this->plasticCount = data.plasticCount;
	this->baseHealth = data.baseHealth;
	this->minionCount = data.minionCount;
	this->towerCount = data.towerCount;

	//print();
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