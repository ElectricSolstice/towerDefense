#include"resourceManager.h"
#include"resourceManagerDefineResource.h"
#include<allegro5/allegro_image.h>

#define COM_SEP ,

namespace ResourceManager {
	MANAGER_DEFINE_RESOURCE(ALLEGRO_BITMAP,Image,al_load_bitmap(fileName.c_str()),al_destroy_bitmap(data),fileName,)
	MANAGER_DEFINE_RESOURCE(ALLEGRO_FONT,Font,al_load_font(fileName.c_str(),fontSize,flags), al_destroy_font(data),
			fileName+std::to_string(fontSize)+"_"+std::to_string(flags),
			COM_SEP int fontSize COM_SEP int flags)
}

#undef COM_SEP
