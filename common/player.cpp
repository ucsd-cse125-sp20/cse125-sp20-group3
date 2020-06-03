#include "player.h"
#include "../server/SceneManager_Server.h"

Player::Player(GameObjectData data, int id, Team* t, SceneManager_Server* sm) : Entity(data, id, PLAYER_HEALTH, PLAYER_ATTACK, t, sm) {
	velocity_x = 0.f;
	velocity_z = 0.f;
	rotation_y = 0.f;
	acceleration_x = 0.f;
	acceleration_z = 0.f;
	buildMode = NEUTRAL;
	interactPos = vec3(0, 0, 0);

	if (sm != nullptr) {
		int flags = DETECTION_FLAG_PLAYER | DETECTION_FLAG_ENTITY | DETECTION_FLAG_COLLIDABLE;
		if (t->teamColor == RED_TEAM) flags = flags | DETECTION_FLAG_RED_TEAM;
		else flags = flags | DETECTION_FLAG_BLUE_TEAM;
		ObjectDetection::addObject(this, flags, 0, 0, 0, 0); //-PLAYER_WIDTH, PLAYER_WIDTH, -PLAYER_LENGTH, PLAYER_LENGTH);
	}
}

void Player::update(float deltaTime) {
	//should only execute on the server
	
	velocity_x *= 0.8f;
	velocity_z *= 0.8f;
	velocity_x += (cos(rotation_y) * acceleration_x - sin(rotation_y) * acceleration_z) * deltaTime;
	velocity_z += (cos(rotation_y) * acceleration_z + sin(rotation_y) * acceleration_x) * deltaTime;
	//printf("%f %f %f %f %f\n", velocity_x, velocity_z, acceleration_x, acceleration_z, deltaTime);

	mat4 oldModel = model;
	if (sqrt(velocity_x * velocity_x) > 0.001) {
		model[3][0] += velocity_x;
	}
	std::vector<GameObject*> c = ObjectDetection::getCollisions(this, DETECTION_FLAG_COLLIDABLE);
	if (c.size() > 0) {
		model = oldModel;
	}

	oldModel = model;
	if (sqrt(velocity_z * velocity_z) > 0.001) {
		model[3][2] += velocity_z;
	}
	c = ObjectDetection::getCollisions(this, DETECTION_FLAG_COLLIDABLE);
	if (c.size() > 0) {
		model = oldModel;
	}

	//movement ok, check for pickups
	std::vector<GameObject*> p = ObjectDetection::getCollisions(this, DETECTION_FLAG_PICKUP);
	for (GameObject* pickup : p) {
		std::pair<char, int> res = ((Pickup*)pickup)->pickup();
		this->team->addResource(res.first, res.second);
	}

	vec3 forward = vec3(-sin(rotation_y), 0, cos(rotation_y));
	vec3 right = cross(forward, vec3(0, 1, 0));
	model[0] = vec4(right, 0);
	model[2] = vec4(-forward, 0);

	float xpos = model[3][0];
	float zpos = model[3][2];
	float yrot = atan2(-model[2][2], -model[2][0]);
	//std::cout << "player x: " << xpos << " z: " << zpos << " y: " << yrot << "\n";

	ObjectDetection::updateObject(this);

	interactPos = this->getPosition() + (normalize(forward) * INTERACT_DISTANCE);
}

void Player::processInput(PlayerInput in) {
	this->setMoveAndDir(in.move_x, in.move_z, in.view_y_rot);

	//std::cout << "intent: " << in.buildIntent << " buildType: " << in.buildType << " harvestResource: " << in.harvestResource << "\n";

	if (in.buildIntent == BUILD_CANCEL) {
		buildMode = NEUTRAL;
		std::cout << "canceling\n";
	}
	else {
		if (in.buildIntent == BUILD_CONFIRM && buildMode != NEUTRAL) {
			int flags = DETECTION_FLAG_BUILD_NODE;
			if (team->teamColor == RED_TEAM) flags = flags | DETECTION_FLAG_RED_TEAM;
			else flags = flags | DETECTION_FLAG_BLUE_TEAM;
			BuildNode* buildTarget = (BuildNode*)ObjectDetection::getNearestObject(vec2(interactPos.getX(), interactPos.getZ()), flags, 0);
			
			if (buildTarget != nullptr && buildTarget->isOccupied() == false) {
				vec3 buildPos = buildTarget->getPosition();
				std::cout << "building at " << buildPos.getX() << " " << buildPos.getZ() << "\n";

				switch (buildMode) { //build something based on buildMode
				case LASER:
					if (team->checkResources(LASER_TYPE))
					{
						team->buildEntity(LASER_TYPE);
						int id = manager->spawnEntity(LASER_TYPE, buildPos.getX(), buildPos.getZ(), 0, this->team);
						buildTarget->build(id);
					}
					break;
				case CLAW:
					if (team->checkResources(CLAW_TYPE))
					{
						team->buildEntity(CLAW_TYPE);
						int id = manager->spawnEntity(CLAW_TYPE, buildPos.getX(), buildPos.getZ(), 0, this->team);
						buildTarget->build(id);
					} 
					break;
				case SUPER_MINION:
					if (team->checkResources(SUPER_MINION_TYPE))
					{
						team->buildEntity(SUPER_MINION_TYPE);
						int id = manager->spawnEntity(SUPER_MINION_TYPE, buildPos.getX(), buildPos.getZ(), 0, this->team);
						buildTarget->build(id);
					} 
					break;
				default:
					std::cout << "invalid buildMode\n";
				}

				buildMode = NEUTRAL; //reset to neutral after building
			}
			else std::cout << "tried building but no BuildNode in front\n";
		}
		else {
			buildMode = in.buildType == LASER_TYPE ? LASER : 
						in.buildType == CLAW_TYPE ? CLAW : 
						in.buildType == SUPER_MINION_TYPE ? SUPER_MINION : 
						buildMode;
			if(in.buildType != NO_BUILD_TYPE) std::cout << "setting buildMode to " << buildMode << "\n";
		}
	}

	if (in.harvestResource) {
		std::cout << "harvesting\n";
		std::cout << "player at " << getPosition().getX() << " " << getPosition().getZ() << "\n";
		std::cout << "interact at " << interactPos.getX() << " " << interactPos.getZ() << "\n";
		Resource* res = (Resource*)ObjectDetection::getNearestObject(vec2(interactPos.getX(), interactPos.getZ()), DETECTION_FLAG_RESOURCE, 0);
		if (res != nullptr && res->isActive()) {
			std::pair<char, int> resCount = res->harvest();
			this->team->addResource(resCount.first, resCount.second);
		}
	}
}

//read the move_x, move_z, and view_y_rot from PlayerInput
//ignore the other values
void Player::setMoveAndDir(int move_x, int move_z, float view_y_rot) {
	if (move_x < 0) acceleration_x = -1 * MOVE_SPEED;
	else if (move_x > 0) acceleration_x = MOVE_SPEED;
	else acceleration_x = 0;

	if (move_z < 0) acceleration_z = -1 * MOVE_SPEED;
	else if (move_z > 0) acceleration_z = MOVE_SPEED;
	else acceleration_z = 0;

	rotation_y = view_y_rot;
}

BUILD_MODE Player::getBuildMode() {
	return this->buildMode;
}

void Player::setEntData(EntityData data) {
	if (data.actionState == ACTION_STATE_IDLE) this->buildMode = NEUTRAL;
	else if (data.actionState == ACTION_STATE_MOVE) this->buildMode = LASER;
	else if (data.actionState == ACTION_STATE_ATTACK) this->buildMode = CLAW;
	else if (data.actionState == ACTION_STATE_FIRE) this->buildMode = SUPER_MINION;

	data.actionState = ACTION_STATE_IDLE;
	Entity::setEntData(data);
	//vec3 move_dir = this->getPosition() - lastPosition;
	//std::cout << "player actionState: " << (int)actionState << "\n";
	//std::cout << "player move_dir x: " << move_dir.getX() << " z: " << move_dir.getZ() << "\n";
	//TODO set run animation in move_dir taking into account forward vector

	//this->team->print();

	//std::cout << "player " << id << " buildMode: " << this->buildMode << "\n";
}

int Player::writeData(char buf[], int index) {
	EntityData entData;
	entData.GO_data = this->GameObject::getData();

	if (this->buildMode == NEUTRAL) entData.actionState = ACTION_STATE_IDLE;
	else if (this->buildMode == LASER) entData.actionState = ACTION_STATE_MOVE;
	else if (this->buildMode == CLAW) entData.actionState = ACTION_STATE_ATTACK;
	else if (this->buildMode == SUPER_MINION) entData.actionState = ACTION_STATE_FIRE;

	entData.teamColor = team != nullptr ? team->teamColor : NO_TEAM;
	entData.health = this->health;
	entData.targetID = attackTarget != nullptr ? attackTargetID : NO_TARGET_ID;
	((EntityData*)(buf + index))[0] = entData;
	return sizeof(EntityData);
}