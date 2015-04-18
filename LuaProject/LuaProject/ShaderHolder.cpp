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

	vertex_shader = R"(
		#version 400
		layout(location = 0) in vec2 vertex_position;
		out vec2 UV;

		void main ()
		{
			if (gl_VertexID == 0)
			{
				UV = vec2(0,1);
			}
			if (gl_VertexID == 1)
			{
				UV = vec2(0,0);
			}
			if (gl_VertexID == 2)
			{
				UV = vec2(1,1);
			}
			if (gl_VertexID == 3)
			{
				UV = vec2(1,0);
			}
			gl_Position = vec4(vertex_position, 1.0, 1.0);
		}
	)";

	fragment_shader = R"(
		#version 400
		out vec4 fragment_color;
		uniform sampler2D texSampler;
		in vec2 UV;
		
		void main ()
		{
			fragment_color = texture(texSampler, vec2(UV.s, 1-UV.t));
		}
	)";

	//create vertex shader
	vs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vs, 1, &vertex_shader, nullptr);
	glCompileShader(vs);

	//create fragment shader
	fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fs, 1, &fragment_shader, nullptr);
	glCompileShader(fs);

	//link shader program (connect vs and ps)
	buttonprog = glCreateProgram();
	glAttachShader(buttonprog, vs);
	glAttachShader(buttonprog, fs);
	glLinkProgram(buttonprog);
}

GLuint ShaderHolder::getProgram()
{
	return program;
}

GLuint ShaderHolder::getButtonProgram()
{
	return buttonprog;
}