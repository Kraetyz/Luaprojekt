#include "Game.h"
#include "Windows.h"

Game::Game()
{
	nrOfButtons = 1;
	allButtons = new Button*[nrOfButtons];
	//nrOfButtons = 0; //THUGLÖSNING för att jag inte har några knappar än
	allButtons[0] = new Button(vec2(0.2f, 0.5f), "Hitlerknapp");
	allButtons[0]->loadBMP("HEIL.bmp");

	renderer = new Renderer();
	scripts = luaL_newstate();
	luaL_openlibs(scripts); /* opens the standard libraries */

	if (luaL_loadfile(scripts, "testscript.txt") || lua_pcall(scripts, 0, 0, 0))
		throw;

	map = luaL_newstate();
	luaL_openlibs(map); /* opens the standard libraries */

	if (luaL_loadfile(map, "map.txt") || lua_pcall(map, 0, 0, 0))
		throw;

	player = new GameObject(glm::vec2(0, 0), glm::vec3(1, 0, 0));
	lua_getglobal(map, "NUMBEROFOBJECTS");
	nrOfObjects = lua_tointeger(map, -1);
	lua_pop(map, 1);
	allObjects = new GameObject*[nrOfObjects];
	for (int c=0; c<nrOfObjects; c++)
		allObjects[c] = new GameObject(getPositionOfObject(c), glm::vec3(0, 1, 0));
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

vec2 Game::getPositionOfObject(int index)
{
	vec2 ret;
	if (index == -1) //get player
	{
	}
	lua_getglobal(map, "getObject");
	lua_pushinteger(map, index);
	int error = lua_pcall(map, 1, 2, 0);
	if (error)
		throw;
	ret.y = lua_tonumber(map, -1);
	lua_pop(map, 1);
	ret.x = lua_tonumber(map, -1);
	lua_pop(map, 1);
	return ret;
}

void Game::Render()
{
	renderer->setProgram();
	renderer->Render(player, player);
	for (int c = 0; c < nrOfObjects; c++)
	{
		renderer->Render(allObjects[c]);
	}
	renderer->setBtnProgram();
	for (int c = 0; c < nrOfButtons; c++)
	{
		renderer->Render(allButtons[c]);
	}
}

int Game::update()
{
	int error = 0;
	vec2 corners[4];
	player->getCorners(corners);
	vec2 tempCorners[4] = { vec2(corners[0]), vec2(corners[1]), vec2(corners[2]), vec2(corners[3]) };
	vec2 enemy[4];
	allObjects[0]->getCorners(enemy);

	if (GetKeyState('R') && GetAsyncKeyState('R'))
	{
		if (luaL_loadfile(scripts, "testscript.txt") || lua_pcall(scripts, 0, 0, 0))
			throw;
		if (luaL_loadfile(map, "map.txt") || lua_pcall(map, 0, 0, 0))
			throw;
		for (int c = 0; c < nrOfObjects; c++)
		{
			delete allObjects[c];
			allObjects[c] = new GameObject(getPositionOfObject(c), glm::vec3(0, 1, 0));
		}
	}
	
	float oldX, oldY;
	float newX, newY;
	if (GetKeyState('A') && GetAsyncKeyState('A'))
	{
		for (int c = 0; c < 4; c++)
		{
			lua_getglobal(scripts, "move");
			lua_pushnumber(scripts, corners[c].x);
			lua_pushstring(scripts, "left");
			error = lua_pcall(scripts, 2, 1, 0);
			if (error)
				throw;
			newX = lua_tonumber(scripts, -1);
			lua_pop(scripts, 1);
			player->moveX(newX, c);
		}
	}
	if (GetKeyState('D') && GetAsyncKeyState('D'))
	{
		for (int c = 0; c < 4; c++)
		{
			lua_getglobal(scripts, "move");
			lua_pushnumber(scripts, corners[c].x);
			lua_pushstring(scripts, "right");
			error = lua_pcall(scripts, 2, 1, 0);
			if (error)
				throw;
			newX = lua_tonumber(scripts, -1);
			lua_pop(scripts, 1);
			player->moveX(newX, c);
		}
	}

	player->getCorners(corners);
	if (collide(corners, enemy))
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
			lua_getglobal(scripts, "move");
			lua_pushnumber(scripts, corners[c].y);
			lua_pushstring(scripts, "up");
			error = lua_pcall(scripts, 2, 1, 0);
			if (error)
				throw;
			newY = lua_tonumber(scripts, -1);
			lua_pop(scripts, 1);
			player->moveY(newY, c);
		}
	}
	if (GetKeyState('S') && GetAsyncKeyState('S'))
	{
		for (int c = 0; c < 4; c++)
		{
			lua_getglobal(scripts, "move");
			lua_pushnumber(scripts, corners[c].y);
			lua_pushstring(scripts, "down");
			error = lua_pcall(scripts, 2, 1, 0);
			if (error)
				throw;
			newY = lua_tonumber(scripts, -1);
			lua_pop(scripts, 1);
			player->moveY(newY, c);
		}
	}

	player->getCorners(corners);
	if (collide(corners, enemy))
	{
		for (int c = 0; c < 4; c++)
		{
			player->moveY(tempCorners[c].y, c);
		}
	}

	return 0;
}

bool Game::collide(vec2 corners[], vec2 enemy[])
{
	lua_getglobal(scripts, "intersects");
	float cNWx = corners[NW].x;
	float cSEx = corners[SE].x;
	float cNWy = corners[NW].y;
	float cSEy = corners[SE].y;
	float eNWx = enemy[NW].x;
	float eSEx = enemy[SE].x;
	float eNWy = enemy[NW].y;
	float eSEy = enemy[SE].y;

	lua_pushnumber(scripts, cNWx);
	lua_pushnumber(scripts, cNWy);
	lua_pushnumber(scripts, cSEx);
	lua_pushnumber(scripts, cSEy);
	lua_pushnumber(scripts, eNWx);
	lua_pushnumber(scripts, eNWy);
	lua_pushnumber(scripts, eSEx);
	lua_pushnumber(scripts, eSEy);
	int error = lua_pcall(scripts, 8, 1, 0);
	if (error)
		throw;
	bool hit = lua_toboolean(scripts, -1);
	lua_pop(scripts, 1);
	
	return hit;
}