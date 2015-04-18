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
	vec2* corners = allObjects[0]->getCorners();
	vec2 tempCorners[4] = { vec2(corners[0]), vec2(corners[1]), vec2(corners[2]), vec2(corners[3]) };
	vec2* enemy = allObjects[1]->getCorners();

	if (GetKeyState('R') && GetAsyncKeyState('R'))
	{
		if (luaL_loadfile(L, "testscript.txt") || lua_pcall(L, 0, 0, 0))
			throw;
	}
	for (int c = 0; c < 4; c++)
	{
		if (GetKeyState('A') && GetAsyncKeyState('A'))
		{
			lua_getglobal(L, "move");
			lua_pushnumber(L, corners[c].x);
			lua_pushnumber(L, corners[c].y);
			lua_pushstring(L, "left");
			error = lua_pcall(L, 3, 1, 0);
			if (error)
				throw;
			corners[c].x = lua_tonumber(L, -1);
			lua_pop(L, 1);
		}
		if (GetKeyState('D') && GetAsyncKeyState('D'))
		{
			lua_getglobal(L, "move");
			lua_pushnumber(L, corners[c].x);
			lua_pushnumber(L, corners[c].y);
			lua_pushstring(L, "right");
			error = lua_pcall(L, 3, 1, 0);
			if (error)
				throw;
			corners[c].x = lua_tonumber(L, -1);
			lua_pop(L, 1);
		}
		if (GetKeyState('W') && GetAsyncKeyState('W'))
		{
			lua_getglobal(L, "move");
			lua_pushnumber(L, corners[c].x);
			lua_pushnumber(L, corners[c].y);
			lua_pushstring(L, "up");
			error = lua_pcall(L, 3, 1, 0);
			if (error)
				throw;
			corners[c].y = lua_tonumber(L, -1);
			lua_pop(L, 1);
		}
		if (GetKeyState('S') && GetAsyncKeyState('S'))
		{
			lua_getglobal(L, "move");
			lua_pushnumber(L, corners[c].x);
			lua_pushnumber(L, corners[c].y);
			lua_pushstring(L, "down");
			error = lua_pcall(L, 3, 1, 0);
			if (error)
				throw;
			corners[c].y = lua_tonumber(L, -1);
			lua_pop(L, 1);
		}
	}

	if (collide(corners, enemy))
	{
		for (int c = 0; c < 4; c++)
		{
			corners[c].x = tempCorners[c].x;
			corners[c].y = tempCorners[c].y;
		}
	}

	return 0;
}

bool Game::collide(vec2* corners, vec2* enemy)
{
	lua_getglobal(L, "intersects");
	lua_pushnumber(L, corners[0].x);
	lua_pushnumber(L, corners[0].y);
	lua_pushnumber(L, corners[3].x);
	lua_pushnumber(L, corners[3].y);
	lua_pushnumber(L, enemy[0].x);
	lua_pushnumber(L, enemy[0].y);
	lua_pushnumber(L, enemy[3].x);
	lua_pushnumber(L, enemy[3].y);
	int error = lua_pcall(L, 8, 1, 0);
	if (error)
		throw;
	bool collide = lua_toboolean(L, -1);
	lua_pop(L, 1);
	return collide;
}