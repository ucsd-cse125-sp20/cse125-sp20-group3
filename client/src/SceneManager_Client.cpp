#include "SceneManager_Client.h"
#include "Application.h"

static bool first_update = true;

namespace {
	//////////////////////////////// Models //////////////////////////////////
	const char* mapFile = "map.gltf";
	const char* dumpsterFile = "resource-1-dumpster.gltf";
	const char* recyclingBinFile = "resource-2-recycling-bin.gltf";
	const char* playerFile = "char-1-female.gltf";
	//const char* baseFile = "";
	const char* minionFile = "minion-retry.gltf";
	const char* superMinionFile = "minion-2-super.gltf";
	const char* laserTowerFile = "tower-1-laser.gltf";
	const char* clawTowerFile = "tower-3-claw-machine.gltf";

	//////////////////////////// Animations ///////////////////////////////////
	// Animations
	const char* playerMaleDir = "male-char-A";
	const char* playerMaleActions[2] = { "Idle", "Walking" };
	const char* playerFemaleDir = "female-char-A";
	const char* playerFemaleActions[2] = { "Idle", "Walking" };
	const char* smallMinionDir = "small-minion-B";
	const char* smallMinionActions[3] = { "Walking", "Fighting", "Death" };
	const char* superMinionDir = "super-minion-A";
	const char* superMinionActions[3] = { "Walking", "Fighting", "Death" };
	const char* clawTowerDir = "claw-machine-A";
	const char* clawTowerActions[1] = { "ClawAction" };
	const char* dumpsterDir = "dumpster";
	const char* dumpsterActions[1] = { "OpenAndClose" };
	const char* recyclingBinDir = "recycling-bin";
	const char* recyclingBinActions[1] = { "Harvested" };


	int counter = 1;
	int animCounter = 1;
}

bool SceneManager_Client::enableCulling = true;

SceneManager_Client::SceneManager_Client(Renderer* renderer)
{
	this->renderer = renderer;
	// It'd be nice if I could put this in a loop later
	gltfGeodes[ENV_GEODE] = conf_new(GLTFGeode, renderer, mapFile); //ok
	//gltfGeodes[PLAYER_GEODE] = conf_new(GLTFGeode, renderer, playerFile);
	//gltfGeodes[BASE_GEODE] = conf_new(GLTFGeode, renderer, baseFile);
	//gltfGeodes[MINION_GEODE] = conf_new(GLTFGeode, renderer, minionFile);
	//gltfGeodes[SUPER_MINION_GEODE] = conf_new(GLTFGeode, renderer, superMinionFile);
	gltfGeodes[LASER_TOWER_GEODE] = conf_new(GLTFGeode, renderer, laserTowerFile); //ok
	//gltfGeodes[CLAW_TOWER_GEODE] = conf_new(GLTFGeode, renderer, clawTowerFile);
	//gltfGeodes[DUMPSTER_GEODE] = conf_new(GLTFGeode, renderer, dumpsterFile); //ok
	//gltfGeodes[RECYCLING_BIN_GEODE] = conf_new(GLTFGeode, renderer, recyclingBinFile); //ok

	ozzGeodes[MINION_GEODE] = conf_new(OzzGeode, renderer, smallMinionDir);
	((OzzObject*)ozzGeodes[MINION_GEODE]->obj)->SetClip(smallMinionActions[0]); // Set a default action
	ozzGeodes[SUPER_MINION_GEODE] = conf_new(OzzGeode, renderer, superMinionDir);
	((OzzObject*)ozzGeodes[SUPER_MINION_GEODE]->obj)->SetClip(superMinionActions[0]); // Set a default action
	ozzGeodes[PLAYER_GEODE] = conf_new(OzzGeode, renderer, playerMaleDir);
	((OzzObject*)ozzGeodes[PLAYER_GEODE]->obj)->SetClip(playerMaleActions[0]); // Set a default action
	ozzGeodes[CLAW_TOWER_GEODE] = conf_new(OzzGeode, renderer, clawTowerDir);
	((OzzObject*)ozzGeodes[CLAW_TOWER_GEODE]->obj)->SetClip(clawTowerActions[0]);
	ozzGeodes[RECYCLING_BIN_GEODE] = conf_new(OzzGeode, renderer, recyclingBinDir);
	((OzzObject*)ozzGeodes[RECYCLING_BIN_GEODE]->obj)->SetClip(recyclingBinActions[0]);
	ozzGeodes[DUMPSTER_GEODE] = conf_new(OzzGeode, renderer, dumpsterDir);
	((OzzObject*)ozzGeodes[DUMPSTER_GEODE]->obj)->SetClip(dumpsterActions[0]);


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
	particleParams.life = MINION_BULLET_LIFE;
	particleParams.initializer = [](ParticleSystem::ParticleData* pd, ParticleSystem::ParticleAuxData* pad) {
		float r = MathUtils::randfUniform(0, 0.05f);
		float a = MathUtils::randfUniform(0, 2 * PI);
		float z = 0;

		pd->position = float3(r * cos(a), r * sin(a), z);
		pd->color = float4(1.0f, 1.0f, 0.5f, 1.0f);
		pd->scale = float2(0.1f, 0.05f);
		pad->velocity = float3(0.f, 0.f, MINION_BULLET_SPEED);
	};
	particleGeodes[MINION_GEODE] = conf_new(ParticleSystemGeode, renderer, particleParams);

	///////////////////////////////////////////////////////////////////////////////


	// TODO This is a hard coded animation example. Remove this later
	/*int key = 8888828;
	Transform* t = conf_new(Transform, mat4::scale(vec3(0.5f)) * mat4::translation(vec3(0, 5.5, 0)) * mat4::rotationX(PI));
	Animator* a = conf_new(Animator, ozzGeodes[CLAW_TOWER_GEODE]);
	a->SetClip(clawTowerActions[0]);
	t->setColor(vec4(1.f, 1.f, 1.f, 1.f));
	t->addChild(a);
	this->addChild(t);
	transforms[key] = t;
	animators[key] = a;*/

	trackedPlayer_ID = NO_TRACKED_PLAYER;

	transforms[GROUND_KEY] = conf_new(Transform, mat4::scale(vec3(1.31f)) * mat4::translation(vec3(49.7f, 0.0f, -38.5f)));
	transforms[GROUND_KEY]->addChild(gltfGeodes[ENV_GEODE]);
	this->addChild(transforms[GROUND_KEY]);

	red_team = new Team(RED_TEAM);
	blue_team = new Team(BLUE_TEAM);
}

SceneManager_Client::~SceneManager_Client()
{
	for (std::pair<int, Entity*> e : idMap) {
		if (ID_PLAYER_MIN <= e.first && e.first <= ID_PLAYER_MAX) {
			conf_delete((Player_Client*)e.second);
		}
		if (ID_BASE_MIN <= e.first && e.first <= ID_BASE_MAX) {
			conf_delete((Base_Client*)e.second);
		}
		if (ID_MINION_MIN <= e.first && e.first <= ID_MINION_MAX) {
			conf_delete((Minion_Client*)e.second);
		}
		if (ID_SUPER_MINION_MIN <= e.first && e.first <= ID_SUPER_MINION_MAX) {
			conf_delete((SuperMinion_Client*)e.second);
		}
		if (ID_LASER_MIN <= e.first && e.first <= ID_LASER_MAX) {
			conf_delete((LaserTower_Client*)e.second);
		}
		if (ID_CLAW_MIN <= e.first && e.first <= ID_CLAW_MAX) {
			conf_delete((ClawTower_Client*)e.second);
		}
		if (ID_DUMPSTER_MIN <= e.first && e.first <= ID_DUMPSTER_MAX) {
			conf_delete((Resource_Client*)e.second);
		}
		if (ID_RECYCLING_BIN_MIN <= e.first && e.first <= ID_RECYCLING_BIN_MAX) {
			conf_delete((Resource_Client*)e.second);
		}
	}
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
	if (data.sceneUpdate.entUpdates.size() > 0) {
		this->updateScene(data.sceneUpdate);
		this->updateState(data.stateUpdate);
	}
}

void SceneManager_Client::updateState(Client::StateUpdateData updateData) {
	red_team->setData(updateData.redTeamData);
	blue_team->setData(updateData.blueTeamData);

	if (!first_update) SceneManager_Client::updateUI();
}

void SceneManager_Client::updateUI() {
	Team* trackedPlayerTeam = idMap[trackedPlayer_ID]->getTeam();
	if (trackedPlayerTeam->teamColor != RED_TEAM && trackedPlayerTeam->teamColor != BLUE_TEAM) return;

	if (trackedPlayerTeam->teamColor == RED_TEAM){
		UIUtils::editText(TEAM_TEXT, "Red Team", "small font", 0xffffffff);
	}else{
		UIUtils::editText(TEAM_TEXT, "Blue Team", "small font", 0xffffffff);
	}

	int plasticCount = trackedPlayerTeam->getPlasticCount();
	//std::cout << "team " << trackedPlayerTeam->teamColor << " plastic count: " << plasticCount << "\n";
	int metalCount = trackedPlayerTeam->getMetalCount();
	//std::cout << "team " << trackedPlayerTeam->teamColor << " metal count: " << metalCount << "\n";

	BUILD_MODE buildMode = ((Player_Client*)idMap[trackedPlayer_ID])->getBuildMode();
	//std::cout << "buildMode " << buildMode << "\n";

	UIUtils::editText(PLASTIC_UI_TEXT, std::to_string(plasticCount), "small font", 0xff6655ff);
	UIUtils::editText(METAL_UI_TEXT, std::to_string(metalCount), "small font", 0xff6655ff);
	if (plasticCount <= 0) { //counts shouldn't ever be negative but it can't hurt to check
		UIUtils::changeImage(PLASTIC_UI_ICON, "resource_plastic_alert.png", float2((float)Application::width / 3200, (float)Application::height / 2000));
	}
	else {
		UIUtils::changeImage(PLASTIC_UI_ICON, "resource_plastic.png", float2((float)Application::width / 3200, (float)Application::height / 2000));
	}
	if (metalCount <= 0) {
		UIUtils::changeImage(METAL_UI_ICON, "resource_metal_alert.png", float2((float)Application::width / 3200, (float)Application::height / 2000));
	}
	else {
		UIUtils::changeImage(METAL_UI_ICON, "resource_metal.png", float2((float)Application::width / 3200, (float)Application::height / 2000));
	}

	bool super_minion = trackedPlayerTeam->checkResources(SUPER_MINION_TYPE);
	bool claw_machine = trackedPlayerTeam->checkResources(CLAW_TYPE);
	bool laser_tower = trackedPlayerTeam->checkResources(LASER_TYPE);

	if (laser_tower) {
		UIUtils::changeImage(LASER_TOWER_UI_ICON, "tower_2.png", float2((float)Application::width / 3200, (float)Application::height / 2000));
	}
	else {
		UIUtils::changeImage(LASER_TOWER_UI_ICON, "tower_2_low.png", float2((float)Application::width / 3200, (float)Application::height / 2000));
	}

	if (claw_machine){
		UIUtils::changeImage(CLAW_MACHINE_UI_ICON, "tower_1.png",  float2((float)Application::width / 3200, (float)Application::height / 2000));
	}else{
		UIUtils::changeImage(CLAW_MACHINE_UI_ICON, "tower_1_low.png",  float2((float)Application::width / 3200, (float)Application::height / 2000));
	}

	if (super_minion) {
		UIUtils::changeImage(SUPER_MINION_UI_ICON, "tower_4.png", float2((float)Application::width / 3200, (float)Application::height / 2000));
	}
	else {
		UIUtils::changeImage(SUPER_MINION_UI_ICON, "tower_4_low.png", float2((float)Application::width / 3200, (float)Application::height / 2000));
	}

	if(buildMode == LASER){
		UIUtils::changeImage(LASER_TOWER_UI_ICON, "tower_2_build_mode.png", float2((float)Application::width / 3200, (float)Application::height / 2000));
	}

	if(buildMode == CLAW){
		UIUtils::changeImage(CLAW_MACHINE_UI_ICON, "tower_1_build_mode.png", float2((float)Application::width / 3200, (float)Application::height / 2000));
	}
	
	if(buildMode == SUPER_MINION){
		UIUtils::changeImage(SUPER_MINION_UI_ICON, "tower_4_build_mode.png", float2((float)Application::width / 3200, (float)Application::height / 2000));
	}

	// update base health
	int red_health = red_team->getBaseHealth();
	int blue_health = blue_team->getBaseHealth();

	//printf("%s %d %d\n", "base health red and blue", red_health, blue_health);
	
	//UIUtils::changeImage(HEALTH_BAR_BLUE_TEAM, "base_health_bars_blue.png", float2((float)blue_health/(2 * BASE_HEALTH), (float)1/2));
	UIUtils::changeImage(HEALTH_BAR_BLUE_TEAM_DEDUCTED, "base_health_bars_black.png", float2((float)(BASE_HEALTH - blue_health) / (2 * BASE_HEALTH), (float)1 / 2));
	UIUtils::changeImage(HEALTH_BAR_RED_TEAM, "base_health_bars_red.png", float2((float)red_health / (2 * BASE_HEALTH),(float)1/2));

	if (red_health <= 0) {
		if (trackedPlayerTeam->teamColor == RED_TEAM) {
			UIUtils::createText("defeat_text", "Defeat", 640, 280, "large font", 0xff6655ff, 3);
		}
		else {
			UIUtils::createText("victory_text", "Victory", 600, 280, "large font", 0xff6655ff, 3);
		}
	}
	else if (blue_health <= 0) {
		if (trackedPlayerTeam->teamColor == BLUE_TEAM) {
			UIUtils::createText("defeat_text", "Defeat", 640, 280, "large font", 0xff6655ff, 3);
		}
		else {
			UIUtils::createText("victory_text", "Victory", 600, 280, "large font", 0xff6655ff, 3);
		}
	}

}

void SceneManager_Client::updateScene(Client::SceneUpdateData updateData)
{
	std::vector<int> deadEntities;

	if (updateData.entUpdates.size() > 0) first_update = false;

	//std::cout << "updating from client buf of size " << updateData.entUpdates.size() << "\n";
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

				Player_Client* p_c = conf_new(Player_Client, GO_data, data.id, team, this, ozzGeodes[PLAYER_GEODE], adjustment);
				idMap[data.id] = p_c;
				wrapperMap[data.id] = p_c;

				Transform* previewAdjustment = conf_new(Transform, mat4::scale(vec3(0.5f)) * mat4::translation(vec3(0, 5.5, 0)) * mat4::rotationX(PI));
				previewAdjustment->addChild(ozzGeodes[CLAW_TOWER_GEODE]);
				otherTransforms.push_back(previewAdjustment);
				p_c->setPreview(BUILD_MODE::SUPER_MINION, ozzGeodes[SUPER_MINION_GEODE]);
				p_c->setPreview(BUILD_MODE::CLAW, previewAdjustment);
				p_c->setPreview(BUILD_MODE::LASER, gltfGeodes[LASER_TOWER_GEODE]);
			}
			else if (ID_BASE_MIN <= data.id && data.id <= ID_BASE_MAX) {
				std::cout << "creating new base, id: " << data.id << "\n";
				transforms[data.id] = conf_new(Transform, mat4::identity());
				Base_Client* b_c = conf_new(Base_Client, GO_data, data.id, team, this, ozzGeodes[MINION_GEODE], transforms[data.id]);
				idMap[data.id] = b_c;
				wrapperMap[data.id] = b_c;
			}
			else if (ID_MINION_MIN <= data.id && data.id <= ID_MINION_MAX) {
				std::cout << "creating new minion, id: " << data.id << "\n";
				transforms[data.id] = conf_new(Transform, mat4::identity());
				Minion_Client* m_c = conf_new(Minion_Client, GO_data, data.id, team, this, ozzGeodes[MINION_GEODE], particleGeodes[MINION_GEODE], transforms[data.id]);
				idMap[data.id] = m_c;
				wrapperMap[data.id] = m_c;
			}
			else if (ID_SUPER_MINION_MIN <= data.id && data.id <= ID_SUPER_MINION_MAX) {
				std::cout << "creating new super minion, id: " << data.id << "\n";
				transforms[data.id] = conf_new(Transform, mat4::identity());
				SuperMinion_Client* s_m_c = conf_new(SuperMinion_Client, GO_data, data.id, team, this, ozzGeodes[SUPER_MINION_GEODE], transforms[data.id]);
				idMap[data.id] = s_m_c;
				wrapperMap[data.id] = s_m_c;
			}
			else if (ID_LASER_MIN <= data.id && data.id <= ID_LASER_MAX) {
				std::cout << "creating new laser tower, id: " << data.id << "\n";
				transforms[data.id] = conf_new(Transform, mat4::identity());
				LaserTower_Client* l_c = conf_new(LaserTower_Client, GO_data, data.id, team, this, gltfGeodes[LASER_TOWER_GEODE], particleGeodes[LASER_TOWER_GEODE], transforms[data.id]);
				idMap[data.id] = l_c;
				wrapperMap[data.id] = l_c;
			}
			else if (ID_CLAW_MIN <= data.id && data.id <= ID_CLAW_MAX) {
				std::cout << "creating new claw tower, id: " << data.id << "\n";
				transforms[data.id] = conf_new(Transform, mat4::identity());
				Transform* adjustment = conf_new(Transform, mat4::scale(vec3(0.5f)) * mat4::translation(vec3(0, 5.5, 0)) * mat4::rotationX(PI));
				transforms[data.id]->addChild(adjustment);
				ClawTower_Client* c_c = conf_new(ClawTower_Client, GO_data, data.id, team, this, ozzGeodes[CLAW_TOWER_GEODE], adjustment);
				otherTransforms.push_back(adjustment);
				idMap[data.id] = c_c;
				wrapperMap[data.id] = c_c;
			}
			else if (ID_DUMPSTER_MIN <= data.id && data.id <= ID_DUMPSTER_MAX) {
				std::cout << "creating new dumpster, id: " << data.id << "\n";
				transforms[data.id] = conf_new(Transform, mat4::identity());
				Resource_Client* d_c = conf_new(Resource_Client, DUMPSTER_TYPE, GO_data, data.id, this, ozzGeodes[DUMPSTER_GEODE], transforms[data.id]);
				idMap[data.id] = d_c;
				wrapperMap[data.id] = d_c;
			}
			else if (ID_RECYCLING_BIN_MIN <= data.id && data.id <= ID_RECYCLING_BIN_MAX) {
				std::cout << "creating new recycling bin id: " << data.id << "\n";
				transforms[data.id] = conf_new(Transform, mat4::identity());
				Transform* adjustment = conf_new(Transform, mat4::scale(vec3(2.0f)));
				transforms[data.id]->addChild(adjustment);
				Resource_Client* r_b_c = conf_new(Resource_Client, RECYCLING_BIN_TYPE, GO_data, data.id, this, ozzGeodes[RECYCLING_BIN_GEODE], adjustment);
				otherTransforms.push_back(adjustment);
				idMap[data.id] = r_b_c;
				wrapperMap[data.id] = r_b_c;
			}

			this->addChild(transforms[data.id]);
		}

		if (data.ent_data.health <= 0 && (data.id < ID_BASE_MIN || data.id > ID_BASE_MAX)) { //if updated health marks entity as dead
			//ignore bases, we want to know that bases reached 0 health
			deadEntities.push_back(data.id); //delay actual deletion until after all particle systems have been fired
		}
		else { //otherwise, update the entity's data and transform
			idMap[data.id]->setEntData(data.ent_data);
			transforms[data.id]->setMatrix(idMap[data.id]->getMatrix());
			wrapperMap[data.id]->updateAnimParticles();
		}
		/*
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
		*/
	}

	for (int id : deadEntities) {
		this->removeChild(transforms[id]);

		if (ID_PLAYER_MIN <= id && id <= ID_PLAYER_MAX) {
			conf_delete((Player_Client*)idMap[id]);
		}
		if (ID_BASE_MIN <= id && id <= ID_BASE_MAX) {

		}
		if (ID_MINION_MIN <= id && id <= ID_MINION_MAX) {
			conf_delete((Minion_Client*)idMap[id]);
		}
		if (ID_SUPER_MINION_MIN <= id && id <= ID_SUPER_MINION_MAX) {
			conf_delete((SuperMinion_Client*)idMap[id]);
		}
		if (ID_LASER_MIN <= id && id <= ID_LASER_MAX) {
			conf_delete((LaserTower_Client*)idMap[id]);
		}
		if (ID_CLAW_MIN <= id && id <= ID_CLAW_MAX) {
			conf_delete((ClawTower_Client*)idMap[id]);
		}
		if (ID_DUMPSTER_MIN <= id && id <= ID_DUMPSTER_MAX) {
			conf_delete((Resource_Client*)idMap[id]);
		}
		if (ID_RECYCLING_BIN_MIN <= id && id <= ID_RECYCLING_BIN_MAX) {
			conf_delete((Resource_Client*)idMap[id]);
		}

		idMap.erase(id);
		conf_delete(transforms[id]);
		transforms.erase(id);
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

vec3 SceneManager_Client::getTargetPosition(int targetID) {
	return idMap[targetID]->getPosition();
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
	//if (counter % 100 == 0) animators[9999999]->SetClip(smallMinionActions[animCounter]);
	//if (counter % 100 == 0)	AudioManager::playAudioSource(vec3(0), "laser");
	//if (counter++ % 100 == 0) animators[8888888]->SetClip(superMinionActions[animCounter = (animCounter + 1) % 3]);
	Transform::update(deltaTime);
}

void SceneManager_Client::draw(Cmd* cmd)
{
	// Update per-instance uniforms
	BufferUpdateDesc shaderCbv = { instanceBuffer[Application::gFrameIndex] };
	beginUpdateResource(&shaderCbv);
	updateTransformBuffer(shaderCbv, mat4::identity(), vec4(1));
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