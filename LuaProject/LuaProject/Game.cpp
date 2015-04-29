#include "Game.h"
#include "Windows.h"

Game::Game()
{
	L = luaL_newstate();
	luaL_openlibs(L); /* opens the standard libraries */
	if (luaL_loadfile(L, "Error.lua") || lua_pcall(L, 0, 0, 0))
	{
		std::cout << "Error handler failed" << std::endl;
		std::cerr << lua_tostring(L, -1) << std::endl;
		lua_pop(L, 1);
	}
	lua_getglobal(L, "ErrorHandler");
	luaErrorHandlerPos = lua_gettop(L);

	map = luaL_newstate();
	luaL_openlibs(map); /* opens the standard libraries */

	if (luaL_loadfile(L, "map.txt") || lua_pcall(L, 0, 0, 0))
		throw;
	createPlayer();
	createGoal();
	lua_getglobal(L, "NUMBEROFOBJECTS");
	nrOfObjects = lua_tointeger(L, -1);
	lua_pop(L, 1);
	allObjects = new GameObject*[nrOfObjects];
	for (int c = 0; c < nrOfObjects; c++)
		createObject(c);

	lua_getglobal(L, "RADIUS");
	float rad = lua_tonumber(L, -1);
	renderer->setRadius(rad);
	lua_pop(L, 1);
	lua_getglobal(L, "BACKR");
	float r = lua_tonumber(L, -1);
	lua_getglobal(L, "BACKG");
	float g = lua_tonumber(L, -1);
	lua_getglobal(L, "BACKB");
	float b = lua_tonumber(L, -1);
	lua_pop(L, 3);
	renderer->setClearColor(r, g, b);
}

Game::~Game()
{
	delete player;
	delete renderer;
	for (int c = 0; c < nrOfObjects; c++)
	{
		delete allObjects[c];
	}
	delete[]allObjects;

	for (int c = 0; c < nrOfButtons; c++)
	{
		delete allButtons[c];
	}
	delete[]allButtons;

	lua_close(scripts);
	lua_close(map);
}

void Game::createPlayer()
{
	vec2 ret;
	lua_getglobal(L, "getObject");
	lua_pushinteger(L, -1);
	int error = lua_pcall(L, 1, 2, 0);
	if (error)
		throw;
	ret.y = lua_tonumber(L, -1);
	lua_pop(L, 1);
	ret.x = lua_tonumber(L, -1);
	lua_pop(L, 1);

	player = new GameObject(ret, glm::vec3(1, 0, 0), 0.8, 0.8);
}

void Game::createGoal()
{
	vec2 ret;
	lua_getglobal(L, "getObject");
	lua_pushinteger(L, -2);
	int error = lua_pcall(L, 1, 2, 0);
	if (error)
		throw;
	ret.y = lua_tonumber(L, -1);
	lua_pop(L, 1);
	ret.x = lua_tonumber(L, -1);
	lua_pop(L, 1);

	goal = new GameObject(ret, glm::vec3(1, 0, 0), 0.4, 0.4);
}

void Game::createObject(int index)
{
	vec2 scale;
	vec2 pos;
	vec3 col = vec3(0.5, 0.5, 0.5);
	lua_getglobal(L, "getObject");
	lua_pushinteger(L, index);
	int error = lua_pcall(L, 1, 5, 0);
	if (error)
		throw;
	scale.y = lua_tonumber(L, -1);
	lua_pop(L, 1);
	scale.x = lua_tonumber(L, -1);
	lua_pop(L, 1);
	std::string color = lua_tostring(L, -1);
	if (color == "red")
		col = vec3(1, 0, 0);
	if (color == "green")
		col = vec3(0, 1, 0);
	if (color == "blue")
		col = vec3(0, 0, 1);
	if (color == "white")
		col = vec3(1, 1, 1);
	lua_pop(L, 1);
	pos.y = lua_tonumber(L, -1);
	lua_pop(L, 1);
	pos.x = lua_tonumber(L, -1);
	lua_pop(L, 1);

	allObjects[index] = new GameObject(pos, col, scale.x, scale.y);
}

void Game::Render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	renderer->setProgram();
	renderer->Render(player, player);
	for (int c = 0; c < nrOfObjects; c++)
	{
		renderer->Render(allObjects[c]);
	}

	renderer->Render(goal);

	renderer->setBtnProgram();
	for (int c = 0; c < nrOfButtons; c++)
	{
		renderer->Render(allButtons[c]);
	}

	
}

string Game::update()
{
	int error = 0;
	vec2 corners[4];
	player->getCorners(corners);
	vec2 tempCorners[4] = { vec2(corners[0]), vec2(corners[1]), vec2(corners[2]), vec2(corners[3]) };

	if (GetKeyState('R') && GetAsyncKeyState('R'))
	{
		if (luaL_loadfile(L, "testscript.txt") || lua_pcall(L, 0, 0, 0))
			throw;
		if (luaL_loadfile(L, "map.txt") || lua_pcall(L, 0, 0, 0))
			throw;
		delete player;
		createPlayer();
		delete goal;
		createGoal();
		for (int c = 0; c < nrOfObjects; c++)
		{
			delete allObjects[c];
			createObject(c);
		}
		lua_getglobal(L, "RADIUS");
		renderer->setRadius(lua_tonumber(L, -1));
		lua_getglobal(L, "BACKR");
		float r = lua_tonumber(L, -1);
		lua_getglobal(L, "BACKG");
		float g = lua_tonumber(L, -1);
		lua_getglobal(L, "BACKB");
		float b = lua_tonumber(L, -1);
		lua_pop(L, 3);
		renderer->setClearColor(r, g, b);
	}
	
	float oldX, oldY;
	float newX, newY;
	if (GetKeyState('A') && GetAsyncKeyState('A'))
	{
		for (int c = 0; c < 4; c++)
		{
			lua_getglobal(L, "move");
			lua_pushnumber(L, corners[c].x);
			lua_pushstring(L, "left");
			error = lua_pcall(L, 2, 1, 0);
			if (error)
				throw;
			newX = lua_tonumber(L, -1);
			lua_pop(L, 1);
			player->moveX(newX, c);
		}
	}
	if (GetKeyState('D') && GetAsyncKeyState('D'))
	{
		for (int c = 0; c < 4; c++)
		{
			lua_getglobal(L, "move");
			lua_pushnumber(L, corners[c].x);
			lua_pushstring(L, "right");
			error = lua_pcall(L, 2, 1, 0);
			if (error)
				throw;
			newX = lua_tonumber(L, -1);
			lua_pop(L, 1);
			player->moveX(newX, c);
		}
	}

	player->getCorners(corners);
	if (collide(corners))
	{
		for (int c = 0; c < 4; c++)
		{
			player->moveX(tempCorners[c].x, c);
		}
	}

	if (GetKeyState('W') && GetAsyncKeyState('W'))
	{
		for (int c = 0; c < 4; c++)
		{
			lua_getglobal(L, "move");
			lua_pushnumber(L, corners[c].y);
			lua_pushstring(L, "up");
			error = lua_pcall(L, 2, 1, 0);
			if (error)
				throw;
			newY = lua_tonumber(L, -1);
			lua_pop(L, 1);
			player->moveY(newY, c);
		}
	}
	if (GetKeyState('S') && GetAsyncKeyState('S'))
	{
		for (int c = 0; c < 4; c++)
		{
			lua_getglobal(L, "move");
			lua_pushnumber(L, corners[c].y);
			lua_pushstring(L, "down");
			error = lua_pcall(L, 2, 1, 0);
			if (error)
				throw;
			newY = lua_tonumber(L, -1);
			lua_pop(L, 1);
			player->moveY(newY, c);
		}
	}

	player->getCorners(corners);
	if (goalCollide(corners))
	{
		for (int c = 0; c < 4; c++)
		{
			exit(0);
		}
	}

	this->goalUpdate();

	player->getCorners(corners);
	if (collide(corners))
	{
		for (int c = 0; c < 4; c++)
		{
			player->moveY(tempCorners[c].y, c);
		}
	}

	return "";
}

bool Game::collide(vec2 playerCorners[])
{
	bool hit = false;
	vec2 map[4];
	float cNWx = playerCorners[NW].x;
	float cSEx = playerCorners[SE].x;
	float cNWy = playerCorners[NW].y;
	float cSEy = playerCorners[SE].y;

	for (int c = 0; c < nrOfObjects && !hit; c++)
	{
		allObjects[c]->getCorners(map);
		lua_getglobal(L, "intersects");
		float eNWx = map[NW].x;
		float eSEx = map[SE].x;
		float eNWy = map[NW].y;
		float eSEy = map[SE].y;
		lua_pushnumber(L, cNWx);
		lua_pushnumber(L, cNWy);
		lua_pushnumber(L, cSEx);
		lua_pushnumber(L, cSEy);
		lua_pushnumber(L, eNWx);
		lua_pushnumber(L, eNWy);
		lua_pushnumber(L, eSEx);
		lua_pushnumber(L, eSEy);
		int error = lua_pcall(L, 8, 1, 0);
		if (error)
			throw;
		hit = lua_toboolean(L, -1);
		lua_pop(L, 1);
	}	
	return hit;
}

bool Game::goalCollide(vec2 playerCorners[])
{
	bool hit = false;
	vec2 map[4];
	goal->getCorners(map);
	lua_getglobal(L, "intersects");
	float cNWx = playerCorners[NW].x;
	float cSEx = playerCorners[SE].x;
	float cNWy = playerCorners[NW].y;
	float cSEy = playerCorners[SE].y;
	float eNWx = map[NW].x;
	float eSEx = map[SE].x;
	float eNWy = map[NW].y;
	float eSEy = map[SE].y;
	lua_pushnumber(L, cNWx);
	lua_pushnumber(L, cNWy);
	lua_pushnumber(L, cSEx);
	lua_pushnumber(L, cSEy);
	lua_pushnumber(L, eNWx);
	lua_pushnumber(L, eNWy);
	lua_pushnumber(L, eSEx);
	lua_pushnumber(L, eSEy);

	int error = lua_pcall(L, 8, 1, 0);
	if (error)
		throw;
	hit = lua_toboolean(L, -1);
	lua_pop(L, 1);

	return hit;
}

void Game::goalUpdate()
{
	this->goal->updateColor();
}