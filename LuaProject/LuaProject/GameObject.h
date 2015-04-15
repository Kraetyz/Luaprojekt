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
	GameObject(vec2 pos, vec3 rgb);
	vec2* getCorners();
	vec3 getColor();
};

#endif