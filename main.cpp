#include<allegro5/allegro.h>
#include<allegro5/allegro_image.h>
#include<allegro5/allegro_font.h>
#include<allegro5/allegro_ttf.h>
#include<allegro5/allegro_tiled.h>
#include<memory>
#include<ctime>
#include"towerContexts.h"
#include"resourceManager.h"

int displayHeight = 600;
int displayWidth = 1400;
const int FPS = 60;
//FunctionTable globalFunctionTable;

void initGlobalFunctionTable() {
}

void defaultKeyMapping (PlayerConfiguration* conf) {
	conf->keyMap[ALLEGRO_KEY_UP]=ALLEGRO_KEY_UP;
	conf->keyMap[ALLEGRO_KEY_DOWN]=ALLEGRO_KEY_DOWN;
	conf->keyMap[ALLEGRO_KEY_LEFT]=ALLEGRO_KEY_LEFT;
	conf->keyMap[ALLEGRO_KEY_RIGHT]=ALLEGRO_KEY_RIGHT;
	conf->keyMap[ALLEGRO_KEY_SPACE]=ALLEGRO_KEY_SPACE;
}

int main() {
	if(!al_init()){
		return -1;
	}

	//al_set_new_display_flags(ALLEGRO_FULLSCREEN_WINDOW);
	ALLEGRO_DISPLAY *display = al_create_display(displayWidth,displayHeight);
	if(!display){
		return -1;
	}

	if(!al_install_keyboard()) {
		al_destroy_display(display);
		return -1;
	}
	if(!al_install_mouse()) {
		al_destroy_display(display);
		return -1;
	}
	ALLEGRO_EVENT_QUEUE *event_queue = al_create_event_queue();
	if(!event_queue){
		al_destroy_display(display);
		return -1;
	}

	ALLEGRO_TIMER *timer =al_create_timer(1.0/FPS);
	if(!timer){
		al_destroy_display(display);
		al_destroy_event_queue(event_queue);
		return -1;
	}
	al_init_image_addon();
	al_init_font_addon();
	al_init_ttf_addon();
	al_set_system_mouse_cursor(display,ALLEGRO_SYSTEM_MOUSE_CURSOR_DEFAULT);
	al_show_mouse_cursor(display);
	al_register_event_source(event_queue, al_get_display_event_source(display));
	al_register_event_source(event_queue, al_get_timer_event_source(timer));
	al_register_event_source(event_queue, al_get_keyboard_event_source());
	al_register_event_source(event_queue, al_get_mouse_event_source());

	al_clear_to_color(al_map_rgb(0,0,0));

	al_flip_display();

	bool redraw = false;
	al_start_timer(timer);

	initGlobalFunctionTable();

	Context* currentContext;
	PlayerConfiguration config(&currentContext,&displayWidth,&displayHeight,ResourceManager::loadFont("data/Sofia-Regular.otf",-32,0),
			ResourceManager::loadFont("data/Sofia-Regular.otf",-32,0),al_map_rgb(0,255,255),al_map_rgb(0,255,0));
	defaultKeyMapping(&config);
	currentContext=new MainMenu(&config,new ImageBackground(ResourceManager::loadImage(BACKGROUND_FILENAME)));
       	RootContextHolder rootContext(currentContext,&currentContext);
	
	config.rootContext = &rootContext;
	srand(time(NULL));
	double previousTime = al_get_time();
	while(!config.quitApp) {
		ALLEGRO_EVENT ev;
		al_wait_for_event(event_queue,&ev);
		if (ev.type == ALLEGRO_EVENT_TIMER) {
			redraw = true;
		} else if (ev.type == ALLEGRO_EVENT_KEY_UP || ev.type == ALLEGRO_EVENT_KEY_DOWN) {
			//performs key mapping outside contexts, so contexts don't have to worry about converting
			if (config.keyMap.count(ev.keyboard.keycode)) {
				ev.keyboard.keycode = config.keyMap[ev.keyboard.keycode];
			}
		}
		currentContext->eventStep(&ev);
		if (redraw == true and al_is_event_queue_empty(event_queue)) {
			double deltaTime = al_get_time()-previousTime;
			previousTime+=deltaTime;
			currentContext->frameStep(deltaTime);
			al_flip_display();
			redraw = false;
		}

	}

	al_destroy_display(display);
	al_destroy_event_queue(event_queue);
	al_destroy_timer(timer);

	return 1;
}
