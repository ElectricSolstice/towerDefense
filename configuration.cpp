#include"configuration.h"

PlayerConfiguration::PlayerConfiguration (Context** current,const int* screenWidth, const int* screenHeight,
		ResourceManager::FontWrapper select,ResourceManager::FontWrapper norm, ALLEGRO_COLOR txtColor,
		ALLEGRO_COLOR selectedTextColor) : selectFont(select), font(norm) {
	textColor = txtColor;
	selectTextColor = selectedTextColor;
	currentContext = current;
	quitApp=false;
	displayWidth=screenWidth;
	displayHeight=screenHeight;
}
