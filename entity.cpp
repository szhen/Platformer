#include "entity.h"

Entity::Entity(float x, float y, float rot, GLuint textID, float w, float h) : x(x), y(y), rotation(rot), 
textureID(textID), width(w), height(h) {
	xVel = 0.0f;
	yVel = 0.0f;
	xAccel = 0.0f;
	yAccel = 0.0f;
	xFriction = 2.0f;
	yFriction = 2.0f;
}

Entity::Entity(Spritesheet temp, float x, float y) {
	sprite = temp;
	Entity::x = x;
	Entity::y = y;
	rotation = 0.0f;
	textureID = 0;
	width = temp.getWidth();
	height = temp.getHeight();
	direction = direction::LEFT;
	xVel = 0.0f;
	yVel = 0.0f;
	xAccel = 0.0f;
	yAccel = 0.0f;
	xFriction = 2.0f;
	yFriction = 2.0f;
	scale = 1.0f;
	airborne = false;
	isStatic = true;
	specialCollideLeft = false;
	specialCollideRight = false;
	collideTop = false;
	collideBottom = false;
	collideLeft = false;
	collideRight = false;
}

Entity::Entity() {
	x = 0.0f;
	y = 0.0f;
	rotation = 0.0f;

	textureID = 0;

	width = 0.0f;
	height = 0.0f;
}

void Entity::setX(float newX) {	x = newX; }

void Entity::setY(float newY) {	y = newY; }

void Entity::setDirection(int dire) { direction = dire; }

float Entity::getX() { return x; }

float Entity::getY() { return y; }

float Entity::getWidth() { return width; }

float Entity::getHeight() { return height; }

int Entity::getDirection() { return direction; }

void Entity::setScale(float newScale) { scale = newScale; }

float Entity::getScale() { return scale; }
bool Entity::getAirborne() { return airborne; }
void Entity::setAirborne(bool a) { airborne = a; }
void Entity::setSpecialCollideLeft(bool a) { specialCollideLeft = a; }
void Entity::setSpecialCollideRight(bool a) { specialCollideRight = a; }
bool Entity::getSpecialCollideLeft() { return specialCollideLeft; }
bool Entity::getSpecialCollideRight() { return specialCollideRight; }

bool Entity::collidesWith(Entity* other) {
	bool topCheck = y - (height * scale) < other->getY() + (other->getHeight() * other->getScale());
	bool bottomCheck = y + (height * scale) > other->getY() - (other->getHeight()* other->getScale());
	bool leftCheck = x + (width * scale) > other->getX() - (other->getWidth()* other->getScale());
	bool rightCheck = x - (width * scale) < other->getX() + (other->getWidth()* other->getScale());
	return (topCheck && bottomCheck && leftCheck&& rightCheck);
}
bool Entity::topCheck(Entity* other) {
	return y - (height * scale) < other->getY() + (other->getHeight() * other->getScale());
}
bool Entity::bottomCheck(Entity* other) {
	return y + (height * scale) > other->getY() - (other->getHeight() * other->getScale());
}
bool Entity::leftCheck(Entity* other) {
	return x + (width * scale) > other->getX() - (other->getWidth() * other->getScale());
}
bool Entity::rightCheck(Entity* other){
	return x - (width * scale) < other->getX() + (other->getWidth() * other->getScale());
}
void Entity::setCollideTop(bool set) { collideTop = set; }
void Entity::setCollideBottom(bool set) { collideBottom = set; }
void Entity::setCollideLeft(bool set) { collideLeft = set; }
void Entity::setCollideRight(bool set) { collideRight = set; }
bool Entity::getCollideTop() { return collideTop; }
bool Entity::getCollideBottom() { return collideBottom; }
bool Entity::getCollideLeft() { return collideLeft; }
bool Entity::getCollideRight() { return collideRight; }

void Entity::Draw() {
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, textureID);

	glMatrixMode(GL_MODELVIEW);

	glLoadIdentity();
	glTranslatef(x, y, 0.0);
	glRotatef(rotation, 0.0, 0.0, 1.0);

	GLfloat quad[] = { -width * 0.5f, height * 0.5f, -width * 0.5f, -height * 0.5f, width * 0.5f, -height * 0.5f, width * 0.5f, height * 0.5f };
	glVertexPointer(2, GL_FLOAT, 0, quad);
	glEnableClientState(GL_VERTEX_ARRAY);

	GLfloat quadUVs[] = { 0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 1.0, 0.0 };
	glTexCoordPointer(2, GL_FLOAT, 0, quadUVs);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glDrawArrays(GL_QUADS, 0, 4);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisable(GL_TEXTURE_2D);
}

void Entity::ssDraw() {
	glLoadIdentity();
	glTranslatef(x, y, 0.0);
	glRotatef(rotation, 0.0, 0.0, 1.0);
	sprite.draw(Entity::scale);
}

void Entity::setSprite(Spritesheet temp) { sprite = temp; }

void Entity::setXVel(float newVel) { xVel = newVel; }
void Entity::setYVel(float newVel) { yVel = newVel; }
void Entity::setXAccel(float newAccel) { xAccel = newAccel; }
void Entity::setYAccel(float newAccel) { yAccel = newAccel; }
float Entity::getXVel() { return xVel; }
float Entity::getYVel() { return yVel; }
float Entity::getXAccel() { return xAccel; }
float Entity::getYAccel() { return yAccel; }
float Entity::getXFriction() { return xFriction; }
float Entity::getYFriction() { return yFriction; }
void Entity::setStatic(bool newBool) { isStatic = newBool; }