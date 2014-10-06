#include "Application.h"

Application::Application() {
	init();
	createGameLevel();
}

Application::~Application() {
	SDL_Quit();
}

void Application::init() {
	SDL_Init(SDL_INIT_VIDEO);
	displayWindow = SDL_CreateWindow("My Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);
	glViewport(0, 0, 800, 600);
	glMatrixMode(GL_PROJECTION);
	glOrtho(-2.66, 2.66, -2.0, 2.0, -2.0, 2.0);
	
	srand(time(NULL));
	pokemonID = LoadTexture("sprites.png", GL_RGBA);
	Spritesheet temp(pokemonID, 0.0f / 128.0f, 86.0f / 128.0f, 28.0f / 128.0f, 32.0f / 128.0f);
	Entity playerTemp(temp, 0.0f, 0.0f);
	player = playerTemp;
	player.setStatic(false);
	player.setScale(0.5f);
	spawnEnemy = 0.0f;
	playerFacing = direction::RIGHT;
	spawnProtection = 0.0f;
	lastFrameTicks = 0.0f;
	timeLeftOver = 0.0f;
	shootCD = 0.0f;
	gameTileIndex = 0;
	projectileIndex = 0;
	enemyIndex = 0;
	keys = SDL_GetKeyboardState(NULL);
}

bool Application::UpdateAndRender() {
	float ticks = (float)SDL_GetTicks() / 1000.0f;
	float elapsed = ticks - lastFrameTicks;
	lastFrameTicks = ticks;
	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
			return true;
		}
	}
	// timestep 
	float fixedElapsed = elapsed + timeLeftOver;
	if (fixedElapsed > FIXED_TIMESTEP * MAX_TIMESTEPS) {
		fixedElapsed = FIXED_TIMESTEP * MAX_TIMESTEPS;
	}
	while (fixedElapsed >= FIXED_TIMESTEP) {
		fixedElapsed -= FIXED_TIMESTEP;
		FixedUpdate();
	}
	timeLeftOver = fixedElapsed;
	
	update(elapsed);
	render();
	return false;
}

void Application::update(float elapsed) {
	//player.setXVel(player.getXVel() + player.getXAccel() * elapsed);
	player.setXAccel(0.0f);
	if (!player.getAirborne()) {
		if (keys[SDL_SCANCODE_UP]) {
			player.setYVel(3.5f);
		}
	}
	if (keys[SDL_SCANCODE_LEFT]) {
		player.setXAccel(-3.5f);
		playerFacing = direction::LEFT;
	}
	else if (keys[SDL_SCANCODE_RIGHT]) {
		player.setXAccel(3.5f);
		playerFacing = direction::RIGHT;
	}
	// shooting logic
	if (keys[SDL_SCANCODE_SPACE]) {
		if (projectileIndex < MAX_BULLETS - 1) {
			if (shootCD > 0.1f) { // can shoot every .5 seconds
				if (bullets[projectileIndex] != NULL)
					delete bullets[projectileIndex]; // prevent memory leak
				bullets[projectileIndex] = new Entity(Spritesheet(pokemonID, 0.0f / 128.0f, 52.0f / 128.0f, 32.0f / 128.0f, 32.0f / 128.0f), player.getX(), player.getY());
				if (playerFacing == direction::RIGHT)
					bullets[projectileIndex]->setXVel(3.0f);
				else
					bullets[projectileIndex]->setXVel(-3.0f);
				bullets[projectileIndex]->setScale(0.25f);
				projectileIndex++;
				shootCD = 0.0f;
			}
		}
		else {
			if (shootCD > 0.1f) { // can shoot every .5 seconds
				projectileIndex = 0;
				if (bullets[projectileIndex] != NULL)
					delete bullets[projectileIndex];
				bullets[projectileIndex] = new Entity(Spritesheet(pokemonID, 0.0f / 128.0f, 52.0f / 128.0f, 32.0f / 128.0f, 32.0f / 128.0f), player.getX(), player.getY());
				if (playerFacing == direction::RIGHT)
					bullets[projectileIndex]->setXVel(3.0f);
				else
					bullets[projectileIndex]->setXVel(-3.0f);
				bullets[projectileIndex]->setScale(0.25f);
				projectileIndex++;
				shootCD = 0.0f;
			}
		}
	}
	// update bullets
	for (int i = 0; i < MAX_BULLETS - 1; ++i) {
		if (bullets[i] != NULL) {
			bullets[i]->setX(bullets[i]->getX() + bullets[i]->getXVel() * elapsed);
		}
	}
	// initialize the enemy and update
	for (int i = 0; i < MAX_ENEMIES - 1; ++i) {
		if (enemies[i] == NULL) {
			if (spawnEnemy > 0.75f) {
				spawnEnemy = 0.0f;
				enemies[i] = new Entity(Spritesheet(pokemonID, 0.0f / 128.0f, 0.0f / 128.0f, 51.0f / 128.0f, 50.0f / 128.0f), 0.0f, 1.75f);
				enemies[i]->setScale(0.5f);
				enemies[i]->setDirection(RANDOM_DIRECTION);
				if (enemies[i]->getDirection() == direction::RIGHT)
					enemies[i]->setXVel(1.5f);
				else
					enemies[i]->setXVel(-1.5f);
			}
		}
		else {
			enemies[i]->setX(enemies[i]->getX() + enemies[i]->getXVel() * elapsed);
			enemies[i]->setY(enemies[i]->getY() + enemies[i]->getYVel() * elapsed);
		}
	}
	// bullet collision
	for (int i = 0; i < MAX_BULLETS - 1; ++i) {
		if (bullets[i] != NULL) {
			for (int j = 0; j < MAX_ENEMIES - 1; ++j) {
				if (enemies[j] != NULL) {
					if (bullets[i]->collidesWith(enemies[j])) {
						delete bullets[i];
						bullets[i] = NULL;
						enemies[j]->setX(0.0f);
						enemies[j]->setY(1.75f);
						break;
					}
				}
			}
		}
	}
	// if player collide with enemy
	for (int i = 0; i < MAX_ENEMIES - 1; ++i) {
		if (enemies[i] != NULL) {
			if (player.collidesWith(enemies[i]) && spawnProtection > 1.0f) {
				player.setX(0.0f);
				player.setY(0.0f);
				spawnProtection = 0.0f;
			}
		}
	}
	collideLava();
	player.setX(player.getX() + player.getXVel() * elapsed);
	player.setY(player.getY() + player.getYVel() * elapsed);
	shootCD += elapsed;
	spawnEnemy += elapsed;
	spawnProtection += elapsed;
}

void Application::render() {
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);

	renderGameLevel();
	player.ssDraw();
	renderBullets();
	renderEnemies();
	SDL_GL_SwapWindow(displayWindow);
}	

void Application::FixedUpdate() {
	player.setAirborne(true);
	player.setXVel(lerp(player.getXVel(), 0.0f, FIXED_TIMESTEP * player.getXFriction()));
	player.setYVel(lerp(player.getYVel(), 0.0f, FIXED_TIMESTEP * player.getYFriction()));
	player.setXVel(player.getXVel() + player.getXAccel() * FIXED_TIMESTEP);
	player.setYVel(player.getYVel() + player.getYAccel() * FIXED_TIMESTEP);
	player.setXVel(player.getXVel() + x_gravity * FIXED_TIMESTEP);
	player.setYVel(player.getYVel() + y_gravity * FIXED_TIMESTEP);
	// enemies gravity logic
	for (int i = 0; i < MAX_ENEMIES - 1; ++i) {
		if (enemies[i] != NULL) {
			enemies[i]->setXVel(enemies[i]->getXVel() + x_gravity * FIXED_TIMESTEP);
			enemies[i]->setYVel(enemies[i]->getYVel() + y_gravity * FIXED_TIMESTEP);
		}
	}

	for (int i = 0; i < MAX_BLOCKS; ++i) {
		if (gameTile[i] != NULL && gameTile[i + 1] != NULL) {
			if (player.collidesWith(gameTile[i]) && (gameTile[i]->getY() == gameTile[i + 1]->getY() && player.getY() > gameTile[i]->getY())) {
				player.setCollideBottom(true);
				player.setAirborne(false);
				break;
			}
			else if (player.collidesWith(gameTile[i]) && (gameTile[i]->getX() == gameTile[i + 1]->getX() && player.getX() < gameTile[i]->getX())) {
				player.setCollideRight(true);
				break;
			}

			else if (player.collidesWith(gameTile[i]) && (gameTile[i]->getX() == gameTile[i + 1]->getX() && player.getX() > gameTile[i]->getX())) {
				player.setCollideLeft(true);
				break;
			}

			else if (player.collidesWith(gameTile[i]) && (gameTile[i]->getY() == gameTile[i + 1]->getY() && player.getY() < gameTile[i]->getY())) {
				player.setCollideTop(true);
				break;
			}
			// buggy code!!
			/*
			else if (gameTile[i - 1] != NULL && player.collidesWith(gameTile[i]) && gameTile[i - 1]->getY() == gameTile[i]->getY() && gameTile[i + 1]->getY() != gameTile[i]->getY() && player.getX() < gameTile[i]->getX() && player.getAirborne()
				&& (player.getY() + player.getHeight() < gameTile[i]->getY() + gameTile[i]->getHeight() || player.getY() - player.getHeight() < gameTile[i]->getY() - gameTile[i]->getHeight()
				|| (player.getY() + player.getHeight() > gameTile[i]->getY() + gameTile[i]->getHeight() && player.getY() - player.getHeight() > gameTile[i]->getY() - gameTile[i]->getHeight()))) {
				player.setSpecialCollideRight(true);
				break;
			}
			else if (player.collidesWith(gameTile[i]) && gameTile[i]->getY() == gameTile[i + 1]->getY() && player.getX() > gameTile[i]->getX() && player.getAirborne()
				&& (player.getY() + player.getHeight() < gameTile[i]->getY() + gameTile[i]->getHeight() || player.getY() - player.getHeight() < gameTile[i]->getY() - gameTile[i]->getHeight()
				|| (player.getY() + player.getHeight() > gameTile[i]->getY() + gameTile[i]->getHeight() && player.getY() - player.getHeight() > gameTile[i]->getY() - gameTile[i]->getHeight()))) {
				player.setSpecialCollideLeft(true);
				break;
			}*/
		}
	}
	// enemy physics collision logic
	for (int i = 0; i < MAX_BLOCKS; ++i) {
		if (gameTile[i] != NULL && gameTile[i+1] != NULL) {
			checkEnemyCollision(gameTile[i], gameTile[i + 1]);
		}
	}

	// fixing player collision
	if (player.getCollideBottom()) {
		for (int i = 0; i < MAX_BLOCKS; ++i) {
			if (gameTile[i] != NULL && gameTile[i + 1] != NULL) {
				if (player.collidesWith(gameTile[i]) && (gameTile[i]->getY() == gameTile[i + 1]->getY() && player.getY() > gameTile[i]->getY())) {
					float ypenetration = fabs((player.getY() - (player.getHeight() * player.getScale())) - (gameTile[i]->getY() + (gameTile[i]->getHeight() * gameTile[i]->getScale())));
					player.setY(player.getY() + ypenetration + 0.0001f);
					break;
				}
			}
		}
		player.setYVel(0.0f);
		player.setCollideBottom(false);
	}
	else if (player.getCollideTop()) {
		for (int i = 0; i < MAX_BLOCKS; ++i) {
			if (gameTile[i] != NULL && gameTile[i + 1] != NULL) {
				if (player.collidesWith(gameTile[i]) && (gameTile[i]->getY() == gameTile[i + 1]->getY() && player.getY() < gameTile[i]->getY())) {
					float ypenetration = fabs((player.getY() + player.getHeight() * player.getScale()) - (gameTile[i]->getY() - gameTile[i]->getHeight() * gameTile[i]->getScale()));
					player.setY(player.getY() - ypenetration - 0.0001f);
					break;
				}
			}
		}
		player.setYVel(0.0f);
		player.setCollideTop(false);
	}
	else if (player.getCollideLeft()) {
		for (int i = 0; i < MAX_BLOCKS; ++i) {
			if (gameTile[i] != NULL && gameTile[i + 1] != NULL) {
				if (player.collidesWith(gameTile[i]) && (gameTile[i]->getX() == gameTile[i + 1]->getX() && player.getX() > gameTile[i]->getX())) {
					float xpenetration = fabs((player.getX() - player.getWidth() * player.getScale()) - (gameTile[i]->getX() + gameTile[i]->getWidth() * gameTile[i]->getScale()));
					player.setX(player.getX() + xpenetration + 0.0001f);
					break;
				}
			}
		}
		player.setXVel(0.0f);
		player.setCollideLeft(false);
	}
	else if (player.getCollideRight()) {
		for (int i = 0; i < MAX_BLOCKS; ++i) {
			if (gameTile[i] != NULL && gameTile[i + 1] != NULL) {
				if (player.collidesWith(gameTile[i]) && (gameTile[i]->getX() == gameTile[i + 1]->getX() && player.getX() < gameTile[i]->getX())) {
					float xpenetration = fabs((player.getX() + player.getWidth() * player.getScale()) - (gameTile[i]->getX() - gameTile[i]->getWidth() * gameTile[i]->getScale()));
					player.setX(player.getX() - xpenetration - 0.0001f);
					break;
				}
			}
		}
		player.setXVel(0.0f);
		player.setCollideRight(false);
	}
	else if (player.getSpecialCollideLeft()) {
		for (int i = 0; i < MAX_BLOCKS; ++i) {
			if (gameTile[i] != NULL && gameTile[i + 1] != NULL) {
				if (player.collidesWith(gameTile[i]) && gameTile[i]->getY() == gameTile[i + 1]->getY() && player.getX() > gameTile[i]->getX()) {
					float xpenetration = fabs((player.getX() - player.getWidth() * player.getScale()) - (gameTile[i]->getX() + gameTile[i]->getWidth() * gameTile[i]->getScale()));
					player.setX(player.getX() + xpenetration + 0.0001f);
					break;
				}
			}
		}
		player.setXVel(0.0f);
		player.setSpecialCollideLeft(false);
	}
	else if (player.getSpecialCollideRight()) {
		for (int i = 0; i < MAX_BLOCKS; ++i) {
			if (gameTile[i] != NULL && gameTile[i + 1] != NULL) {
				if (gameTile[i - 1] != NULL && player.collidesWith(gameTile[i]) && gameTile[i - 1]->getY() == gameTile[i]->getY() && gameTile[i + 1]->getY() != gameTile[i]->getY() && player.getX() < gameTile[i]->getX()) {
					float xpenetration = fabs((player.getX() - player.getWidth() * player.getScale()) - (gameTile[i]->getX() + gameTile[i]->getWidth() * gameTile[i]->getScale()));
					player.setX(player.getX() - xpenetration + 0.0001f);
					break;
				}
			}
		}
		player.setXVel(0.0f);
		player.setSpecialCollideRight(false);
	}
	updateCollision();
	// bullet collision and update
	for (int i = 0; i < MAX_BULLETS - 1; ++i) {
		for (int j = 0; j < MAX_BLOCKS - 1; ++j) {
			if (gameTile[j] != NULL) {
				if (bullets[i] != NULL) {
					if (bullets[i]->collidesWith(gameTile[j])) {
						delete bullets[i];
						bullets[i] = NULL;
					}
					else {
						bullets[i]->setXVel(bullets[i]->getXVel() + bullets[i]->getXAccel() * FIXED_TIMESTEP);
					}
				}
			}
		}
	}
}


void Application::createGameLevel() {
	GLuint tileID = LoadTexture("tiles_spritesheet.png", GL_RGBA);

	Spritesheet temp1(tileID, 792.0f / 914.0f, 144.0f / 936.0f, 70.0f / 914.0f, 70.0f / 936.0f);
	Spritesheet castleLeft(tileID, 792.0f / 914.0f, 216.0f / 936.0f, 70.0f / 914.0f, 70.0f / 936.0f);
	Spritesheet castleRight(tileID, 792.0f / 914.0f, 72.0f / 936.0f, 70.0f / 914.0f, 70.0f / 936.0f);
	Spritesheet castleCenter(tileID, 504.0f / 914.0f, 288.0f / 936.0f, 70.0f / 914.0f, 70.0f / 936.0f);
	Spritesheet sandMid(tileID, 288.0f / 914.0f, 576.0f / 936.0f, 70.0f / 914.0f, 70.0f / 936.0f);
	Spritesheet lava(tileID, 432.0f / 914.0f, 792.0f / 936.0f, 70.0f / 914.0f, 70.0f / 936.0f);
	// called out of order for testing purposes
	// lava, from gameTile[0-35]
	for (int i = 0; i < 36; ++i, ++gameTileIndex) {
		gameTile[gameTileIndex] = new Entity(lava, -2.66f + (i*0.153f), -1.988f);
	}
	// top left platform
	for (int i = 0; i < 12; ++i, ++gameTileIndex) {
		if (i == 11)
			gameTile[gameTileIndex] = new Entity(castleRight, -2.5f + (i*0.153f), 1.75f);
		else
			gameTile[gameTileIndex] = new Entity(temp1, -2.5f + (i*0.153f), 1.75f);
	}
	// left vertical platform
	for (int i = 0; i < 22; ++i, ++gameTileIndex) {
		gameTile[gameTileIndex] = new Entity(castleCenter, -2.5f, 1.60f + (i*-0.1495f));
	}
	// top right platform
	for (int i = 0; i < 12; ++i, ++gameTileIndex) {
		if (i == 0)
			gameTile[gameTileIndex] = new Entity(castleLeft, 0.815f + (i*0.153f), 1.75f);
		else
			gameTile[gameTileIndex] = new Entity(temp1, 0.815f + (i*0.153f), 1.75f);
	}
	// right vertical platform
	for (int i = 0; i < 22; ++i, ++gameTileIndex) {
		gameTile[gameTileIndex] = new Entity(castleCenter, 2.499f, 1.60f + (i*-0.1495f));
	}
	// bottom right platform
	for (int i = 0; i < 10; ++i, ++gameTileIndex) {
		gameTile[gameTileIndex] = new Entity(castleCenter, 1.121f + (i*0.153f), -1.689f);
	}
	// inner right platform
	for (int i = 0; i < 10; ++i, ++gameTileIndex) {
		gameTile[gameTileIndex] = new Entity(castleCenter, 1.121f + (i*0.153f), -0.3f);
	}
	// bottom left platform
	for (int i = 0; i < 10; ++i, ++gameTileIndex) {
		gameTile[gameTileIndex] = new Entity(castleCenter, -2.5f + (i*0.153f), -1.689f);
	}
	// inner upper middle platform
	for (int i = 0; i < 14; ++i, ++gameTileIndex) {
		gameTile[gameTileIndex] = new Entity(sandMid, -1.0f + (i*0.153f), 0.4f);
	}
	// bottom sand left
	for (int i = 0; i < 15; ++i, ++gameTileIndex) {
		gameTile[gameTileIndex] = new Entity(sandMid, -2.5f + (i*0.153f), -1.8385f);
	}
	// inner left platform
	for (int i = 0; i < 10; ++i, ++gameTileIndex) {
		gameTile[gameTileIndex] = new Entity(castleCenter, -2.5f + (i*0.153f), -0.3f);
	}
	// bottom sand right
	for (int i = 0; i < 15; ++i, ++gameTileIndex) {
		gameTile[gameTileIndex] = new Entity(sandMid, 0.356f + (i*0.153f), -1.8385f);
	}
	// lower upper middle platform
	for (int i = 0; i < 14; ++i, ++gameTileIndex) {
		gameTile[gameTileIndex] = new Entity(sandMid, -1.0f + (i*0.153f), -1.0f);
	}
}

void Application::renderGameLevel() {
	for (int i = 0; i < MAX_BLOCKS; ++i) {
		if (gameTile[i] != NULL) {
			gameTile[i]->ssDraw();
		}
	}
}

void Application::renderBullets() {
	for (int i = 0; i < MAX_BULLETS - 1; ++i) {
		if (bullets[i] != NULL) {
			bullets[i]->ssDraw();
		}
	}
}

void Application::renderEnemies() {
	for (int i = 0; i < MAX_ENEMIES - 1; ++i) {
		if (enemies[i] != NULL) {
			enemies[i]->ssDraw();
		}
	}
}

void Application::checkEnemyCollision(Entity* temp, Entity* tempNext) {
	for (int i = 0; i < MAX_ENEMIES - 1; ++i) {
		if (enemies[i] != NULL) {
			if (enemies[i]->collidesWith(temp) && (temp->getY() == tempNext->getY() && enemies[i]->getY() > temp->getY())) {
				enemies[i]->setCollideBottom(true);
				break;
			}
			else if (enemies[i]->collidesWith(temp) && (temp->getX() == tempNext->getX() && enemies[i]->getX() < temp->getX())) {
				enemies[i]->setCollideRight(true);
				break;
			}

			else if (enemies[i]->collidesWith(temp) && (temp->getX() == tempNext->getX() && enemies[i]->getX() > temp->getX())) {
				enemies[i]->setCollideLeft(true);
				break;
			}

			else if (enemies[i]->collidesWith(temp) && (temp->getY() == tempNext->getY() && enemies[i]->getY() < temp->getY())) {
				enemies[i]->setCollideTop(true);
				break;
			}
		}
	}
}

void Application::updateCollision() {
	for (int i = 0; i < MAX_ENEMIES - 1; ++i) {
		if (enemies[i] != NULL) {
			if (enemies[i]->getCollideBottom()) {
				for (int j = 0; j < MAX_BLOCKS ; ++j) {
					if (gameTile[j] != NULL && gameTile[j + 1] != NULL) {
						if (enemies[i]->collidesWith(gameTile[j]) && (gameTile[j]->getY() == gameTile[j + 1]->getY() && enemies[i]->getY() > gameTile[j]->getY())) {
							float ypenetration = fabs((enemies[i]->getY() - (enemies[i]->getHeight() * enemies[i]->getScale())) - (gameTile[j]->getY() + (gameTile[j]->getHeight() * gameTile[j]->getScale())));
							enemies[i]->setY(enemies[i]->getY() + ypenetration + 0.0001f);
							break;
						}
					}
				}
				enemies[i]->setYVel(0.0f);
				enemies[i]->setCollideBottom(false);
			}
			else if (enemies[i]->getCollideTop()) {
				for (int j = 0; j < MAX_BLOCKS; ++j) {
					if (gameTile[j] != NULL && gameTile[j + 1] != NULL) {
						if (enemies[i]->collidesWith(gameTile[j]) && (gameTile[j]->getY() == gameTile[j + 1]->getY() && enemies[i]->getY() < gameTile[j]->getY())) {
							float ypenetration = fabs((enemies[i]->getY() + enemies[i]->getHeight() * enemies[i]->getScale()) - (gameTile[j]->getY() - gameTile[j]->getHeight() * gameTile[j]->getScale()));
							enemies[i]->setY(enemies[i]->getY() - ypenetration - 0.0001f);
							break;
						}
					}
				}
				enemies[i]->setYVel(0.0f);
				enemies[i]->setCollideTop(false);
			}
			else if (enemies[i]->getCollideLeft()) {
				for (int j = 0; j < MAX_BLOCKS; ++j) {
					if (gameTile[j] != NULL && gameTile[j + 1] != NULL) {
						if (enemies[i]->collidesWith(gameTile[j]) && (gameTile[j]->getX() == gameTile[j + 1]->getX() && enemies[i]->getX() > gameTile[j]->getX())) {
							float xpenetration = fabs((enemies[i]->getX() - enemies[i]->getWidth() * enemies[i]->getScale()) - (gameTile[j]->getX() + gameTile[j]->getWidth() * gameTile[j]->getScale()));
							enemies[i]->setX(enemies[i]->getX() + xpenetration + 0.0001f);
							break;
						}
					}
				}
				enemies[i]->setXVel(-1.0f * enemies[i]->getXVel());
				enemies[i]->setDirection(direction::RIGHT);
				enemies[i]->setCollideLeft(false);
			}
			else if (enemies[i]->getCollideRight()) {
				for (int j = 0; j < MAX_BLOCKS; ++j) {
					if (gameTile[j] != NULL && gameTile[j + 1] != NULL) {
						if (enemies[i]->collidesWith(gameTile[j]) && (gameTile[j]->getX() == gameTile[j + 1]->getX() && enemies[i]->getX() < gameTile[j]->getX())) {
							float xpenetration = fabs((enemies[i]->getX() + enemies[i]->getWidth() * enemies[i]->getScale()) - (gameTile[j]->getX() - gameTile[j]->getWidth() * gameTile[j]->getScale()));
							enemies[i]->setX(enemies[i]->getX() - xpenetration - 0.0001f);
							break;
						}
					}
				}
				enemies[i]->setXVel(-1.0f * enemies[i]->getXVel());
				enemies[i]->setDirection(direction::LEFT);
				enemies[i]->setCollideRight(false);
			}
		}
	}
}

void Application::collideLava() {
	if (player.getY() - player.getHeight() * 0.5f < gameTile[0]->getY() + gameTile[0]->getHeight()) {
		player.setX(0.0f);
		player.setY(0.0f);
	}
	for (int i = 0; i < MAX_ENEMIES - 1; ++i) {
		if (enemies[i] != NULL) {
			if (enemies[i]->getY() - enemies[i]->getHeight() * 0.75f < gameTile[0]->getY() + gameTile[0]->getHeight() * 0.75f) {
				enemies[i]->setX(0.0f);
				enemies[i]->setY(1.75f);
			}
		}
	}
}