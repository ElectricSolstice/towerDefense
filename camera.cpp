#include"camera.h"

CameraBase::CameraBase (const int* screenW,const int* screenH) {
	screenWidth = screenW;
	screenHeight = screenH;
}

BoundableCamera::BoundableCamera (float maxXBoundary,float maxYBoundary,float minXBoundary, float minYBoundary) {
	maxX = maxXBoundary;
	maxY = maxYBoundary;
	minX = minXBoundary;
	minY = minYBoundary;
}
void BoundableCamera::setMaxBoundary (float newXBoundary, float newYBoundary) {
	maxX = newXBoundary;
	maxY = newYBoundary;
}
void BoundableCamera::setMinBoundary (float newXBoundary, float newYBoundary) {
	minX = newXBoundary;
	minY = newYBoundary;
}

void PositionableCamera::setPos(float newPosX, float newPosY) {
	camX=newPosX;
	camY=newPosY;
}
PositionableCamera::PositionableCamera(float posX, float posY) {
	setPos(posX,posY);
}
void PositionableCamera::move (float xAmount, float yAmount, float minXBound, float maxXBound, float minYBound, float maxYBound,float screenWidth, float screenHeight) {
	if (camX+xAmount >= minXBound && camX+xAmount+screenWidth <= maxXBound) {
		camX+=xAmount;
	}
	if (camY+yAmount >= minYBound && camY+yAmount+screenHeight <= maxYBound) {
		camY+=yAmount;
	}
}

void FollowableCamera::attach (const float *newX, const float *newY) {
	followedX = newX;
	followedY = newY;
}
FollowableCamera::FollowableCamera (const float* newX, const float* newY) {
	attach(newX,newY);
}

FollowCamera::FollowCamera (const float* newX, const float* newY, const int* screenW,const int* screenH,
		float maxXBoundary,float maxYBoundary,float minXBoundary, float minYBoundary) : 
	CameraBase(screenW,screenH), 
	FollowableCamera(newX,newY),
	BoundableCamera(maxXBoundary,maxYBoundary,minXBoundary,minYBoundary) {
}
float FollowCamera::getX () {
	float camX = *followedX-(*screenWidth/2.0);
	if (camX < minX) {
		return minX;
	//camera's position is top left while max boundary is bottom right,
	//so this compares the camera's position when the max boundary would
	//be drawn at bottom right
	} else if (camX > maxX-(*screenWidth)) {
		return maxX-(*screenWidth);
	} else {
		return camX;
	}
}
float FollowCamera::getY () {
	float camY = *followedY-(*screenHeight/2.0);
	if (camY <= minY) {
		return minY;
	//camera's position is top left while max boundary is bottom right,
	//so this compares the camera's position when the max boundary would
	//be drawn at bottom right
	} else if (camY > maxY-(*screenHeight)) {
		return maxY-(*screenHeight);
	} else {
		return camY;
	}
}

ControlledCamera::ControlledCamera (float posX, float posY, const int* screenW,const int* screenH,
	float maxXBoundary,float maxYBoundary,float minXBoundary,float minYBoundary) :
	CameraBase(screenW,screenH), 
	PositionableCamera(posX,posY),
	BoundableCamera(maxXBoundary,maxYBoundary,minXBoundary,minYBoundary) {
}
float ControlledCamera::getX () {
	return camX;
}
float ControlledCamera::getY () {
	return camY;
}

EdgeFollowCamera::EdgeFollowCamera (const float* folX, const float* folY, float posX, float posY, const int* screenW,const int* screenH,
	float maxXBoundary,float maxYBoundary,float minXBoundary,float minYBoundary,float edgePad,float scrollSpd) : 
	CameraBase(screenW,screenH),
	FollowableCamera(folX,folY),
	BoundableCamera(maxXBoundary,maxYBoundary,minXBoundary,minYBoundary),
	PositionableCamera(posX,posY) {
		edgeBound=edgePad;
		scrollSpeed = scrollSpd;
}
float EdgeFollowCamera::getX () {
	if (camX+edgeBound > (*followedX)) {
		camX-=scrollSpeed;
	} else if ((camX+(*screenWidth))-edgeBound < (*followedX)) {
		camX+=scrollSpeed;
	}
	if (camX <= minX) {
		camX=minX;
	} else if (camX > maxX-(*screenWidth)) {
		camX=maxX-(*screenWidth);
	}
	return camX;
}
float EdgeFollowCamera::getY () {
	if (edgeBound > (*followedY)) {
		camY-=scrollSpeed;
	} else if ((*screenHeight)-edgeBound < (*followedY)) {
		camY+=scrollSpeed;
	}
	if (camY < minY) {
		camY=minY;
	} else if (camY > maxY-(*screenHeight)) {
		camY=maxY-(*screenHeight);
	}
	return camY;
}
