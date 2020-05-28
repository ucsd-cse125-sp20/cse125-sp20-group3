#include "lasertower_client.h"

std::vector<ParticleSystemGeode*> LaserTower_Client::geodes = std::vector<ParticleSystemGeode*>();

//pass in gltfGeode and position transform as parent
LaserTower_Client::LaserTower_Client(GameObjectData data, int id, Team* t, SceneManager_Server* sm_server, Renderer* renderer) : LaserTower(data, id, t, sm_server)
{
	ParticleSystem::ParticleSystemParams params = {};
	params.spriteFile = LASER_TOWER_BEAM_SPRITE;
	params.numParticles = MAX_PARTICLES;
	params.life = 1;
	laser = conf_new(ParticleSystemGeode, renderer, params);
	LaserTower_Client::geodes.push_back(laser);

	laserTransform = conf_new(TimedTransform, mat4::translation(LASER_TOWER_BEAM_OFFSET));
	laserTransform->addChild(laser);

	//parent gltfGeode
	//parent laserTransform
}

LaserTower_Client::~LaserTower_Client()
{
	//printf("ASDFASDFASDF\n");
	LaserTower_Client::geodes.erase(std::remove(LaserTower_Client::geodes.begin(), LaserTower_Client::geodes.end(), laser), LaserTower_Client::geodes.end());
	conf_delete(laserTransform);
	conf_delete(laser);
}

void LaserTower_Client::setProgram(Geode::GeodeShaderDesc shader)
{
	for (auto ps : geodes) {
		ps->setProgram(shader);
	}
}

void LaserTower_Client::activate(vec3 target)
{
	vec3 laserPosition = this->getPosition() + LASER_TOWER_BEAM_OFFSET;
	ParticleSystem::ParticleInitializer initializer = [](ParticleSystem::ParticleData* pd, ParticleSystem::ParticleAuxData* pad) {
		float r = MathUtils::randfUniform(0, 0.1f);
		float a = MathUtils::randfUniform(0, 2 * PI);
		float z = 0;

		pd->position = float3(r * cos(a), r * sin(a), z);
		pd->color = float4(0.8f, 0.8f, 1.0f, 1.0f);
		pd->scale = float2(0.2f, 0.2f);
		pad->velocity = float3(0.f, 0.f, LASER_TOWER_BEAM_SPEED);
	};
	((ParticleSystem*)laser->obj)->setInitilizer(initializer);
	((ParticleSystem*)laser->obj)->setLife(length(laserPosition - target) / LASER_TOWER_BEAM_SPEED);
	((ParticleSystem*)laser->obj)->reset(0, 1);
	//printf("%f\n", (float)length(laserPosition - target));
	
	
	laserTransform->setPositionDirection(LASER_TOWER_BEAM_OFFSET, target - laserPosition);
	//print(target - laserPosition);
	//print(laserTransform->getMatrix());
	mat4 currMat = this->getMatrix();
	mat4 inverseMat = inverse(currMat);
	laserTransform->setMatrix(inverseMat * mat4::translation(this->getPosition()) * laserTransform->getMatrix());

	laserTransform->activate(LASER_TOWER_BEAM_TIMEOUT);
}