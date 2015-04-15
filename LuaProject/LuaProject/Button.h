#ifndef BUTTON_H
#define BUTTON_H

#include "GameObject.h"
#include <string>

using namespace std;

class Button : public GameObject
{
private:
	GLuint buttonTex;
	string name;
public:
	Button();
	Button(vec2 pos, string name);
	bool isClicked(vec2 mousePos);
	string getName();
	GLuint getText();
};

#endif