#pragma once

#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include <random>
#include <ctime>
#include <vector>
#include <string>

namespace direction {
	enum directions { LEFT, RIGHT };
}

GLuint LoadTexture(const char *, GLuint);

void drawText(GLuint, std::string, float, float, float, float, float, float);

float lerp(float, float, float);