#include"gameLevel.h"
#include"resourceManagerDefineResource.h"

#define COM_SEP ,

namespace ResourceManager {
	MANAGER_DEFINE_RESOURCE(ALLEGRO_MAP,GameLevel,al_open_map("data",fileName.c_str()),
			al_free_map(data),fileName,)
}

#undef COM_SEP
