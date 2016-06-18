#ifndef TOWER_DEFENSE_GAME_SCRIPTS
#define TOWER_DEFENSE_GAME_SCRIPTS

#include"towerEntities.h"

//AIScript delcared in towerEntities.h so unique_ptr could be created for Entity

class WayPathScript : public AIScript {
	public:
		WayPathScript (WayPath, Entity*);
		virtual AIScript* copy ();
		virtual void onStep ();
	protected:
		WayPath path;
};

#endif
