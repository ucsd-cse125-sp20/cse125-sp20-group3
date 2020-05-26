#include "AudioManager.h"
#include "Application.h"

SoLoud::Soloud* AudioManager::audio = NULL;
std::map<std::string, SoLoud::Wav*> AudioManager::wavs = std::map<std::string, SoLoud::Wav*>();
std::vector<AudioManager::AudioSource> AudioManager::sources = std::vector<AudioManager::AudioSource>();
SoLoud::handle AudioManager::loop = 0;
SceneManager_Client* AudioManager::scene = NULL;

void AudioManager::initialize(SceneManager_Client* scene)
{
	AudioManager::scene = scene;
	audio = conf_new(SoLoud::Soloud);
	audio->init();
}

void AudioManager::exit()
{
	audio->stopAll();
	for (auto w : wavs) conf_delete(w.second);
	audio->deinit();
	conf_delete(audio);
}

void AudioManager::setGlobalVolume(float volume)
{
	audio->setGlobalVolume(volume);
}

void AudioManager::loadWav(std::string filename, std::string label)
{
	wavs[label] = conf_new(SoLoud::Wav);
	SoLoud::result res = wavs[label]->load(filename.c_str());
	ASSERT(res == SoLoud::SO_NO_ERROR);
}

void AudioManager::setLoop(std::string label, float volume)
{
	loop = audio->playBackground(*wavs[label], volume);
	audio->setLooping(loop, true);
}

void AudioManager::playAudioSource(vec3 src, std::string label, float volume)
{
	auto soundParams = getVolumePan(src);
	AudioSource clip = {};
	clip.wav = audio->play(*wavs[label], soundParams.first * volume, soundParams.second);
	clip.src = src;
	clip.volume = volume;

	sources.push_back(clip);
}

void AudioManager::update()
{
	for (int i = 0; i < sources.size(); i++) {
		if (audio->getVoiceFromHandle(sources[i].wav) != -1) {
			auto soundParams = getVolumePan(sources[i].src);
			audio->setVolume(sources[i].wav, soundParams.first * sources[i].volume);
			audio->setPan(sources[i].wav, soundParams.second);
		}
		else {
			sources.erase(sources.begin() + i);
			i--;
		}
	}
}

std::pair<float, float> AudioManager::getVolumePan(vec3 src)
{
	vec3 position = scene->getPlayerTransformMat()[3].getXYZ();
	vec3 right = normalize(inverse(Application::viewMat)[0].getXYZ());
	
	vec3 diff = src - position;
	float len = length(diff);
	float volume = len > AUDIO_FALLOFF_DISTANCE ? 1 / pow(len / AUDIO_FALLOFF_DISTANCE, 2) : 1.0f;
	float pan = len > AUDIO_FALLOFF_DISTANCE ? dot(normalize(diff), right) : (len > AUDIO_FALLOFF_DISTANCE / 2 ? dot(normalize(diff), right) * (len -  AUDIO_FALLOFF_DISTANCE / 2) : 0);
	//printf("%f %f ", pan, volume);
	//print(right);
	return std::make_pair(volume, pan);
}
