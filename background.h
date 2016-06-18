#ifndef BACKGROUND
#define BACKGROUND

#include"resourceManager.h"

class Background {
	public:
		virtual ~Background();
		virtual void draw (int x, int y, unsigned int width, unsigned int height)=0;
		virtual Background* copy ()=0;
		template<class T> static inline Background* defaultCopy (T* self) {
			return (Background*) new T(*self);
		}
};

class NullBackground : public Background {
	public:
		virtual void draw (int, int, unsigned int, unsigned int);
		virtual Background* copy ();
};

class ImageBackground : public Background {
	public:
		ImageBackground (ResourceManager::ImageWrapper img);
		virtual void draw (int, int, unsigned int, unsigned int);
		virtual Background* copy ();
	protected:
		ResourceManager::ImageWrapper image;
		unsigned int imageWidth;
		unsigned int imageHeight;
};

#endif
