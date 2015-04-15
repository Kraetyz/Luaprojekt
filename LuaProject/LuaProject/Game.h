#ifndef GAME_H
#define GAME_H

#include "Renderer.h"
#include "GameObject.h"

class Game
{
private:
	GameObject** allObjects;
	int nrOfObjects;
	Button** allButtons;
	int nrOfButtons;
	Renderer* renderer;
public:
	Game();
	~Game();
	void Render();
};

#endif