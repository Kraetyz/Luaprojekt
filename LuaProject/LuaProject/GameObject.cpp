#include "GameObject.h"

void GameObject::setPos(vec2 pos)
{
	corners[NW] = vec2(pos.x - 0.01, pos.y + 0.01);
	corners[NE] = vec2(pos.x + 0.01, pos.y + 0.01);
	corners[SW] = vec2(pos.x - 0.01, pos.y - 0.01);
	corners[SE] = vec2(pos.x + 0.01, pos.y - 0.01);
}

void GameObject::setColor(vec3 col)
{
	color = col;
}

GameObject::GameObject()
{
	color = vec3(1, 1, 1);
}

GameObject::GameObject(vec2 pos, vec3 rgb)
{
	float ratio = 1280.0f / 768.0f;
	corners[NW] = vec2(pos.x - 0.032, pos.y - 0.032*ratio);
	corners[NE] = vec2(pos.x + 0.032, pos.y - 0.032*ratio);
	corners[SW] = vec2(pos.x - 0.032, pos.y + 0.032*ratio);
	corners[SE] = vec2(pos.x + 0.032, pos.y + 0.032*ratio);
	color = rgb;
}

void GameObject::getCorners(vec2 toFill[])
{
	toFill[NW] = corners[NW];
	toFill[NE] = corners[NE];
	toFill[SW] = corners[SW];
	toFill[SE] = corners[SE];
}

vec3 GameObject::getColor()
{
	return color;
}

void GameObject::moveX(float newX, int index)
{
	corners[index].x = newX;
}

void GameObject::moveY(float newY, int index)
{
	corners[index].y = newY;
}

