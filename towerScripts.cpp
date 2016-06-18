#include"towerScripts.h"
#include<cmath>

AIScript::~AIScript() {
}
void AIScript::onClick() {
}
void AIScript::onPoint() {
}
void AIScript::onStep() {
}

WayPathScript::WayPathScript (WayPath newPath,Entity* ent) {
	path = newPath;
	actor = ent;
}
AIScript* WayPathScript::copy () {
	return AIScript::defaultCopy<WayPathScript>(this);
}
void WayPathScript::onStep () {
	if (actor->pos != path.getCurrent()) {
		if (distance2(actor->pos,path.getCurrent()) < actor->base->stats[STAT_SPD]) {
			actor->pos=path.getCurrent();
		} else {
			/*create unit vector*/
			Vector2 velocity = path.getCurrent();
			velocity.x-=actor->pos.x;
			velocity.y-=actor->pos.y;
			float scale = sqrt(velocity.x*velocity.x+velocity.y*velocity.y);
			velocity.x/=scale;
			velocity.y/=scale;
			/*scale up unit vector by speed*/
			velocity.x*=actor->base->stats[STAT_SPD];
			velocity.y*=actor->base->stats[STAT_SPD];

			actor->pos.x+=velocity.x;
			actor->pos.y+=velocity.y;
		}
	} else if (path.isNext()) {
		path.getNext();
	}
}
