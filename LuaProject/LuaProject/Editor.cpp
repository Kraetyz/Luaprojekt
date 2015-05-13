#include "Editor.h"
#include "Windows.h"

Editor::Editor()
{
	L = luaL_newstate();
	luaL_openlibs(L); // opens the standard libraries
	if (luaL_loadfile(L, "Error.lua") || lua_pcall(L, 0, 0, 0))
	{
		std::cout << "Error handler failed" << std::endl;
		std::cerr << lua_tostring(L, -1) << std::endl;
		lua_pop(L, 1);
	}
	lua_getglobal(L, "ErrorHandler");
	luaErrorHandlerPos = lua_gettop(L);

	if (luaL_loadfile(L, "testscript.txt") || lua_pcall(L, 0, 0, luaErrorHandlerPos))
	{
		std::cout << "erroooor" << std::endl;
		std::cout << lua_tostring(L, -1) << std::endl;
		lua_pop(L, 1);
	}

	if (luaL_loadfile(L, "map.txt") || lua_pcall(L, 0, 0, luaErrorHandlerPos))
	{
		std::cerr << lua_tostring(L, -1) << std::endl;
		lua_pop(L, 1);
	}
	//createPlayer();
	//createGoal();
	lua_getglobal(L, "NUMBEROFOBJECTS");
	nrOfObjects = lua_tointeger(L, -1);
	lua_pop(L, 1);
	allObjects = new GameObject*[nrOfObjects];
	for (int c = 0; c < nrOfObjects; c++)
		createObject(c);

	render->setRadius(20.0f);
	lua_getglobal(L, "BACKR");
	float r = lua_tonumber(L, -1);
	lua_getglobal(L, "BACKG");
	float g = lua_tonumber(L, -1);
	lua_getglobal(L, "BACKB");
	float b = lua_tonumber(L, -1);
	lua_pop(L, 3);
	render->setClearColor(r, g, b);
	lua_pop(L, 1);
}

Editor::~Editor()
{
	delete player;
	for (int c = 0; c < nrOfObjects; c++)
	{
		delete allObjects[c];
	}
	delete[]allObjects;



	lua_close(L);
}

void Editor::createPlayer()
{
	lua_getglobal(L, "ErrorHandler");
	luaErrorHandlerPos = lua_gettop(L);
	vec2 ret;
	lua_getglobal(L, "getObject");
	lua_pushinteger(L, -1);
	int error = lua_pcall(L, 1, 2, luaErrorHandlerPos);
	if (error)
	{
		std::cout << " ERROR: " << std::endl;
		std::cout << lua_tostring(L, -1) << std::endl;
		lua_pop(L, 1);
	}
	else
	{
		ret.y = lua_tonumber(L, -1);
		lua_pop(L, 1);
		ret.x = lua_tonumber(L, -1);
		lua_pop(L, 1);
	}
	player = new GameObject(ret, glm::vec3(1, 0, 0), 0.8, 0.8);
	lua_pop(L, 1);
}

void Editor::createGoal()
{
	lua_getglobal(L, "ErrorHandler");
	luaErrorHandlerPos = lua_gettop(L);
	vec2 ret;
	lua_getglobal(L, "getObject");
	lua_pushinteger(L, -2);
	int error = lua_pcall(L, 1, 2, luaErrorHandlerPos);
	if (error)
	{
		std::cout << " ERROR: " << std::endl;
		std::cout << lua_tostring(L, -1) << std::endl;
		lua_pop(L, 1);
	}
	ret.y = lua_tonumber(L, -1);
	lua_pop(L, 1);
	ret.x = lua_tonumber(L, -1);
	lua_pop(L, 1);

	goal = new GameObject(ret, glm::vec3(1, 0, 0), 0.4, 0.4);
	lua_pop(L, 1);
}

void Editor::createObject(int index)
{
	lua_getglobal(L, "ErrorHandler");
	luaErrorHandlerPos = lua_gettop(L);
	vec2 scale;
	vec2 pos;
	vec3 col = vec3(0.5, 0.5, 0.5);
	lua_getglobal(L, "getObject");
	lua_pushinteger(L, index);
	int error = lua_pcall(L, 1, 5, luaErrorHandlerPos);
	if (error)
	{
		std::cout << " ERROR: " << std::endl;
		std::cout << lua_tostring(L, -1) << std::endl;
		lua_pop(L, 1);
	}
	else
	{
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
		if (color == "black")
			col = vec3(0, 0, 0);
		lua_pop(L, 1);
		pos.y = lua_tonumber(L, -1);
		lua_pop(L, 1);
		pos.x = lua_tonumber(L, -1);
		lua_pop(L, 1);
	}
	allObjects[index] = new GameObject(pos, col, scale.x, scale.y);
	lua_pop(L, 1);
}

void Editor::Render()
{
	render->setProgram();
	if (player)
		render->Render(player, player);
	for (int c = 0; c < nrOfObjects; c++)
	{
		render->Render(allObjects[c]);
	}

	if (goal)
		render->Render(goal);
}

string Editor::update()
{
	/*
	lua_getglobal(L, "ErrorHandler");
	luaErrorHandlerPos = lua_gettop(L);
	int error = 0;
	vec2 corners[4];
	player->getCorners(corners);
	vec2 tempCorners[4] = { vec2(corners[0]), vec2(corners[1]), vec2(corners[2]), vec2(corners[3]) };

	if (GetKeyState('R') && GetAsyncKeyState('R'))
	{
		if (luaL_loadfile(L, "testscript.txt") || lua_pcall(L, 0, 0, luaErrorHandlerPos))
		{
			std::cout << " ERROR: " << std::endl;
			std::cout << lua_tostring(L, -1) << std::endl;
			lua_pop(L, 1);
		}
		if (luaL_loadfile(L, "map.txt") || lua_pcall(L, 0, 0, luaErrorHandlerPos))
		{
			std::cout << " ERROR: " << std::endl;
			std::cout << lua_tostring(L, -1) << std::endl;
			lua_pop(L, 1);
		}
		delete player;
		createPlayer();
		for (int c = 0; c < nrOfObjects; c++)
		{
			delete allObjects[c];
			createObject(c);
		}
		lua_getglobal(L, "RADIUS");
		render->setRadius(lua_tonumber(L, -1));
		lua_getglobal(L, "BACKR");
		float r = lua_tonumber(L, -1);
		lua_getglobal(L, "BACKG");
		float g = lua_tonumber(L, -1);
		lua_getglobal(L, "BACKB");
		float b = lua_tonumber(L, -1);
		lua_pop(L, 3);
		render->setClearColor(r, g, b);
	}

	this->goalUpdate();

	*/
	return "";
}

void Editor::goalUpdate()
{
	this->goal->updateColor();
}

void Editor::giveCursorPos(glm::vec2 pos)
{
	
}