#ifndef GAME_H
#define GAME_H

#include "Lua/lua.hpp"
#include "Lua/lauxlib.h"
#include "Lua/lualib.h"

#include "Renderer.h"
#include "GameObject.h"

class Game
{
private:
	GameObject* player;
	GameObject** allObjects;
	int nrOfObjects;
	Button** allButtons;
	int nrOfButtons;
	Renderer* renderer;
	lua_State* scripts = 0;
	lua_State* map = 0;

	bool collide(vec2 corners[], vec2 enemy[]);

	vec2 getPositionOfObject(int index);
public:
	Game();
	~Game();
	void Render();
	int update();
};

#endif