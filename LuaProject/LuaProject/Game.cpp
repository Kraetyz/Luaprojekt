#include "Game.h"
#include "Windows.h"

Game::Game()
{
	nrOfObjects = 2;
	allObjects = new GameObject*[nrOfObjects];
	allObjects[0] = new GameObject(glm::vec2(0, 0), glm::vec3(1, 0, 0));
	allObjects[1] = new GameObject(glm::vec2(0.5, 0), glm::vec3(0, 1, 0));

	nrOfButtons = 1;
	allButtons = new Button*[nrOfButtons];
	//nrOfButtons = 0; //THUGLÖSNING för att jag inte har några knappar än
	allButtons[0] = new Button(vec2(0.2f, 0.5f), "Hitlerknapp");
	allButtons[0]->loadBMP("HEIL.bmp");

	renderer = new Renderer();
	L = luaL_newstate();
	luaL_openlibs(L); /* opens the standard libraries */

	if (luaL_loadfile(L, "testscript.txt") || lua_pcall(L, 0, 0, 0))
		throw;
}

Game::~Game()
{
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

	lua_close(L);
}

void Game::Render()
{
	renderer->setProgram();
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
	allObjects[0]->getCorners(corners);
	vec2 tempCorners[4] = { vec2(corners[0]), vec2(corners[1]), vec2(corners[2]), vec2(corners[3]) };
	vec2 enemy[4];
	allObjects[1]->getCorners(enemy);

	if (GetKeyState('R') && GetAsyncKeyState('R'))
	{
		if (luaL_loadfile(L, "testscript.txt") || lua_pcall(L, 0, 0, 0))
			throw;
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
			allObjects[0]->moveX(newX, c);
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
			allObjects[0]->moveX(newX, c);
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
			allObjects[0]->moveY(newY, c);
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
			allObjects[0]->moveY(newY, c);
		}
	}

	allObjects[0]->getCorners(corners);
	if (collide(corners, enemy))
	{
		for (int c = 0; c < 4; c++)
		{
			allObjects[0]->moveX(tempCorners[c].x, c);
			allObjects[0]->moveY(tempCorners[c].y, c);
		}
	}

	return 0;
}

bool Game::collide(vec2 corners[], vec2 enemy[])
{
	//lua_getglobal(L, "intersects");
	float cNWx = corners[NW].x;
	float cSEx = corners[SE].x;
	float cNWy = corners[NW].y;
	float cSEy = corners[SE].y;
	float eNWx = enemy[NW].x;
	float eSEx = enemy[SE].x;
	float eNWy = enemy[NW].y;
	float eSEy = enemy[SE].y;
	if (cSEy <= eNWy)
		return false;
	if (cSEx <= eNWx)
		return false;
	if (cNWx >= eSEx)
		return false;
	if (cNWy >= eSEy)
		return false;
	return true;
/*	lua_pushnumber(L, cNWx);
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
	bool collide = lua_toboolean(L, -1);
	if (collide)
		throw;
	lua_pop(L, 1);
	*/
	//return collide;
}