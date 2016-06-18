#ifndef GAME_LEVEL_RESOURCE
#define GAME_LEVEL_RESOURCE
#include"resourceManager.h"
#include<allegro5/allegro_tiled.h>

#define RESOURCE_LD_ARGS_COM_SEP ,

namespace ResourceManager {
	MANAGER_DECLARE_RESOURCE(ALLEGRO_MAP,GameLevel,)
}

#undef RESOURCE_LD_ARGS_COM_SEP
#endif
