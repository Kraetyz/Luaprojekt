#ifndef EDITOR_H
#define EDITOR_H

#include "Lua/lua.hpp"
#include "Lua/lauxlib.h"
#include "Lua/lualib.h"

#include<iostream>

#include "State.h"
#include "Renderer.h"
#include "GameObject.h"

class Editor : public State
{
private:
	int mode; //0 = nothing, 1 = wall, 2 = player, 3 = goal

	GameObject* goal;
	GameObject* player;
	GameObject** allObjects;
	int nrOfObjects;
	lua_State* L = 0;
	//lua_State* map = 0;
	int luaErrorHandlerPos = 0;

	void goalUpdate();
	void createGoal();
	void createPlayer();
	void createObject(int index);
public:
	Editor();
	~Editor();
	void Render();
	string update();

	void giveCursorPos(glm::vec2 pos);
	void setMode(int mode)
	{
		this->mode = mode;
	}

	void saveMap();
};
#endif