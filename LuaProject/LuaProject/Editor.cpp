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

	render->setClearColor(0.1f, 0.2f, 0.3f);
	lua_pop(L, 1);

	nrOfObjects = 0;
	objectMax = 20;
	allObjects = new GameObject*[objectMax];

	currentColor = "green";
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

void Editor::createObject(glm::vec2 pos, string col)
{
	allObjects[nrOfObjects] = new GameObject(pos, col, 1, 1);
	nrOfObjects++;
	if (nrOfObjects == objectMax)
		expandObjectArray();
}

void Editor::expandObjectArray()
{
	GameObject** temp = new GameObject*[objectMax + 10];
	for (int c = 0; c < nrOfObjects; c++)
	{
		temp[c] = allObjects[c];
	}
	delete[]allObjects;
	allObjects = temp;
	objectMax += 10;
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
	if (clickTimer > FLT_EPSILON)
		clickTimer -= 0.016;
	goalUpdate();
	return "";
}

void Editor::goalUpdate()
{
	if (goal)
		goal->updateColor();
}

void Editor::giveCursorPos(glm::vec2 pos)
{
	if (mode == 1 && clickTimer < FLT_EPSILON)
	{
		createObject(pos, currentColor);
		clickTimer = 0.3f;
	}
}