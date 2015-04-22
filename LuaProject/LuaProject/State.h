#ifndef STATE_H
#define STATE_H

#include "Lua/lua.hpp"
#include "Lua/lauxlib.h"
#include "Lua/lualib.h"
#include <string>
#include "Renderer.h"
#include "GameObject.h"

using namespace std;

class State
{
public:
	virtual string update() = 0;
	virtual void Render() = 0;
};

#endif