#pragma once

#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>

class Spritesheet {
public:
	Spritesheet();
	Spritesheet(unsigned int textureID, float u, float v, float width, float height);

	void draw(float scale);
	void flipX();
	float getWidth();
	float getHeight();
private:
	unsigned int textureID;
	float u;
	float v;
	float width;
	float height;
};
