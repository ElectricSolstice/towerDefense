#ifndef TOWER_DEFENSE_GAME_CONTEXTS
#define TOWER_DEFENSE_GAME_CONTEXTS

#include"context.h"
#include"towerEntities.h"
#include"towerScripts.h"
#include"gameLevel.h"
#include"camera.h"
#include<vector>
#include<queue>

const std::string BACKGROUND_FILENAME = "data/pumpkin.png";

class MainMenu : public OuterContext {
	public:
		MainMenu(PlayerConfiguration*,Background* menuBg = new NullBackground());
		MainMenu(const MainMenu& original);
		MainMenu& operator=(const MainMenu& rhs);
		virtual void frameStep (double deltaTime);
		virtual void eventStep (ALLEGRO_EVENT* event);
		virtual void drawStep ();
		virtual void outerControl();
		virtual Context* copy ();
	protected:
		std::unique_ptr<Background> bg;
		bool initialized;
};

class DefenseLevel : public OuterContext {
	public:
		DefenseLevel(PlayerConfiguration*,std::string mapName);
		~DefenseLevel();
		virtual void frameStep (double deltaTime);
		virtual void eventStep (ALLEGRO_EVENT* event);
		virtual void drawStep ();
		virtual void outerControl();
		virtual Context* copy ();
	protected:
		struct Spawner {
			double spawnRate;
			double spawnTime;
			float x;
			float y;
			std::queue<unsigned int> monsters;
		};
		std::vector<Spawner> spawners;
		std::vector<Entity> entities;
		std::vector<BaseEntity> bases;
		std::vector<Vector2> goals;
		ResourceManager::GameLevelWrapper level;
		float mouseFollowX;
		float mouseFollowY;
		EdgeFollowCamera cam;
		PathRouter pathRouter;
};

#endif
