#ifndef RENDERER_H
#define RENDERER_H

#include "ShaderHolder.h"
#include "Button.h"

class Renderer
{
private:
	ShaderHolder* shader;
	GLuint buf;
public:
	Renderer();
	~Renderer();
	void Render(Button* toRender);
	void Render(GameObject* toRender);
	void setProgram();
};

#endif