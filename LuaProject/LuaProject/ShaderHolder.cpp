#include "ShaderHolder.h"

ShaderHolder::ShaderHolder()
{
	const char* vertex_shader = R"(
		#version 400
		layout(location = 0) in vec2 vertex_position;
		
		void main ()
		{
			gl_Position = vec4(vertex_position, 1.0, 1.0);
		}
	)";

	const char* fragment_shader = R"(
		#version 400
		out vec4 fragment_color;

		uniform vec3 color;
		
		void main ()
		{
			fragment_color = vec4(color, 1.0);
		}
	)";
	//create vertex shader
	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vs, 1, &vertex_shader, nullptr);
	glCompileShader(vs);

	//create fragment shader
	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fs, 1, &fragment_shader, nullptr);
	glCompileShader(fs);

	//link shader program (connect vs and ps)
	program = glCreateProgram();
	glAttachShader(program, vs);

	glAttachShader(program, fs);
	glLinkProgram(program);
}

GLuint ShaderHolder::getProgram()
{
	return program;
}
