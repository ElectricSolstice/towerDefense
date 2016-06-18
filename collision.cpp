#include"collision.h"

//detect if two bounding boxes collide
bool boxCollision (float xOne, float yOne, float widthOne, float heightOne, \
		float xTwo, float yTwo, float widthTwo, float heightTwo) {
	bool withinX = false;
	bool withinY = false;
	//check if inside the x range
	if (xTwo >= xOne && xTwo <= xOne+widthOne) {
		withinX = true;
	} else if (xOne >= xTwo && xOne <= xTwo+widthTwo) {
		withinX = true;
	}
	else {
		return false;
	}
	//check if inside the y range
	if (yTwo >= yOne && yTwo <= yOne+heightOne){
		withinY = true;
	} else if (yOne >= yTwo && yOne <= yTwo+heightTwo) {
		withinY = true;
	}
	if (withinX and withinY) {
		return true;
	}
	return false;
}
