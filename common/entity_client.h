#pragma once

class SceneManager_Client;

class Entity_Client {
protected:
	SceneManager_Client* sm_c;

public:
	Entity_Client(SceneManager_Client* sm) { sm_c = sm; }

	virtual void updateAnimParticles() {}
	void processAction(char actionState) {
		switch (actionState) {
		case ACTION_STATE_IDLE:
			this->idleAction();
			break;
		case ACTION_STATE_MOVE:
			this->moveAction();
			break;
		case ACTION_STATE_ATTACK:
			this->attackAction();
			break;
		case ACTION_STATE_FIRE:
			this->fireAction();
			break;
		default:
			break;
		}
	}
	virtual void idleAction() { std::cout << "entity_client idle action\n"; }
	virtual void moveAction() { std::cout << "entity_client move action\n"; }
	virtual void attackAction() { std::cout << "entity_client attack action\n"; }
	virtual void fireAction() { std::cout << "entity_client fire action\n"; }

	virtual void die() { std::cout << "entity_client die\n"; } //override this to play death animations
};