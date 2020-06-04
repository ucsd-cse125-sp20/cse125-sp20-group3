#include "SuperMinion.h"
#include "../server/SceneManager_Server.h"

SuperMinion::SuperMinion(GameObjectData data, int id, Team* t, SceneManager_Server* sm) : Minion(data, id, SUPER_MINION_HEALTH, SUPER_MINION_ATTACK, SUPER_MINION_ATK_RANGE, SUPER_MINION_ATK_INTERVAL, SUPER_MINION_VELOCITY, t, sm) {
	if (sm != nullptr) {
		int flags = DETECTION_FLAG_ENTITY | DETECTION_FLAG_COLLIDABLE | DETECTION_FLAG_MINION |
			DETECTION_FLAG_MINION_TARGET | DETECTION_FLAG_LASER_TARGET;
		if (t->teamColor == RED_TEAM) flags = flags | DETECTION_FLAG_RED_TEAM;
		else flags = flags | DETECTION_FLAG_BLUE_TEAM;
		ObjectDetection::addObject(this, flags, -SUPER_MINION_WIDTH, SUPER_MINION_WIDTH, -SUPER_MINION_LENGTH, SUPER_MINION_LENGTH);
	}
}

void SuperMinion::move(float deltaTime) {
	if (doneMoving) return; //small optimization

	actionState = ACTION_STATE_MOVE;

	float remaining_move_dist = velocity * deltaTime; //full movement distance this minion travels this tick

	while (remaining_move_dist > 0) { //while this minion still has distance to move
		vec3 dest_vec = destNode->getPosition() - this->getPosition(); //vector to reach destNode
		float dest_dist = length(dest_vec); //distance between this and destNode
		float move_dist = std::min(remaining_move_dist, dest_dist); //this iteration, move full move_dist, or only to destNode if closer
		vec3 move_vec = move_dist > 0 ? normalize(dest_vec) * move_dist : vec3(0); //move calculated distance along vector to destination

		mat4 oldModel = model; //super minions ignore movement collision

		/*auto other = ObjectDetection::getNearestObject(this, DETECTION_FLAG_COLLIDABLE, 1);
		if (other) {
			vec3 away = getPosition() - other->getPosition();
			float dist = length(away);
			if (dist < 1.0f) {
				//print(away);
				vec3 perp = vec3(-away[2], 0.f, -away[0]);
				if (abs(dot(perp, move_vec)) < 0.001f) {
					away += MathUtils::randfUniform(-1, 1) * perp;
				}
				move_vec += move_dist * (0.5f / max(dist, 0.5f)) * normalize(away);
				move_vec = move_dist * normalize(move_vec);
			}
		}*/

		remaining_move_dist -= move_dist;

		model[3][0] += move_vec[0];
		model[3][2] += move_vec[2];
		vec3 forward = vec3(model[3][0] - oldModel[3][0], 0, model[3][2] - oldModel[3][2]);
		if (length(forward) > 0.01f) {
			forward = normalize(forward);
			vec3 right = cross(forward, vec3(0, 1, 0));
			model[0] = vec4(right, 0);
			model[2] = vec4(-forward, 0);
		}


		if (length(this->getPosition() - destNode->getPosition()) < 0.5f) { //reached destNode with this iteration
			PathNode* nextNode; //continue moving to the next node
			if (this->team->teamColor == RED_TEAM) nextNode = this->destNode->next_red;
			else nextNode = this->destNode->next_blue;

			if (nextNode != nullptr) this->destNode = nextNode;
			else {
				std::cout << "minion reached the end of the path!\n";
				doneMoving = true;
				actionState = ACTION_STATE_IDLE;
				break;
			}
		}
	}
}

void SuperMinion::dropPickups() {
	srand((unsigned int)time(NULL));
	vec3 pos = this->getPosition();
	if (rand() % SUPER_MINION_IRON_DROP_CHANCE == 0) {
		float x = pos.getX() + ((float)pow(-1, rand() % 2) * (((rand() % 100) / 100.0f) * DROP_RANGE));
		float z = pos.getZ() + ((float)pow(-1, rand() % 2) * (((rand() % 100) / 100.0f) * DROP_RANGE));
		manager->spawnEntity(IRON_TYPE, x, z, 0, nullptr);
	}
	if (rand() % SUPER_MINION_BOTTLE_DROP_CHANCE == 0) {
		float x = pos.getX() + ((float)pow(-1, rand() % 2) * (((rand() % 100) / 100.0f) * DROP_RANGE));
		float z = pos.getZ() + ((float)pow(-1, rand() % 2) * (((rand() % 100) / 100.0f) * DROP_RANGE));
		manager->spawnEntity(BOTTLE_TYPE, x, z, 0, nullptr);
	}
}

void SuperMinion::setEntData(EntityData data) {
	Minion::setEntData(data);
	//std::cout << "superminion " << id << " x: " << this->getPosition().getX() << " z: " << this->getPosition().getZ() << "\n";
}