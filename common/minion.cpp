#include "minion.h"
#include "../server/SceneManager_Server.h"

Minion::Minion(int id, Team* t, SceneManager_Server* sm) : Entity(id, MINION_HEALTH, MINION_ATTACK, t, sm) {
	actionState = MINION_ACTION_IDLE;

	attackRange = MINION_ATK_RANGE;
	attackInterval = MINION_ATK_INTERVAL;
	velocity = MINION_VELOCITY;
	doneMoving = false;

	if (sm != nullptr) { //only execute on server
		destNode = (mapNode*)ObjectDetection::getNearestObject(this, DETECTION_FLAG_MAP_NODE, 50); //TODO unbounded radius check

		int flags = DETECTION_FLAG_ENTITY | DETECTION_FLAG_COLLIDABLE | DETECTION_FLAG_MINION | 
					DETECTION_FLAG_MINION_TARGET | DETECTION_FLAG_LASER_TARGET;
		if (t->teamColor == RED_TEAM) flags = flags | DETECTION_FLAG_RED_TEAM;
		else flags = flags | DETECTION_FLAG_BLUE_TEAM;
		ObjectDetection::addObject(this, flags);
	}
}

Minion::Minion(int id, int health, int attack, int range, float interval, float vel, Team* t, SceneManager_Server* sm) : Entity(id, health, attack, t, sm) {
	actionState = MINION_ACTION_IDLE;
	
	attackRange = range;
	attackInterval = interval;
	velocity = vel;
	doneMoving = false;

	if (sm != nullptr) { //only execute on server
		destNode = (mapNode*)ObjectDetection::getNearestObject(this, DETECTION_FLAG_MAP_NODE, 50); //TODO unbounded radius check

		int flags = DETECTION_FLAG_ENTITY | DETECTION_FLAG_COLLIDABLE | DETECTION_FLAG_MINION |
					DETECTION_FLAG_MINION_TARGET | DETECTION_FLAG_LASER_TARGET;
		if (t->teamColor == RED_TEAM) flags = flags | DETECTION_FLAG_RED_TEAM;
		else flags = flags | DETECTION_FLAG_BLUE_TEAM;
		ObjectDetection::addObject(this, flags);
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
		actionState = MINION_ACTION_ATTACK;

		//first face the attack target, regardless of timeElapsed
		vec3 forward = normalize(attackTarget->getPosition() - this->getPosition()); //TODO check vectors?
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
	if (health <= 0) { team->decMinion(); std::cout << "i die\n"; }
}

void Minion::attack() {
	attackTarget->takeDamage(this->attackDamage);
	actionState = MINION_ACTION_FIRE;
	//TODO manipulate necessary data to spawn particle systems
}

void Minion::move(float deltaTime) {
	if (doneMoving) return; //small optimization

	actionState = MINION_ACTION_MOVE;

	float remaining_move_dist = velocity * deltaTime; //full movement distance this minion travels this tick

	while (remaining_move_dist > 0) { //while this minion still has distance to move
		vec3 dest_vec = destNode->getPosition() - this->getPosition(); //vector to reach destNode
		float dest_dist = length(dest_vec); //distance between this and destNode
		float move_dist = std::min(remaining_move_dist, dest_dist); //this iteration, move full move_dist, or only to destNode if closer

		vec3 move_vec = move_dist > 0 ? normalize(dest_vec) * move_dist : vec3(0); //move calculated distance along vector to destination

		float lastXPos = model[3][0];
		float lastZPos = model[3][2];
		model[3][0] += move_vec[0];
		model[3][2] += move_vec[2];
		//if (doneMoving) std::cout << "x: " << model[3][0] << " z: " << model[3][2] << "\n";
		remaining_move_dist -= move_dist; //decrease remaining movement distance
		vec3 forward = normalize(vec3(model[3][0] - lastXPos, 0, model[3][2] - lastZPos)); //TODO check vectors?
		vec3 right = cross(forward, vec3(0, 1, 0));
		model[0] = vec4(right, 0);
		model[2] = vec4(-forward, 0);

		if (this->getPosition() == destNode->getPosition()) { //reached destNode with this iteration
			mapNode* nextNode; //continue moving to the next node
			if (this->team->teamColor == RED_TEAM) nextNode = this->destNode->next_red;
			else nextNode = this->destNode->next_blue;

			if (nextNode != nullptr) this->destNode = nextNode; 
			else {
				std::cout << "minion reached the end of the path!\n";
				doneMoving = true;
				actionState = MINION_ACTION_IDLE;
				break;
			}
		}
	}
}

void Minion::setEntData(EntityData data) {
	Entity::setEntData(data);
	std::cout << "minion " << id << " targetID: " << attackTargetID << "\n";
}

/* TESTING SPECIFIC FUNCTIONALITY - DO NOT USE */
void Minion::setAttackTarget(Entity* e) { attackTarget = e; attackTargetID = e->getID(); }