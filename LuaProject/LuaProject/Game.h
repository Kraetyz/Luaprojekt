#ifndef GAME_H
#define GAME_H

#include "Lua/lua.hpp"
#include "Lua/lauxlib.h"
#include "Lua/lualib.h"

#include<iostream>

#include "State.h"
#include "Renderer.h"
#include "GameObject.h"

class Game : public State
{
private:
	GameObject* player;
	GameObject** allObjects;
	int nrOfObjects;
	Button** allButtons;
	int nrOfButtons;
	Renderer* renderer;
	lua_State* L = 0;
	//lua_State* map = 0;
	int luaErrorHandlerPos = 0;

	bool collide(vec2 playerCorners[]);

	void createPlayer();
	void createObject(int index);
public:
	Game();
	~Game();
	void Render();
	string update();
};

#endif