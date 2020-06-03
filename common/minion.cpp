#include "minion.h"
#include "../server/SceneManager_Server.h"

Minion::Minion(GameObjectData data, int id, Team* t, SceneManager_Server* sm) : Entity(data, id, MINION_HEALTH, MINION_ATTACK, t, sm) {
	attackRange = MINION_ATK_RANGE;
	attackInterval = MINION_ATK_INTERVAL;
	velocity = MINION_VELOCITY;
	doneMoving = false;

	if (sm != nullptr) { //only execute on server
		t->incMinion();

		destNode = (PathNode*)ObjectDetection::getNearestObject(this, DETECTION_FLAG_PATH_NODE, 10);

		int flags = DETECTION_FLAG_ENTITY | DETECTION_FLAG_COLLIDABLE | DETECTION_FLAG_MINION | 
					DETECTION_FLAG_MINION_TARGET | DETECTION_FLAG_LASER_TARGET;
		if (t->teamColor == RED_TEAM) flags = flags | DETECTION_FLAG_RED_TEAM;
		else flags = flags | DETECTION_FLAG_BLUE_TEAM;
		ObjectDetection::addObject(this, flags, -MINION_WIDTH, MINION_WIDTH, -MINION_LENGTH, MINION_LENGTH);
	}
}

Minion::Minion(GameObjectData data, int id, int health, int attack, int range, float interval, float vel, Team* t, SceneManager_Server* sm) : Entity(data, id, health, attack, t, sm) {
	attackRange = range;
	attackInterval = interval;
	velocity = vel;
	doneMoving = false;

	if (sm != nullptr) { //only execute on server
		t->incMinion();

		destNode = (PathNode*)ObjectDetection::getNearestObject(this, DETECTION_FLAG_PATH_NODE, 10);
	}
}

void Minion::update(float deltaTime) { //should they be able to switch attack targets instantaneously?

	if (attackTarget != nullptr &&																//first, if targeting something
			(!manager->checkEntityAlive(attackTargetID) ||										//but either target is dead
			length(attackTarget->getPosition() - this->getPosition()) > this->attackRange)) {	//or target is out of range, null out ptr
		std::cout << "minion " << id << " nulling out attackTarget\n";
		attackTarget = nullptr; //do this check here instead of after attacking in the case of multiple entities targeting one entity
	}

	if (this->attackTarget == nullptr) { //next, if not currently targeting something, check if there is a valid enemy in range
		int flags = DETECTION_FLAG_MINION_TARGET;
		if (this->team->teamColor == RED_TEAM) flags = flags | DETECTION_FLAG_BLUE_TEAM;
		else flags = flags | DETECTION_FLAG_RED_TEAM;
		attackTarget = (Entity*)ObjectDetection::getNearestObject(this, flags, attackRange);

		if (attackTarget != nullptr && length(attackTarget->getPosition() - this->getPosition()) > this->attackRange) {
			attackTarget = nullptr; //if target isn't actually in range, ignore it
			//std::cout << "minion " << id_str << " ignoring target out of range\n";
		} //object detection doesn't strictly follow distance, based on hashed block sections

		if (attackTarget != nullptr) {
			attackTargetID = attackTarget->getID();
			timeElapsed = 0; //reset attack timer on acquiring new target
			std::cout << "minion " << id<< " found target " << attackTarget->getID() << "\n";
		}
	}

	if (attackTarget != nullptr) { //if this minion should be attacking something, don't move
		timeElapsed += deltaTime; //increase timeElapsed
		actionState = ACTION_STATE_ATTACK;

		//first face the attack target, regardless of timeElapsed
		vec3 forward = normalize(attackTarget->getPosition() - this->getPosition());
		vec3 right = cross(forward, vec3(0, 1, 0));
		model[0] = vec4(right, 0);
		model[2] = vec4(-forward, 0);

		if (timeElapsed >= attackInterval) { //only attack on an interval
			std::cout << "minion: " << id << " attacking that " << attackTargetID << "\n";
			this->attack();
			timeElapsed = 0;
		}
	}
	else { //no attack target after all checks, move
		this->move(deltaTime);
	}

	ObjectDetection::updateObject(this);
}

void Minion::takeDamage(int damage) {
	Entity::takeDamage(damage);
	std::cout << "minion: " << id << " took " << damage << " damage | remaining health: " << health << "\n";
	if (health <= 0) {
		std::cout << "minion " << id << " dying\n";
		this->dropPickups();
		team->decMinion(); 
		ObjectDetection::removeObject(this);
	}
}

void Minion::dropPickups() {
	srand((unsigned int)time(NULL));
	vec3 pos = this->getPosition();
	if (rand() % MINION_IRON_DROP_CHANCE == 0) {
		float x = pos.getX() + (((rand() % 100) / 100.0f) * DROP_RANGE);
		float z = pos.getZ() + (((rand() % 100) / 100.0f) * DROP_RANGE);
		manager->spawnEntity(IRON_TYPE, x, z, 0, nullptr);
	}
	if (rand() % MINION_BOTTLE_DROP_CHANCE == 0) {
		float x = pos.getX() + (((rand() % 100) / 100.0f) * DROP_RANGE);
		float z = pos.getZ() + (((rand() % 100) / 100.0f) * DROP_RANGE);
		manager->spawnEntity(BOTTLE_TYPE, x, z, 0, nullptr);
	}
}

void Minion::attack() {
	attackTarget->takeDamage(this->attackDamage);
	actionState = ACTION_STATE_FIRE;
	//TODO manipulate necessary data to spawn particle systems
}

void Minion::move(float deltaTime) {
	if (doneMoving) return; //small optimization

	actionState = ACTION_STATE_MOVE;

	float remaining_move_dist = velocity * deltaTime; //full movement distance this minion travels this tick

	while (remaining_move_dist > 0) { //while this minion still has distance to move
		vec3 dest_vec = destNode->getPosition() - this->getPosition(); //vector to reach destNode
		float dest_dist = length(dest_vec); //distance between this and destNode
		float move_dist = std::min(remaining_move_dist, dest_dist); //this iteration, move full move_dist, or only to destNode if closer
		vec3 move_vec = move_dist > 0 ? normalize(dest_vec) * move_dist : vec3(0); //move calculated distance along vector to destination

		int move_attempts = 0;
		mat4 oldModel = model;
		while (move_attempts < MINION_MOVE_ATTEMPTS) { //attempt to move along dest_vec and check for collisions. if collision, adjust and try again
			move_attempts++;

			model[3][0] += move_vec[0];
			model[3][2] += move_vec[2];
			vec3 forward = normalize(vec3(model[3][0] - oldModel[3][0], 0, model[3][2] - oldModel[3][2]));
			vec3 right = cross(forward, vec3(0, 1, 0));
			model[0] = vec4(right, 0);
			model[2] = vec4(-forward, 0);

			std::vector<GameObject*> collisions = ObjectDetection::getCollisions(this, DETECTION_FLAG_COLLIDABLE);
			if (collisions.size() > 0) {
				//std::cout << "minion " << id << " detected collision on move_attempt " << move_attempts << "\n";
				if (move_attempts != MINION_MOVE_ATTEMPTS) this->model = oldModel;
				vec4 move_vec4 = vec4(move_vec, 0);
				float rot_degrees = (float)pow(-1, move_attempts - 1) * ((float)move_attempts * 20); //20, -40, 60, -80, 100, -120, last one doesn't matter
				move_vec = (mat4::rotationY(degToRad(rot_degrees)) * move_vec4).getXYZ();
			}
			else break;

			if (move_attempts == MINION_MOVE_ATTEMPTS) {
				remaining_move_dist = 0;
			}
		}

		remaining_move_dist -= move_dist;

		if (this->getPosition() == destNode->getPosition()) { //reached destNode with this iteration
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

void Minion::setEntData(EntityData data) {
	Entity::setEntData(data);
	//if(actionState != ACTION_STATE_IDLE) std::cout << "minion " << id << " actionState: " << (int)actionState << "\n";
}

/* TESTING SPECIFIC FUNCTIONALITY - DO NOT USE */
void Minion::setAttackTarget(Entity* e) { attackTarget = e; attackTargetID = e->getID(); }