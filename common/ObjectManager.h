#ifndef _OBJECT_MANAGER_H_
#define _OBJECT_MANAGER_H_

#include <map>
#include <string>
#include "../server/GameObject.h"

class ObjectManager {
private:
		std::map<std::string, GameObject*> objectIDmap;
		int next_id;

public:
	ObjectManager();

};

#endif