#pragma once

#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>

#include "entity.h"
#include "Spritesheet.h"
#include "general_functions.h"

#define MAX_BLOCKS 250
#define FIXED_TIMESTEP 0.0166666f
#define MAX_BULLETS 25
#define MAX_ENEMIES 15
#define MAX_TIMESTEPS 6
#define RANDOM_DIRECTION (direction::directions)rand()%2

class Application {
public:
	Application();
	~Application();

	void init();
	bool UpdateAndRender();
	void update(float);
	void render();
private:
	SDL_Window* displayWindow;
	SDL_Event event;



	const Uint8 *keys;
	GLuint pokemonID;
	float lastFrameTicks;
	float timeLeftOver;
	float shootCD;
	float spawnProtection;
	float spawnEnemy;

	int playerFacing;
	int gameTileIndex;
	int projectileIndex;
	int enemyIndex;
	Entity* gameTile[MAX_BLOCKS];
	Entity* enemies[MAX_ENEMIES];
	Entity* bullets[MAX_BULLETS];
	Entity player;

	void createGameLevel();
	void renderGameLevel();
	void renderBullets();
	void renderEnemies();
	void FixedUpdate();
	void checkEnemyCollision(Entity*, Entity*);
	void collideLava();
	void updateCollision();
};