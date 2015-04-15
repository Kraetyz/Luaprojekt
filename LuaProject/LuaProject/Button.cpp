#include "Button.h"

Button::Button() : GameObject()
{
}

Button::Button(vec2 pos, string name) : GameObject(pos, vec3(0,0,0))
{
	this->name = name;
}

bool Button::isClicked(vec2 mousePos)
{
	//Collision logic
	return false;
}

string Button::getName()
{
	return name;
}

GLuint Button::getText()
{
	return buttonTex;
}