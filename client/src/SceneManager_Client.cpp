#include "SceneManager_Client.h"
#include "Application.h"

namespace {
	// Models
	const char* mapFile = "map.gltf";
	const char* playerFile = "char-1-female.gltf";
	const char* groundFile = "map.gltf";
	const char* minionFile = "minion-retry.gltf";
	const char* superMinionFile = "minion-2-super.gltf";
	const char* laserTowerFile = "tower-1-laser.gltf";
	const char* clawTowerFile = "tower-3-claw-machine.gltf";
	const char* dumpsterFile = "resource-1-dumpster.gltf";
	const char* recyclingBinFile = "resource-2-recycling-bin.gltf";

	// Animations
	const char* smallMinionDir = "small-minion-B";
	const char* smallMinionActions[3] = { "Walking", "Fighting", "Death" };
	const char* superMinionDir = "super-minion-A";
	const char* superMinionActions[3] = { "Walking", "Fighting", "Death" };
	const char* playerMaleDir = "male-char";
	const char* playerMaleActions[2] = { "Idle", "Walking" };
	const char* playerFemaleDir = "female-char";
	const char* playerFemaleActions[2] = { "Idle", "Walking" };

	int counter = 1;
	int animCounter = 1;
}

bool SceneManager_Client::enableCulling = true;

SceneManager_Client::SceneManager_Client(Renderer* renderer)
{
	this->renderer = renderer;
	// It'd be nice if I could put this in a loop later
	gltfGeodes[ENV_GEODE] = conf_new(GLTFGeode, renderer, mapFile); //ok
	gltfGeodes[PLAYER_GEODE] = conf_new(GLTFGeode, renderer, playerFile);
	//gltfGeodes[BASE_GEODE] = conf_new(GLTFGeode, renderer, baseFile);
	gltfGeodes[MINION_GEODE] = conf_new(GLTFGeode, renderer, minionFile);
	gltfGeodes[SUPER_MINION_GEODE] = conf_new(GLTFGeode, renderer, superMinionFile);
	gltfGeodes[LASER_TOWER_GEODE] = conf_new(GLTFGeode, renderer, laserTowerFile); //ok
	gltfGeodes[CLAW_TOWER_GEODE] = conf_new(GLTFGeode, renderer, clawTowerFile);
	gltfGeodes[DUMPSTER_GEODE] = conf_new(GLTFGeode, renderer, dumpsterFile); //ok
	gltfGeodes[RECYCLING_BIN_GEODE] = conf_new(GLTFGeode, renderer, recyclingBinFile); //ok

	ozzGeodes[MINION_GEODE] = conf_new(OzzGeode, renderer, smallMinionDir);
	((OzzObject*)ozzGeodes[MINION_GEODE]->obj)->SetClip(smallMinionActions[0]); // Set a default action
	ozzGeodes[PLAYER_GEODE] = conf_new(OzzGeode, renderer, playerMaleDir);
	((OzzObject*)ozzGeodes[PLAYER_GEODE]->obj)->SetClip(playerMaleActions[0]); // Set a default action


	ParticleSystem::ParticleSystemParams particleParams = {};
	particleParams.spriteFile = LASER_TOWER_BEAM_SPRITE;
	particleParams.numParticles = MAX_PARTICLES;
	particleParams.life = 20;
	particleParams.initializer = [](ParticleSystem::ParticleData* pd, ParticleSystem::ParticleAuxData* pad) {
		float r = MathUtils::randfUniform(0, 0.1f);
		float a = MathUtils::randfUniform(0, 2 * PI);
		float z = 0;

		pd->position = float3(r * cos(a), r * sin(a), z);
		pd->color = float4(0.8f, 0.8f, 1.0f, 1.0f);
		pd->scale = float2(0.2f, 0.2f);
		pad->velocity = float3(0.f, 0.f, LASER_TOWER_BEAM_SPEED);
	};
	particleGeodes[LASER_TOWER_GEODE] = conf_new(ParticleSystemGeode, renderer, particleParams);


	particleParams = {};
	particleParams.spriteFile = MINION_BULLET_SPRITE;
	particleParams.numParticles = 10;
	particleParams.life = 0.5f;
	particleParams.initializer = [](ParticleSystem::ParticleData* pd, ParticleSystem::ParticleAuxData* pad) {
		float r = MathUtils::randfUniform(0, 0.05f);
		float a = MathUtils::randfUniform(0, 2 * PI);
		float z = 0;

		pd->position = float3(r * cos(a), r * sin(a), z);
		pd->color = float4(1.0f, 1.0f, 0.5f, 1.0f);
		pd->scale = float2(0.2f, 0.05f);
		pad->velocity = float3(0.f, 0.f, MINION_BULLET_SPEED);
	};
	particleGeodes[MINION_GEODE] = conf_new(ParticleSystemGeode, renderer, particleParams);

	///////////////////////////////////////////////////////////////////////////////
	
    // TODO This is a hard coded animation example. Remove this later (SMALL MINION)
	int key = 9999999;
    ozzGeodes["blarf"] = conf_new(OzzGeode, renderer, smallMinionDir);
	((OzzObject*)ozzGeodes["blarf"]->obj)->SetClip(smallMinionActions[2]);
    Transform* t = conf_new(Transform, mat4::translation(vec3(-1, 0, 2)));
    Animator* a = conf_new(Animator, ozzGeodes["blarf"]);
    a->SetClip(smallMinionActions[2]);
    t->addChild(a);
    this->addChild(t);
    transforms[key] = t;
    animators[key] = a;


	// TODO This is a hard coded animation example. Remove this later (SUPER MINION)
	key = 8888888;
	ozzGeodes["blarf2"] = conf_new(OzzGeode, renderer, superMinionDir);
	((OzzObject*)ozzGeodes["blarf2"]->obj)->SetClip(superMinionActions[0]);
	t = conf_new(Transform, mat4::translation(vec3(1, 0, 2)));
	a = conf_new(Animator, ozzGeodes["blarf2"]);
	a->SetClip(superMinionActions[0]);
	t->addChild(a);
	this->addChild(t);
	transforms[key] = t;
	animators[key] = a;

	//// TODO This is a hard coded animation example. Remove this later (MALE CHAR)
	//key = 4672347;
	//ozzGeodes["blarf3"] = conf_new(OzzGeode, renderer, playerMaleDir);
	//((OzzObject*)ozzGeodes["blarf3"]->obj)->SetClip(playerMaleActions[1]);
	//t = conf_new(Transform, mat4::translation(vec3(3, 0, 2)));
	//a = conf_new(Animator, ozzGeodes["blarf3"]);
	//a->SetClip(playerMaleActions[1]);
	//t->addChild(a);
	//this->addChild(t);
	//transforms[key] = t;
	//animators[key] = a;

	//// TODO This is a hard coded animation example. Remove this later (FEMALE CHAR)
	//key = 1734813;
	//ozzGeodes["blarf4"] = conf_new(OzzGeode, renderer, playerFemaleDir);
	//((OzzObject*)ozzGeodes["blarf4"]->obj)->SetClip(playerFemaleActions[1]);
	//t = conf_new(Transform, mat4::translation(vec3(4, 0, 2)));
	//a = conf_new(Animator, ozzGeodes["blarf4"]);
	//a->SetClip(playerFemaleActions[1]);
	//t->addChild(a);
	//this->addChild(t);
	//transforms[key] = t;
	//animators[key] = a;

	//// TODO These are hard coded particle examples. Remove them later
	//key = 7234813;
	//ParticleSystem::ParticleSystemParams params = {};
	//params.spriteFile = "LaserParticle.png";
	//params.initializer = [](ParticleSystem::ParticleData* pd, ParticleSystem::ParticleAuxData* pad) {
	//	float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX / 0.1f);
	//	float a = static_cast <float> (rand()) / static_cast <float> (RAND_MAX / (2 * PI));
	//	float z = 0;

	//	pd->position = float3(r * cos(a), z, r * sin(a));
	//	pd->color = float4(0.8f, 0.8f, 1.0f, 1.0f);
	//	pd->scale = float2(0.2f, 0.2f);
	//	pad->velocity = float3(0, 5, 0);
	//};
	//params.numParticles = 1000;
	//params.life = 1.0f;
	//particleGeodes["blarf5"] = conf_new(ParticleSystemGeode, renderer, params);
	//t = conf_new(Transform, mat4::translation(vec3(-2, 0, 4)));
	//t->addChild(particleGeodes["blarf5"]);
	//this->addChild(t);
	//transforms[key] = t;

	//key = 4201387;
	//params.initializer = [](ParticleSystem::ParticleData* pd, ParticleSystem::ParticleAuxData* pad) {
	//	float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX / 0.1f);
	//	float a = static_cast <float> (rand()) / static_cast <float> (RAND_MAX / (2 * PI));
	//	float z = static_cast <float> (rand()) / static_cast <float> (RAND_MAX / 5);

	//	pd->position = float3(r * cos(a), z, r * sin(a));
	//	pd->color = float4(0.8f, 0.8f, 1.0f, 1.0f);
	//	pd->scale = float2(0.2f, 0.2f);
	//	pad->velocity = float3(cos(a), 0.0f, sin(a));
	//};
	//params.updater = [](ParticleSystem::ParticleData* pd, ParticleSystem::ParticleAuxData* pad, float deltaTime) {
	//	pd->scale *= 0.95f;
	//};
	//particleGeodes["blarf6"] = conf_new(ParticleSystemGeode, renderer, params);
	//t = conf_new(Transform, mat4::translation(vec3(0, 0, 4)));
	//t->addChild(particleGeodes["blarf6"]);
	//this->addChild(t);
	//transforms[key] = t;

	//key = 6672073;
	//params.initializer = [](ParticleSystem::ParticleData* pd, ParticleSystem::ParticleAuxData* pad) {
	//	float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX / 0.1f);
	//	float a = static_cast <float> (rand()) / static_cast <float> (RAND_MAX / (2 * PI));
	//	float z = static_cast <float> (rand()) / static_cast <float> (RAND_MAX / 5);

	//	pd->position = float3(r * cos(a), z, r * sin(a));
	//	pd->color = float4(0.8f, 0.8f, 1.0f, 1.0f);
	//	pd->scale = float2(0.2f, 0.2f);
	//	pad->velocity = float3(cos(a), 0.0f, sin(a));
	//};
	//params.updater = [](ParticleSystem::ParticleData* pd, ParticleSystem::ParticleAuxData* pad, float deltaTime) {
	//	pad->velocity += float3(-pd->position.x, 0.0f, -pd->position.z);
	//};
	//particleGeodes["blarf7"] = conf_new(ParticleSystemGeode, renderer, params);
	//t = conf_new(Transform, mat4::translation(vec3(2, 0, 4)));
	//t->addChild(particleGeodes["blarf7"]);
	//this->addChild(t);
	//transforms[key] = t;

	trackedPlayer_ID = NO_TRACKED_PLAYER;

	transforms[GROUND_KEY] = conf_new(Transform, mat4::identity());
	transforms[GROUND_KEY]->addChild(gltfGeodes[ENV_GEODE]);
	this->addChild(transforms[GROUND_KEY]);

	red_team = new Team(RED_TEAM);
	blue_team = new Team(BLUE_TEAM);
}

SceneManager_Client::~SceneManager_Client()
{
	//printf("ASDFASDFASDF\n");
	for (std::pair<int, Entity*> e : entityMap) conf_delete(e.second);
	for (std::pair<int, Player_Client*> p : playerMap) conf_delete(p.second);
	for (std::pair<int, Minion_Client*> m : minionMap) conf_delete(m.second);
	//super_minion
	for (std::pair<int, LaserTower_Client*> t : laserTowerMap) conf_delete(t.second);
	//claw_tower
	//dumpster
	//recycling_bin
	for (std::pair<int, Transform*> t : transforms) conf_delete(t.second);
	for (std::pair<int, Animator*> t : animators) conf_delete(t.second);
	for (std::pair<std::string, GLTFGeode*> g : gltfGeodes) conf_delete(g.second);
	for (std::pair<std::string, OzzGeode*> g : ozzGeodes) conf_delete(g.second);
	for (std::pair<std::string, ParticleSystemGeode*> g : particleGeodes) conf_delete(g.second);

	for (Transform* a : otherTransforms) conf_delete(a);
}

void SceneManager_Client::createMaterialResources(SceneManager_Client::GeodeType type, RootSignature* pRootSignature, DescriptorSet* pBindlessTexturesSamplersSet, Sampler* defaultSampler)
{
	switch (type) {
	case SceneManager_Client::GeodeType::MESH:
		for (auto ge : gltfGeodes) {
			ge.second->createMaterialResources(pRootSignature, pBindlessTexturesSamplersSet, defaultSampler);
		}
		break;
	case SceneManager_Client::GeodeType::ANIMATED_MESH:
		for (auto ge : ozzGeodes) {
			ge.second->createMaterialResources(pRootSignature, pBindlessTexturesSamplersSet, defaultSampler);
		}
		break;
	case SceneManager_Client::GeodeType::PARTICLES:
		for (auto ge : particleGeodes) {
			ge.second->createSpriteResources(pRootSignature);
		}
		particleRootSignature = (pRootSignature);
		break;
	}
}

void SceneManager_Client::updateStateAndScene(Client::UpData data) {
	this->updateState(data.stateUpdate);
	this->updateScene(data.sceneUpdate);
}

void SceneManager_Client::updateState(Client::StateUpdateData updateData) {
	red_team->setData(updateData.redTeamData);
	SceneManager_Client::updateUI(red_team);
	blue_team->setData(updateData.blueTeamData);
	SceneManager_Client::updateUI(blue_team);
}

void SceneManager_Client::updateUI(Team * teamColor){
	int plasticCount = teamColor->getPlasticCount();
	int metalCount = teamColor->getMetalCount();
	UIUtils::editText(PLASTIC_UI_TEXT, std::to_string(plasticCount), "small font", 0xff6655ff);
	UIUtils::editText(METAL_UI_TEXT, std::to_string(metalCount), "small font", 0xff6655ff);
	if (plasticCount <= 0){
		UIUtils::changeImage(PLASTIC_UI_ICON, "resource_plastic_alert.png",  float2((float)Application::width / 3200, (float)Application::height / 2000));
	}else{
		UIUtils::changeImage(PLASTIC_UI_ICON, "resource_plastic.png",  float2((float)Application::width / 3200, (float)Application::height / 2000));
	}
	if (metalCount <= 0){
		UIUtils::changeImage(METAL_UI_ICON, "resource_metal_alert.png",  float2((float)Application::width / 3200, (float)Application::height / 2000));
	}else{
		UIUtils::changeImage(METAL_UI_ICON, "resource_metal.png",  float2((float)Application::width / 3200, (float)Application::height / 2000));
	}

	bool super_minion = teamColor->checkResources(SUPER_MINION_TYPE);
	bool claw_machine = teamColor->checkResources(CLAW_TYPE);
	bool laser_tower = teamColor->checkResources(LASER_TYPE);

	if (laser_tower){
		UIUtils::changeImage(LASER_TOWER_UI_ICON, "tower_2.png",  float2((float)Application::width / 3200, (float)Application::height / 2000));
	}else{
		UIUtils::changeImage(LASER_TOWER_UI_ICON, "tower_2_low.png",  float2((float)Application::width / 3200, (float)Application::height / 2000));
	}

	if (claw_machine){
		UIUtils::changeImage(CLAW_MACHINE_UI_ICON, "tower_3.png",  float2((float)Application::width / 3200, (float)Application::height / 2000));
	}else{
		UIUtils::changeImage(CLAW_MACHINE_UI_ICON, "tower_3_low.png",  float2((float)Application::width / 3200, (float)Application::height / 2000));
	}

	if (super_minion){
		UIUtils::changeImage(SUPER_MINION_UI_ICON, "tower_4.png",  float2((float)Application::width / 3200, (float)Application::height / 2000));
	}else{
		UIUtils::changeImage(SUPER_MINION_UI_ICON, "tower_4_low.png",  float2((float)Application::width / 3200, (float)Application::height / 2000));
	}

}

void SceneManager_Client::updateScene(Client::SceneUpdateData updateData)
{
	//std::cout << "updating from client buf of size " << updateBuf.size() << "\n";
	for (Client::IDEntData data : updateData.entUpdates) {
		if (idMap.find(data.id) == idMap.end()) { //new id encountered, spawn new object
			GameObject::GameObjectData GO_data = data.ent_data.GO_data;
			Team* team;
			if (data.ent_data.teamColor == RED_TEAM) team = red_team;
			else if (data.ent_data.teamColor == BLUE_TEAM) team = blue_team;
			else team = nullptr;

			if (ID_PLAYER_MIN <= data.id && data.id <= ID_PLAYER_MAX) {
				std::cout << "creating new player, id: " << data.id << "\n";

				transforms[data.id] = conf_new(Transform, mat4::identity());
				Transform* adjustment = conf_new(Transform, mat4::rotationY(-PI / 2));
				transforms[data.id]->addChild(adjustment);

				otherTransforms.push_back(adjustment); //save to be deleted upon closing

				idMap[data.id] = std::make_pair(++subid, EntityType::PLAYER);
				playerMap[subid] = conf_new(Player_Client, GO_data, data.id, team, nullptr, ozzGeodes[PLAYER_GEODE], adjustment);
			}
			else if (ID_BASE_MIN <= data.id && data.id <= ID_BASE_MAX) {
				//idMap[id_str] = new Base();
			}
			else if (ID_MINION_MIN <= data.id && data.id <= ID_MINION_MAX) {
				std::cout << "creating new minion, id: " << data.id << "\n";
				transforms[data.id] = conf_new(Transform, mat4::identity());
				idMap[data.id] = std::make_pair(++subid, EntityType::MINION);
				minionMap[subid] = conf_new(Minion_Client, GO_data, data.id, team, nullptr, ozzGeodes[MINION_GEODE], particleGeodes[MINION_GEODE], transforms[data.id]);
			}
			else if (ID_SUPER_MINION_MIN <= data.id && data.id <= ID_SUPER_MINION_MAX) {
				std::cout << "creating new super minion, id: " << data.id << "\n";
				idMap[data.id] = std::make_pair(++subid, EntityType::OTHER);
				entityMap[subid] = conf_new(SuperMinion, GO_data, data.id, team, nullptr);
				transforms[data.id] = conf_new(Transform, mat4::identity());
				transforms[data.id]->addChild(gltfGeodes[SUPER_MINION_GEODE]);
			}
			else if (ID_LASER_MIN <= data.id && data.id <= ID_LASER_MAX) {
				std::cout << "creating new laser tower, id: " << data.id << "\n";
				idMap[data.id] = std::make_pair(++subid, EntityType::LASER_TOWER);
				transforms[data.id] = conf_new(Transform, mat4::identity());
				laserTowerMap[subid] = conf_new(LaserTower_Client, GO_data, data.id, team, nullptr, gltfGeodes[LASER_TOWER_GEODE], particleGeodes[LASER_TOWER_GEODE], transforms[data.id]);
			}
			else if (ID_CLAW_MIN <= data.id && data.id <= ID_CLAW_MAX) {
				std::cout << "creating new claw tower, id: " << data.id << "\n";
				idMap[data.id] = std::make_pair(++subid, EntityType::OTHER);
				entityMap[subid] = conf_new(ClawTower, GO_data, data.id, team, nullptr);
				transforms[data.id] = conf_new(Transform, mat4::identity());
				transforms[data.id]->addChild(gltfGeodes[CLAW_TOWER_GEODE]);
			}
			else if (ID_DUMPSTER_MIN <= data.id && data.id <= ID_DUMPSTER_MAX) {
				std::cout << "creating new dumpster, id: " << data.id << "\n";
				idMap[data.id] = std::make_pair(++subid, EntityType::OTHER);
				entityMap[subid] = conf_new(Resource, DUMPSTER_TYPE, GO_data, data.id, nullptr);
				transforms[data.id] = conf_new(Transform, mat4::identity());
				transforms[data.id]->addChild(gltfGeodes[DUMPSTER_GEODE]);
			}
			else if (ID_RECYCLING_BIN_MIN <= data.id && data.id <= ID_RECYCLING_BIN_MAX) {
				std::cout << "creating new recycling bin id: " << data.id << "\n";
				idMap[data.id] = std::make_pair(++subid, EntityType::OTHER);
				entityMap[subid] = conf_new(Resource, RECYCLING_BIN_TYPE, GO_data, data.id, nullptr);
				transforms[data.id] = conf_new(Transform, mat4::identity());
				transforms[data.id]->addChild(gltfGeodes[RECYCLING_BIN_GEODE]);
			}

			this->addChild(transforms[data.id]);
		}
		
		if (data.ent_data.health <= 0) { //if updated health marks entity as dead
			//TODO play death animation

			int id = idMap[data.id].first;
			switch (idMap[data.id].second) {
			case EntityType::PLAYER:
				this->removeChild(transforms[data.id]);
				conf_delete(playerMap[id]);
				playerMap.erase(id);
				idMap.erase(data.id);
				conf_delete(transforms[data.id]);
				transforms.erase(data.id);
				break;
			case EntityType::LASER_TOWER:
				this->removeChild(transforms[data.id]);
				conf_delete(laserTowerMap[id]);
				laserTowerMap.erase(id);
				idMap.erase(data.id);
				conf_delete(transforms[data.id]);
				transforms.erase(data.id);
				break;
			case EntityType::MINION:
				deathlist.push_back(data.id);
				minionMap[id]->kill();
				break;
			case EntityType::OTHER:
				this->removeChild(transforms[data.id]);
				conf_delete(entityMap[id]);
				entityMap.erase(id);
				idMap.erase(data.id);
				conf_delete(transforms[data.id]);
				transforms.erase(data.id);
				break;
			}
		}
		else { //otherwise, update the entity's data and transform
			int id = idMap[data.id].first;
			switch (idMap[data.id].second) {
			case EntityType::LASER_TOWER:
				laserTowerMap[id]->setEntData(data.ent_data);
				if (laserTowerMap[id]->getActionState() == ACTION_STATE_FIRE) {
					laserTowerMap[id]->activate(minionMap[idMap[laserTowerMap[id]->getTargetID()].first]->getPosition());
				}
				transforms[data.id]->setMatrix(laserTowerMap[id]->getMatrix());
				break;
			case EntityType::MINION:
				minionMap[id]->setEntData(data.ent_data);
				if (minionMap[id]->getActionState() == ACTION_STATE_FIRE) {
					minionMap[id]->setEntData(data.ent_data); //TODO ask kevin what this does
					minionMap[id]->shoot();
				}
				transforms[data.id]->setMatrix(minionMap[id]->getMatrix());
				break;
			case EntityType::PLAYER:
				playerMap[id]->setEntData(data.ent_data);
				transforms[data.id]->setMatrix(playerMap[id]->getMatrix());
				break;
			case EntityType::OTHER:
				entityMap[id]->setEntData(data.ent_data);
				transforms[data.id]->setMatrix(entityMap[id]->getMatrix());
				break;
			}
		}
		
		// Post death processing
		std::vector<int> toRemove = std::vector<int>();
		for (auto mapID : deathlist) {
			int id = idMap[mapID].first;
			switch (idMap[mapID].second) {
			case EntityType::MINION:
				// We can suspend removal here to play a death animation
				if (!minionMap[id]->alive) {
					this->removeChild(transforms[mapID]);
					conf_delete(minionMap[id]);
					minionMap.erase(id);
					idMap.erase(mapID);
					conf_delete(transforms[mapID]);
					transforms.erase(mapID);
					toRemove.push_back(mapID);
				}
				else {
					(minionMap[id]->deathCounter)--;
					// This is a hack. There's an issue where the tower tries to shoot at the minion after it's been removed
					if (minionMap[id]->deathCounter == 0) {
						minionMap[id]->alive = false;
					}
				}
				break;
			}
		}
		for (auto id : toRemove) deathlist.erase(std::remove(deathlist.begin(), deathlist.end(), id), deathlist.end());
	}
}

void SceneManager_Client::updateFromInputBuf(float deltaTime)
{
	char recvbuf[DEFAULT_BUFLEN];
	Input::EncodeToBuf(recvbuf);
	PlayerInput input = ((PlayerInput*)recvbuf)[0];
	//printf("%d %d %f\n", input.move_x, input.move_z, input.view_y_rot);
	//player.setMoveAndDir(input);
	//player.update();
	//transforms[0]->setMatrix(player.getMatrix());
}

void SceneManager_Client::setBuffer(SceneManager_Client::SceneBuffer type, Buffer** buffer)
{
	switch (type) {
	case SceneManager_Client::SceneBuffer::INSTANCE:
		instanceBuffer = buffer;
		break;
	case SceneManager_Client::SceneBuffer::BONE:
		boneBuffer = buffer;
		break;
	case SceneManager_Client::SceneBuffer::PARTICLES:
		particleBuffer = buffer;
		break;
	}
}

void SceneManager_Client::setProgram(SceneManager_Client::GeodeType type, Geode::GeodeShaderDesc program)
{
	switch (type) {
	case SceneManager_Client::GeodeType::MESH:
		for (auto ge : gltfGeodes) {
			ge.second->setProgram(program);
		}
		break;
	case SceneManager_Client::GeodeType::ANIMATED_MESH:
		for (auto ge : ozzGeodes) {
			ge.second->setProgram(program);
		}
		break;
	case SceneManager_Client::GeodeType::PARTICLES:
		for (auto ge : particleGeodes) {
			ge.second->setProgram(program);
		}
		break;
	}
}

void SceneManager_Client::trackPlayer(int player_id) {
	std::cout << "tracking player " << player_id << "\n";
	trackedPlayer_ID = player_id;
}

mat4 SceneManager_Client::getPlayerTransformMat() {
	
	if (trackedPlayer_ID != NO_TRACKED_PLAYER && transforms.find(trackedPlayer_ID) != transforms.end()) {
		return transforms[trackedPlayer_ID]->getMatrix();
	}
	else return mat4::identity();
}

void SceneManager_Client::update(float deltaTime)
{
	if (counter % 100 == 0) animators[9999999]->SetClip(smallMinionActions[animCounter]);
	//if (counter % 100 == 0)	AudioManager::playAudioSource(vec3(0), "laser");
	if (counter++ % 100 == 0) animators[8888888]->SetClip(superMinionActions[animCounter = (animCounter + 1) % 3]);
	Transform::update(deltaTime);
}

void SceneManager_Client::draw(Cmd* cmd)
{
	// Update per-instance uniforms
	BufferUpdateDesc shaderCbv = { instanceBuffer[Application::gFrameIndex] };
	beginUpdateResource(&shaderCbv);
	updateTransformBuffer(shaderCbv, mat4::identity());
	endUpdateResource(&shaderCbv, NULL);
	
	shaderCbv = { boneBuffer[Application::gFrameIndex] };
	beginUpdateResource(&shaderCbv);
	updateBoneBuffer(shaderCbv, NULL);
	endUpdateResource(&shaderCbv, NULL);

	shaderCbv = { particleBuffer[Application::gFrameIndex] };
	beginUpdateResource(&shaderCbv);
	updateParticleBuffer(shaderCbv);
	endUpdateResource(&shaderCbv, NULL);

	// Do culling check
	vec4 frustumPlanes[6];
	mat4::extractFrustumClipPlanes(Application::projMat * Application::viewMat, frustumPlanes[0], frustumPlanes[1], frustumPlanes[2], frustumPlanes[3], frustumPlanes[4], frustumPlanes[5], true);
	cull(frustumPlanes, SceneManager_Client::enableCulling);
	
	// Draw Graph
	Transform::draw(cmd);
}
/***** legacy code *****/
/*if (trackedPlayer_ID == "" && bufsize == 1) {	//first message from server should be 1 byte message
	trackPlayer(std::string(1, buf[0]));		//of the player's connection id
}
else {
	std::string id_str = "";
	GameObject::GameObjectData data;
	int health;
	int state = 0; // 0 for reading id, 1 for reading gameobjectdata, 2 for reading health, 3 for checking closing delimiter
	for (int i = 0; i < DEFAULT_BUFLEN; i++) {
		//std::cout << "i: " << i << "\n";
		if (state == 0) {
			if (buf[i] == DELIMITER) {
				state++; //end of state found, advance to next state
				//std::cout << "id_str: " + id_str + "\n";
			}
			else { //read id bytes one by one, appending to id_str
				id_str += buf[i];
			}

		}
		else if (state == 1) {
			int move_x = ((int*)(buf + i))[0];
			int move_z = ((int*)(buf + i + 4))[0];
			float rot_y = ((float*)(buf + i + 8))[0];
			//std::cout << "move_x: " << move_x << " move_z: " << move_z << " rot_y: " << rot_y << "\n";
			data = ((GameObject::GameObjectData*)(buf + i))[0];
			i += (sizeof GameObject::GameObjectData); //advance i to where delimiter should be

			if (buf[i] == DELIMITER) {
				//std::cout << "state 1 delimiter at i: " << i << "\n";
				state++; //end of state found, advance to next state
			}
			else {
				//std::cout << "state 1 delimiter expected but not found, i: " << i << "\n";
			}
		}
		else if (state == 2) {
			health = ((int*)(buf + i))[0];
			i += sizeof(int); //advance i to where delimiter should be

			if (buf[i] == DELIMITER) { //end of data line found, write to map, advance to possible final state
				//std::cout << "id: " << id_str << " x: " << data.x << " z: " << data.z << " y: " << data.rot << " health: " << health << "\n";
				//std::cout << "state 2 delimiter at i: " << i << "\n";


			}
			else {
				std::cout << "state 2 delimiter not found when expected, i: " << i << "\n";
			}
		}
		else if (state == 3) {
			if (buf[i] == DELIMITER) {
				std::cout << "closing delimiter found at i: " << i << "\n";
				break;
			}
			else {
				std::cout << "non-closing byte in state 3 at i: " << i << ", rechecking\n";
				state = 0; //reset state and read this byte again
				i--;
			}
		}
		else {
			std::cout << "SceneManager_Client updateFromClientBuf state out of sync";
		}
	}
}*/
