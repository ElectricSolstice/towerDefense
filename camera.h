#ifndef GAME_CAMERA
#define GAME_CAMERA

class CameraBase {
	public:
		CameraBase (const int* screenW,const int* screenH);
		//getX and getY should return the top left position of the region that the camera is looking at
		virtual float getX()=0;
		virtual float getY()=0;
	protected:
		const int * screenWidth;
		const int * screenHeight;
};

///////////////
//Camera mixins
///////////////
class BoundableCamera {
	public:
		BoundableCamera (float maxXBoundary,float maxYBoundary, float minXBoundary=0.0,float minYBoundary=0.0);
		void setMaxBoundary (float newXBoundary, float newYBoundary);
		void setMinBoundary (float newXBoundary, float newYBoundary);
	protected:
		float minX;
		float minY;
		float maxX;
		float maxY;
};

class PositionableCamera {
	public:
		PositionableCamera(float posX,float posY);
		void setPos(float newPosX, float newPosY);
		void move (float xAmount, float yAmount, float minXBound, float maxXBound,
				float minYBound, float maxYBound,float screenW, float screenH);
	protected:
		float camX;
		float camY;
};

class FollowableCamera {
	public:
		//uses pointers to make following easier, but don't allow camera to 
		//change the values of what's being followed.
		FollowableCamera (const float* newX, const float* newY);
		//attach to what to follow
		void attach (const float* newX, const float* newY);
	protected:
		const float * followedX;
		const float * followedY;
};

////////////////////////
//Camera Implementations
////////////////////////
class FollowCamera : public CameraBase, public FollowableCamera, public BoundableCamera {
	public:
		FollowCamera (const float* newX, const float* newY, const int* screenW,const int* screenH,
				float maxXBoundary,float maxYBoundary,float minXBoundary=0.0,float minYBoundary=0.0);
		float getX ();
		float getY ();
};

class ControlledCamera : public CameraBase, public PositionableCamera, public BoundableCamera {
	public:
		ControlledCamera (float posX, float posY, const int* screenW,const int* screenH,
				float maxXBoundary,float maxYBoundary,float minXBoundary=0.0,float minYBoundary=0.0);
		float getX ();
		float getY ();
};

class EdgeFollowCamera : public CameraBase, public FollowableCamera, public BoundableCamera, public PositionableCamera {
	public:
		EdgeFollowCamera (const float* folX, const float* folY, float posX, float posY, const int* screenW,const int* screenH,
				float maxXBoundary,float maxYBoundary,float minXBoundary=0.0,float minYBoundary=0.0,float edgePad=15.0,
				float scrollSpd=1.0);
		float getX ();
		float getY ();
	protected:
		float scrollSpeed;
		float edgeBound;
};

#endif
