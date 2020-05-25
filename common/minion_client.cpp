#include "minion_client.h"

std::vector<ParticleSystemGeode*> Minion_Client::geodes = std::vector<ParticleSystemGeode*>();

namespace {
	const char* smallMinionActions[2] = { "Walking", "Fighting" };
}

Minion_Client::Minion_Client(int id, Team* t, SceneManager_Server* sm, Renderer* renderer, OzzGeode* geode, Transform* parent, RootSignature* root) : Minion(id, t, sm)
{
	ParticleSystem::ParticleSystemParams params = {};
	params.spriteFile = MINION_BULLET_SPRITE;
	params.numParticles = 10;
	params.life = 0.5f;
	params.initializer = [](ParticleSystem::ParticleData* pd, ParticleSystem::ParticleAuxData* pad) {
		float r = MathUtils::randfUniform(0, 0.05f);
		float a = MathUtils::randfUniform(0, 2 * PI);
		float z = 0;

		pd->position = float3(r * cos(a), r * sin(a), z);
		pd->color = float4(1.0f, 1.0f, 0.5f, 1.0f);
		pd->scale = float2(0.2f, 0.05f);
		pad->velocity = float3(0.f, 0.f, MINION_BULLET_SPEED);
	};
	bullets = conf_new(ParticleSystemGeode, renderer, params);
	Minion_Client::geodes.push_back(bullets);

	bulletTransform = conf_new(TimedTransform, mat4::rotationY(PI) * mat4::translation(vec3(0.f, 0.25f, 0.05f)));
	bulletTransform1 = conf_new(Transform, mat4::translation(MINION_BULLET_OFFSET1));
	bulletTransform2 = conf_new(Transform, mat4::translation(MINION_BULLET_OFFSET2));
	bulletTransform->addChild(bulletTransform1);
	bulletTransform->addChild(bulletTransform2);
	bulletTransform1->addChild(bullets);
	bulletTransform2->addChild(bullets);

	animator = conf_new(Animator, geode);
	animator->SetClip(smallMinionActions[0]);
	parent->addChild(animator);
	parent->addChild(bulletTransform);

	bullets->createSpriteResources(root);
}

Minion_Client::~Minion_Client()
{
	Minion_Client::geodes.erase(std::remove(Minion_Client::geodes.begin(), Minion_Client::geodes.end(), bullets), Minion_Client::geodes.end());
	conf_delete(bulletTransform);
	conf_delete(bulletTransform1);
	conf_delete(bulletTransform2);
	conf_delete(bullets);

	conf_delete(animator);
}

void Minion_Client::setProgram(Geode::GeodeShaderDesc shader)
{
	for (auto ps : geodes) {
		ps->setProgram(shader);
	}
}

void Minion_Client::shoot()
{
	bulletTransform->activate(MINION_BULLET_TIMEOUT);
	animator->SetClip(smallMinionActions[1]);
}

void Minion_Client::kill()
{
}
