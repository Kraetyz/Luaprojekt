#ifndef MENU_H
#define MENU_H

#include "State.h"

class Menu : public State
{
private:
	Renderer* renderer;
	Button** buttons;
	int nrOfButtons;
public:
	Menu();
	~Menu();
	string update();
	void Render();
};
#endif