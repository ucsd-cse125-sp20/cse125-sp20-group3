#include "lasertower_client.h"
#include "../client/src/AudioManager.h"

LaserTower_Client::LaserTower_Client(GameObjectData data, int id, Team* t, SceneManager_Server* sm_server, GLTFGeode* geode, ParticleSystemGeode* laser, Transform* parent) : LaserTower(data, id, t, sm_server)
{
	this->laser = laser;

	laserTransform = conf_new(TimedTransform, mat4::translation(LASER_TOWER_BEAM_OFFSET));
	laserTransform->addChild(laser);

	parent->addChild(laserTransform);
	parent->addChild(geode);
}

LaserTower_Client::~LaserTower_Client()
{
	conf_delete(laserTransform);
}

void LaserTower_Client::activate(vec3 target)
{
	((ParticleSystem*)laser->obj)->reset(0, 1);
	
	vec3 laserPosition = this->getPosition() + LASER_TOWER_BEAM_OFFSET;
	laserTransform->setPositionDirection(LASER_TOWER_BEAM_OFFSET, target - laserPosition);
	mat4 currMat = this->getMatrix();
	mat4 inverseMat = inverse(currMat);
	laserTransform->setMatrix(inverseMat * mat4::translation(this->getPosition()) * laserTransform->getMatrix());

	laserTransform->activate(LASER_TOWER_BEAM_TIMEOUT);
	AudioManager::playAudioSource(this->getPosition(), "laser");
}