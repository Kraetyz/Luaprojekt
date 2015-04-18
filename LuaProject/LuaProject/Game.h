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
	GameObject** allObjects;
	int nrOfObjects;
	Button** allButtons;
	int nrOfButtons;
	Renderer* renderer;
	lua_State* L = 0;

	bool collide(vec2* corners, vec2* enemy);
public:
	Game();
	~Game();
	void Render();
	int update();
};

#endif