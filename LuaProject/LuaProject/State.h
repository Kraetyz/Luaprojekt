#ifndef STATE_H
#define STATE_H

#include "Lua/lua.hpp"
#include "Lua/lauxlib.h"
#include "Lua/lualib.h"
#include <string>
#include "Renderer.h"
#include "GameObject.h"

#include "glm/glm/glm.hpp"
#include "glm/glm/gtx/transform.hpp"

using namespace std;

class State
{
protected:
	Renderer* render;
public:
	State();
	virtual ~State();
	virtual string update() = 0;
	virtual void Render() = 0;
	void RenderButton(Button* toRender);
};

#endif