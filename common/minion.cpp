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
		team->decMinion(); 
		ObjectDetection::removeObject(this);
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
		mat4 bestModel = model;
		float collisionDistances = -1;
		int numCollisions = -1;
		bool improvement = false;


		auto other = ObjectDetection::getNearestObject(this, DETECTION_FLAG_COLLIDABLE, 1);
		if (other) {
			vec3 away = getPosition() - other->getPosition();
			if (length(away) < 1.0f) {
				print(away);
				float noise = 0.25f * length(away);
				away += noise * MathUtils::randfUniform(-1, 1) * vec3(-away[2], 0.f, -away[0]);
				move_vec += move_dist * 0.5f * normalize(away);
			}
		}

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

void Minion::setEntData(EntityData data) {
	Entity::setEntData(data);
	//if(actionState != ACTION_STATE_IDLE) std::cout << "minion " << id << " actionState: " << (int)actionState << "\n";
}

/* TESTING SPECIFIC FUNCTIONALITY - DO NOT USE */
void Minion::setAttackTarget(Entity* e) { attackTarget = e; attackTargetID = e->getID(); }