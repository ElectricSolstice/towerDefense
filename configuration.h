#ifndef GAME_CONFIGURATION
#define GAME_CONFIGURATION
#include<allegro5/allegro_font.h>
#include"resourceManager.h"
#include<map>

class RootContextHolder;
class Context;
class PlayerConfiguration {
	public:
		PlayerConfiguration (Context** current,const int* screenWidth, const int* screenHeight,
				ResourceManager::FontWrapper select, ResourceManager::FontWrapper norm,
				ALLEGRO_COLOR txtColor, ALLEGRO_COLOR selectedTextColor);
		ResourceManager::FontWrapper selectFont;
		ResourceManager::FontWrapper font;
		ALLEGRO_COLOR textColor;
		ALLEGRO_COLOR selectTextColor;
		Context** currentContext;
		RootContextHolder* rootContext;
		bool quitApp;
		const int* displayWidth;
		const int* displayHeight;
		std::map<int,int> keyMap;
};

#endif
