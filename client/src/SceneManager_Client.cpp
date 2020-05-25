#include "SceneManager_Client.h"
#include "Application.h"

namespace {
	const char* playerFile = "char-1-female.gltf";
	const char* groundFile = "Ground.gltf";
	const char* minionFile = "minion-retry.gltf";
	const char* superMinionFile = "minion-2-super.gltf";
	const char* laserTowerFile = "tower-1-laser.gltf";
	const char* clawTowerFile = "tower-3-claw-machine.gltf";
	const char* dumpsterFile = "resource-1-dumpster.gltf";
	const char* recyclingBinFile = "resource-2-recycling-bin.gltf";

	// Animations
	const char* smallMinionDir = "small-minion";
	const char* smallMinionActions[2] = { "Walking", "Fighting" };
	const char* superMinionDir = "super-minion";
	const char* superMinionActions[2] = { "Walking", "Fighting" };
	const char* playerMaleDir = "male-char";
	const char* playerMaleActions[2] = { "Idle", "Walking" };
	const char* playerFemaleDir = "female-char";
	const char* playerFemaleActions[2] = { "Idle", "Walking" };

	int counter = 0;
	int animCounter = 1;
}

bool SceneManager_Client::enableCulling = true;

SceneManager_Client::SceneManager_Client(Renderer* renderer)
{
	this->renderer = renderer;
	// It'd be nice if I could put this in a loop later
	gltfGeodes[ENV_GEODE] = conf_new(GLTFGeode, renderer, groundFile);
	gltfGeodes[PLAYER_GEODE] = conf_new(GLTFGeode, renderer, playerFile);
	//gltfGeodes[BASE_GEODE] = conf_new(GLTFGeode, renderer, baseFile);
	gltfGeodes[MINION_GEODE] = conf_new(GLTFGeode, renderer, minionFile);
	gltfGeodes[SUPER_MINION_GEODE] = conf_new(GLTFGeode, renderer, superMinionFile);
	gltfGeodes[LASER_TOWER_GEODE] = conf_new(GLTFGeode, renderer, laserTowerFile);
	gltfGeodes[CLAW_TOWER_GEODE] = conf_new(GLTFGeode, renderer, clawTowerFile);
	gltfGeodes[DUMPSTER_GEODE] = conf_new(GLTFGeode, renderer, dumpsterFile);
	gltfGeodes[RECYCLING_BIN_GEODE] = conf_new(GLTFGeode, renderer, recyclingBinFile);

	///////////////////////////////////////////////////////////////////////////////

    // TODO This is a hard coded animation example. Remove this later (SMALL MINION)
	int key = 9999999;
    ozzGeodes["blarf"] = conf_new(OzzGeode, renderer, smallMinionDir);
	((OzzObject*)ozzGeodes["blarf"]->obj)->SetClip(smallMinionActions[0]);
    Transform* t = conf_new(Transform, mat4::translation(vec3(-1, 0, 2)));
    Animator* a = conf_new(Animator, ozzGeodes["blarf"]);
    a->SetClip(smallMinionActions[0]);
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

	// TODO This is a hard coded animation example. Remove this later (MALE CHAR)
	key = 4672347;
	ozzGeodes["blarf3"] = conf_new(OzzGeode, renderer, playerMaleDir);
	((OzzObject*)ozzGeodes["blarf3"]->obj)->SetClip(playerMaleActions[1]);
	t = conf_new(Transform, mat4::translation(vec3(3, 0, 2)));
	a = conf_new(Animator, ozzGeodes["blarf3"]);
	a->SetClip(playerMaleActions[1]);
	t->addChild(a);
	this->addChild(t);
	transforms[key] = t;
	animators[key] = a;

	// TODO This is a hard coded animation example. Remove this later (FEMALE CHAR)
	key = 1734813;
	ozzGeodes["blarf4"] = conf_new(OzzGeode, renderer, playerFemaleDir);
	((OzzObject*)ozzGeodes["blarf4"]->obj)->SetClip(playerFemaleActions[1]);
	t = conf_new(Transform, mat4::translation(vec3(4, 0, 2)));
	a = conf_new(Animator, ozzGeodes["blarf4"]);
	a->SetClip(playerFemaleActions[1]);
	t->addChild(a);
	this->addChild(t);
	transforms[key] = t;
	animators[key] = a;

	///////////////////////////////////////////////////////////////////////////////

	trackedPlayer_ID = NO_TRACKED_PLAYER;

	//transforms[GROUND_KEY] = conf_new(Transform, mat4::identity());
	//transforms[GROUND_KEY]->addChild(gltfGeodes[ENV_GEODE]);
	//this->addChild(transforms[GROUND_KEY]);

	red_team = new Team(RED_TEAM);
	blue_team = new Team(BLUE_TEAM);
}

SceneManager_Client::~SceneManager_Client()
{
	//printf("ASDFASDFASDF\n");
	for (std::pair<int, Entity*> e : entityMap) conf_delete(e.second);
	for (std::pair<int, LaserTower_Client*> t : laserTowerMap) conf_delete(t.second);
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

void SceneManager_Client::updateScene(std::vector<Client::UpdateData> updateBuf)
{
	//std::cout << "updating from client buf of size " << updateBuf.size() << "\n";
	for (Client::UpdateData data : updateBuf) {
		if (idMap.find(data.id) == idMap.end()) { //new id encountered, spawn new object
			Team* team;
			if (data.ent_data.teamColor == RED_TEAM) team = red_team;
			else if (data.ent_data.teamColor == BLUE_TEAM) team = blue_team;
			else team = nullptr;

			if (ID_PLAYER_MIN <= data.id && data.id <= ID_PLAYER_MAX) {
				std::cout << "creating new player, id: " << data.id << "\n";

				idMap[data.id] = std::make_pair(++subid, EntityType::OTHER);
				entityMap[subid] = conf_new(Player, data.id, team, nullptr);
				transforms[data.id] = conf_new(Transform, mat4::identity());
				Transform* adjustment = conf_new(Transform, mat4::rotationY(-PI / 2));

				adjustment->addChild(gltfGeodes[PLAYER_GEODE]);
				transforms[data.id]->addChild(adjustment);

				otherTransforms.push_back(adjustment); //save to be deleted upon closing
			}
			else if (ID_BASE_MIN <= data.id && data.id <= ID_BASE_MAX) {
				//idMap[id_str] = new Base();
			}
			else if (ID_MINION_MIN <= data.id && data.id <= ID_MINION_MAX) {
				std::cout << "creating new minion, id: " << data.id << "\n";
				idMap[data.id] = std::make_pair(++subid, EntityType::OTHER);
				entityMap[subid] = conf_new(Minion, data.id, team, nullptr);
				transforms[data.id] = conf_new(Transform, mat4::identity());
				transforms[data.id]->addChild(gltfGeodes[MINION_GEODE]);
			}
			else if (ID_SUPER_MINION_MIN <= data.id && data.id <= ID_SUPER_MINION_MAX) {
				std::cout << "creating new super minion, id: " << data.id << "\n";
				idMap[data.id] = std::make_pair(++subid, EntityType::OTHER);
				entityMap[subid] = conf_new(SuperMinion, data.id, team, nullptr);
				transforms[data.id] = conf_new(Transform, mat4::identity());
				transforms[data.id]->addChild(gltfGeodes[SUPER_MINION_GEODE]);
			}
			else if (ID_LASER_MIN <= data.id && data.id <= ID_LASER_MAX) {
				std::cout << "creating new laser tower, id: " << data.id << "\n";
				idMap[data.id] = std::make_pair(++subid, EntityType::LASER_TOWER);
				laserTowerMap[subid] = conf_new(LaserTower_Client, data.id, team, nullptr, renderer);
				transforms[data.id] = conf_new(Transform, mat4::identity());
				transforms[data.id]->addChild(gltfGeodes[LASER_TOWER_GEODE]);
				transforms[data.id]->addChild(laserTowerMap[subid]->laserTransform);
				laserTowerMap[subid]->laser->createSpriteResources(particleRootSignature);
			}
			else if (ID_CLAW_MIN <= data.id && data.id <= ID_CLAW_MAX) {
				std::cout << "creating new claw tower, id: " << data.id << "\n";
				idMap[data.id] = std::make_pair(++subid, EntityType::OTHER);
				entityMap[subid] = conf_new(ClawTower, data.id, team, nullptr);
				transforms[data.id] = conf_new(Transform, mat4::identity());
				transforms[data.id]->addChild(gltfGeodes[CLAW_TOWER_GEODE]);
			}
			else if (ID_DUMPSTER_MIN <= data.id && data.id <= ID_DUMPSTER_MAX) {
				std::cout << "creating new dumpster, id: " << data.id << "\n";
				idMap[data.id] = std::make_pair(++subid, EntityType::OTHER);
				entityMap[subid] = conf_new(Resource, DUMPSTER_TYPE, data.id, nullptr);
				transforms[data.id] = conf_new(Transform, mat4::identity());
				transforms[data.id]->addChild(gltfGeodes[DUMPSTER_GEODE]);
			}
			else if (ID_RECYCLING_BIN_MIN <= data.id && data.id <= ID_RECYCLING_BIN_MAX) {
				std::cout << "creating new recycling bin id: " << data.id << "\n";
				idMap[data.id] = std::make_pair(++subid, EntityType::OTHER);
				entityMap[subid] = conf_new(Resource, RECYCLING_BIN_TYPE, data.id, nullptr);
				transforms[data.id] = conf_new(Transform, mat4::identity());
				transforms[data.id]->addChild(gltfGeodes[RECYCLING_BIN_GEODE]);
			}

			this->addChild(transforms[data.id]);
		}
		
		if (data.ent_data.health <= 0) { //updated health marks entity as dead
			//TODO play death animation

			this->removeChild(transforms[data.id]);

			int id = idMap[data.id].first;
			switch (idMap[data.id].second) {
			case EntityType::LASER_TOWER:
				conf_delete(laserTowerMap[id]);
				laserTowerMap.erase(id);
				break;
			case EntityType::OTHER:
				//conf_delete(entityMap[id]);
				//entityMap.erase(id);
				break;
			}
			//idMap.erase(data.id);
			conf_delete(transforms[data.id]);
			transforms.erase(data.id);
		}
		else { //otherwise, update the entity's data and transform
			int id = idMap[data.id].first;
			switch (idMap[data.id].second) {
			case EntityType::LASER_TOWER:
				laserTowerMap[id]->setEntData(data.ent_data);
				if (laserTowerMap[id]->getActionState() == LASER_ACTION_FIRE) {
					print(entityMap[idMap[laserTowerMap[id]->getTargetID()].first]->getMatrix());
					print(laserTowerMap[id]->getMatrix());
					laserTowerMap[id]->activate(entityMap[idMap[laserTowerMap[id]->getTargetID()].first]->getPosition());
				}
				transforms[data.id]->setMatrix(laserTowerMap[id]->getMatrix());
				break;
			case EntityType::OTHER:
				entityMap[id]->setEntData(data.ent_data);
				transforms[data.id]->setMatrix(entityMap[id]->getMatrix());
				break;
			}
		}
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
		for (auto ltower : laserTowerMap) {
			ltower.second->setProgram(program);
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
	if (counter++ % 100 == 0) animators[8888888]->SetClip(superMinionActions[animCounter = (animCounter + 1) % 2]);
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
