#pragma once //for header files

#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>

#include "general_functions.h"
#include "Spritesheet.h"

#define y_gravity -4.49
#define x_gravity 0
#define PI 3.14159265358979323846

class Entity {
public:
	Entity(float, float, float, GLuint, float, float);
	Entity(Spritesheet, float, float);
	Entity();

	void Draw();
	void ssDraw();

	void setSprite(Spritesheet);
	void setY(float);
	void setX(float);
	void setDirection(int);
	float getX();
	float getY();
	float getWidth();
	float getHeight();
	int getDirection();
	float getScale();
	void setScale(float);
	bool getAirborne();
	void setAirborne(bool a);

	bool collidesWith(Entity*);
	bool topCheck(Entity*);
	bool bottomCheck(Entity*);
	bool leftCheck(Entity*);
	bool rightCheck(Entity*);

	void setCollideTop(bool);
	void setCollideBottom(bool);
	void setCollideLeft(bool);
	void setCollideRight(bool);
	void setSpecialCollideLeft(bool);
	void setSpecialCollideRight(bool);
	bool getSpecialCollideLeft();
	bool getSpecialCollideRight();
	bool getCollideTop();
	bool getCollideBottom();
	bool getCollideLeft();
	bool getCollideRight();

	void setXVel(float);
	void setYVel(float);
	void setXAccel(float);
	void setYAccel(float);
	void setStatic(bool);
	float getXVel();
	float getYVel();
	float getXAccel();
	float getYAccel();
	float getXFriction();
	float getYFriction();
private:
	int direction;
	float x;
	float y;
	float rotation;
	float scale;

	float xVel;
	float yVel;
	float xAccel;
	float yAccel;
	float xFriction;
	float yFriction;

	bool airborne;
	bool specialCollideLeft;
	bool specialCollideRight;
	bool collideTop;
	bool collideBottom;
	bool collideLeft;
	bool collideRight;
	bool isStatic;

	GLuint textureID;

	float width;
	float height;

	Spritesheet sprite;
};