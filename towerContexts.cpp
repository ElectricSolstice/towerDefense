#include"towerContexts.h"
#include<iostream>

MainMenu::MainMenu (PlayerConfiguration* curConfig, Background* menuBg) : OuterContext(curConfig) {
	bg.reset(menuBg);
}
MainMenu::MainMenu(const MainMenu& original) : OuterContext(original) {
	bg.reset(original.bg->copy());
}
MainMenu& MainMenu::operator=(const MainMenu& rhs) {
	if (&rhs==this) {
		return *this;
	}
	OuterContext::operator=(rhs);
	bg.reset(rhs.bg->copy());
	return *this;
}
void MainMenu::frameStep (double deltaTime) {
	if (getInner()==NULL) {
		std::vector<std::string> choices;
		choices.push_back("New Game");
		choices.push_back("Options");
		choices.push_back("Scores");
		choices.push_back("Exit");
		ALLEGRO_FONT* font = currentConfig->font.data;
		Context* choice = new ChoiceBox(currentConfig,this,choices,new NullBackground(),false,
				al_get_text_width(font,"New Game  ")*2,al_get_font_line_height(font)*(choices.size()/2),*currentConfig->displayWidth/2,0,
				choices.size()/2,choices.size()/2,0.5,0.0);
		passControl(choice);
	}
	drawStep();
}
void MainMenu::eventStep (ALLEGRO_EVENT* event) {
	if (event->type == ALLEGRO_EVENT_DISPLAY_CLOSE || (event->type==ALLEGRO_EVENT_KEY_DOWN && event->keyboard.keycode == ALLEGRO_KEY_ESCAPE)) {
		currentConfig->quitApp=true;
	}
}
void MainMenu::drawStep () {
	bg->draw(0,0,*currentConfig->displayWidth,*currentConfig->displayHeight);
}
void MainMenu::outerControl() {
	ChoiceBox* menu = (ChoiceBox*)getInner();
	switch(menu->curChoice) {
		case 0:
			releaseInner();
			currentConfig->rootContext->rerootContext(new DefenseLevel(currentConfig,"test.tmx"));
			break;
		case 3:
			currentConfig->quitApp=true;
			break;
		default:
			break;
	}
}
Context* MainMenu::copy () {
	return Context::defaultCopy<MainMenu>(this);
}

DefenseLevel::DefenseLevel(PlayerConfiguration* curConfig,std::string mapName) : OuterContext(curConfig),
	mouseFollowX(0), mouseFollowY(0),
	cam (&mouseFollowX,&mouseFollowY,mouseFollowX,mouseFollowY,currentConfig->displayWidth,
			currentConfig->displayHeight,0,0) {
	level = ResourceManager::loadGameLevel(mapName);
	pathRouter = PathRouter(level);
	int objectCount;
	ALLEGRO_MAP_OBJECT** objects = al_get_objects(al_get_map_layer(level,"Goals"),&objectCount);
	goals.reserve(objectCount);
	for (int i=0;i<objectCount;++i) {
		Vector2 endPoint;
		endPoint.x = std::stoi(al_get_object_property(objects[i],"x","5"));
		endPoint.y = std::stoi(al_get_object_property(objects[i],"y","5"));
		goals.push_back(endPoint);
	}
	al_free(objects);
	objects = al_get_objects(al_get_map_layer(level,"Spawns"),&objectCount);
	spawners.reserve(objectCount);
	std::string prop;
	//TODO load up base entities based on MonsterSet
	//prop = al_get_object_property(objects[i],"MonsterSet","0");
	BaseEntity weakEntity;
	weakEntity.img = ResourceManager::loadImage("data/pumpkin.png");
	weakEntity.stats[STAT_HP] = 25;
	weakEntity.stats[STAT_SPD] = 5;
	weakEntity.stats[STAT_STR] = 25;
	bases.push_back(weakEntity);
	for (int i=0;i<objectCount;++i) {
		Spawner spawner;
		prop = al_get_object_property(objects[i],"SpawnRate","0.0");
		spawner.spawnRate = std::stod(prop,NULL);
		prop = al_get_object_property(objects[i],"SpawnStartTime","0.0");
		spawner.spawnTime = std::stod(prop,NULL);
		prop = al_get_object_property(objects[i],"Monsters","");
		std::size_t pos = prop.find(",");
		assert(pos!=0);
		std::size_t prevPos = 0;
		assert(prevPos!=pos);
		while (prevPos!=pos) {
			spawner.monsters.push(std::stoi(prop.substr(prevPos,pos-prevPos)));
			if (pos!=std::string::npos) {
				prevPos=pos+1;
				pos = prop.find(",",prevPos);
			} else {
				prevPos=pos;
			}
		}
		assert(!spawner.monsters.empty());
		spawner.x = al_get_object_x(objects[i]);
		spawner.y = al_get_object_y(objects[i]);
		spawners.push_back(spawner);
	}
	al_free(objects);
	cam.setMaxBoundary(al_get_map_width(level)*al_get_tile_width(level),al_get_map_height(level)*al_get_tile_height(level));
}
DefenseLevel::~DefenseLevel() {
}
void DefenseLevel::frameStep (double deltaTime) {
	drawStep();
	for (unsigned int i=0;i<spawners.size();++i) {
		if (spawners[i].monsters.empty()) {
			continue;
		}
		spawners[i].spawnTime+=deltaTime;
		while (spawners[i].spawnTime >= spawners[i].spawnRate && !spawners[i].monsters.empty()) {
			Entity spawn = Entity::create(spawners[i].x,spawners[i].y,&bases[spawners[i].monsters.front()]);
			entities.push_back(spawn);
			std::vector<float> weights;
			weights.push_back(1.0f);
			entities[entities.size()-1].script = new WayPathScript(pathRouter.generatePath(spawn.pos, goals[0], weights), 
					&entities[entities.size()-1]);
			spawners[i].monsters.pop();
			spawners[i].spawnTime-=spawners[i].spawnRate;
		}
	}
	for (unsigned int i=0;i<entities.size();++i) {
		entities[i].script->onStep();
	}
}
void DefenseLevel::eventStep (ALLEGRO_EVENT* event) {
	if (event->type == ALLEGRO_EVENT_DISPLAY_CLOSE || (event->type==ALLEGRO_EVENT_KEY_DOWN && event->keyboard.keycode == ALLEGRO_KEY_ESCAPE)) {
		currentConfig->rootContext->rerootContext(new MainMenu(currentConfig,new ImageBackground(ResourceManager::loadImage(BACKGROUND_FILENAME))));
	} else if (event->type==ALLEGRO_EVENT_KEY_DOWN) {
	} else if (event->type==ALLEGRO_EVENT_MOUSE_AXES) {
		mouseFollowX=event->mouse.x;
		mouseFollowY=event->mouse.y;
	}
}
void DefenseLevel::drawStep () {
	al_clear_to_color(al_map_rgb(0,0,0));
	float width =*currentConfig->displayWidth;
	float height=*currentConfig->displayHeight;
	al_draw_map_region(level,cam.getX(),cam.getY(),width,height,0,0,0);
	al_hold_bitmap_drawing(true);
	for (unsigned int i=0;i<bases.size();++i) {
		for (unsigned int j=0;j<entities.size();++j) {
			if (entities[j].base == &(bases[i])) {
				al_draw_bitmap(bases[i].img.data,entities[j].pos.x-cam.getX(),entities[j].pos.y-cam.getY(),0);
			}
		}
	}
	al_hold_bitmap_drawing(false);
}
void DefenseLevel::outerControl() {}
Context* DefenseLevel::copy () {
	return Context::defaultCopy<DefenseLevel>(this);
}
