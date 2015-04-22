#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <gl/glew.h>
#include <gl/GL.h>
#include "glm/glm/glm.hpp"
#include "glm/glm/gtx/transform.hpp"

#define NW 0
#define SW 1
#define NE 2
#define SE 3

using namespace glm;

class GameObject
{
private:
	vec2 corners[4];
	vec3 color;
protected:
	void setPos(vec2 pos);
	void setColor(vec3 col);
public:
	GameObject();
	GameObject(vec2 pos, vec3 rgb, float sizeX, float sizeY);
	void getCorners(vec2 toFill[]);
	vec3 getColor();
	void moveX(float newX, int index);
	void moveY(float newY, int index);
};

#endif