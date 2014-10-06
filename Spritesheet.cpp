#include "Spritesheet.h"

Spritesheet::Spritesheet(unsigned int textureID, float u, float v, float width, float height)
	: textureID(textureID), u(u), v(v), width(width), height(height) {}

Spritesheet::Spritesheet() {
	textureID = 0;
	u = 0.0f;
	v = 0.0f;
	width = 0.0f;
	height = 0.0f;
}

void Spritesheet::draw(float scale) {
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, textureID);

	GLfloat quad[] = { -width * scale, height * scale, -width * scale, -height * scale, width * scale, -height * scale, width * scale, height * scale };
	glVertexPointer(2, GL_FLOAT, 0, quad);
	glEnableClientState(GL_VERTEX_ARRAY);

	GLfloat quadUVs[] = { u, v, u, v + height, u + width, v + height, u + width, v };
	glTexCoordPointer(2, GL_FLOAT, 0, quadUVs);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glDrawArrays(GL_QUADS, 0, 4);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisable(GL_TEXTURE_2D);
}

void Spritesheet::flipX() {
	u = -u;
}

float Spritesheet::getWidth() {
	return width;
}

float Spritesheet::getHeight() {
	return height;
}