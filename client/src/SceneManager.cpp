#include "SceneManager.h"

SceneManager::SceneManager()
{
	root = conf_new(Transform);
}

SceneManager::~SceneManager()
{
	conf_delete(root);
}
