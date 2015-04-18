#include "Renderer.h"

Renderer::Renderer()
{
	shader = new ShaderHolder();
}

Renderer::~Renderer()
{
	delete shader;
}

void Renderer::Render(Button* toRender)
{
	vec2* corners = toRender->getCorners();

	GLuint buf;
	glGenBuffers(1, &buf);
	glBindBuffer(GL_ARRAY_BUFFER, buf);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 8, corners, GL_STATIC_DRAW);

	//define vertex data layout
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glEnableVertexAttribArray(0); //the vertex attribute object will remember its enabled attributes
	GLuint prog = shader->getButtonProgram();
	GLuint vertexPos = glGetAttribLocation(prog, "vertex_position");
	glVertexAttribPointer(vertexPos, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, (void*)0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, toRender->getText());

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	glDeleteBuffers(1, &buf);
	glDeleteVertexArrays(1, &vao);
}

void Renderer::Render(GameObject* toRender)
{
	vec2* corners = toRender->getCorners();
	vec3 color = toRender->getColor();

	GLuint buf;
	glGenBuffers(1, &buf);
	glBindBuffer(GL_ARRAY_BUFFER, buf);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*8, corners, GL_STATIC_DRAW);

	//define vertex data layout
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glEnableVertexAttribArray(0); //the vertex attribute object will remember its enabled attributes
	GLuint prog = shader->getProgram();
	GLuint vertexPos = glGetAttribLocation(prog, "vertex_position");
	glVertexAttribPointer(vertexPos, 2, GL_FLOAT, GL_FALSE, sizeof(float)*2, (void*)0);

	GLuint colorID = glGetUniformLocation(prog, "color");
	glUniform3fv(colorID, 1, &color[0]);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	glDeleteBuffers(1, &buf);
	glDeleteVertexArrays(1, &vao);
}

void Renderer::setProgram()
{
	glUseProgram(shader->getProgram());
}