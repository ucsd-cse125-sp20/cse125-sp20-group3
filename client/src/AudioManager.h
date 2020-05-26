#pragma once

#include "../The-Forge/Common_3/ThirdParty/OpenSource/soloud20181119/include/soloud.h"
#include "../The-Forge/Common_3/ThirdParty/OpenSource/soloud20181119/include/soloud_wav.h"

#include "../../common/GameObject.h"

#include "SceneManager_Client.h"

#include <map>
#include <algorithm>

#define AUDIO_FALLOFF_DISTANCE 15

class AudioManager {
private:
	struct AudioSource {
		SoLoud::handle wav;
		vec3 src;
		float volume;
	};

	static SoLoud::Soloud* audio;

	static std::map<std::string, SoLoud::Wav*> wavs;
	static std::vector<AudioSource> sources;
	static SoLoud::handle loop;

	static SceneManager_Client* scene;

	static std::pair<float, float> getVolumePan(vec3 src);

public:
	static void initialize(SceneManager_Client* scene);
	static void exit();

	static void setGlobalVolume(float volume);

	static void loadWav(std::string filename, std::string label);

	static void setLoop(std::string label, float volume=1.0f);

	static void playAudioSource(vec3 src, std::string label, float volume=1.0f);

	static void update();
};