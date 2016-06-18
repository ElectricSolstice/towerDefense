#include"background.h"

Background::~Background() {}

void NullBackground::draw(int, int, unsigned int, unsigned int) {}
Background* NullBackground::copy () {
	return Background::defaultCopy<NullBackground>(this);
}

ImageBackground::ImageBackground(ResourceManager::ImageWrapper img) : image(img) {
	imageWidth = al_get_bitmap_width(image);
	imageHeight = al_get_bitmap_height(image);
}
void ImageBackground::draw(int x, int y, unsigned int width, unsigned int height) {
	if (image!=NULL) {
		al_draw_scaled_bitmap(image,0,0,imageWidth,imageHeight,x,y,width,height,0x0);
	}
}
Background* ImageBackground::copy () {
	return Background::defaultCopy<ImageBackground>(this);
}
