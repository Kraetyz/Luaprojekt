//--------------------------------------------------------------------------------------
// BTH - Stefan Petersson 2014.
//--------------------------------------------------------------------------------------
#include <windows.h>

#include <gl/glew.h>
#include <gl/GL.h>
#include "glm/glm/glm.hpp"
#include "glm/glm/gtx/transform.hpp"

#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glew32.lib")

HWND InitWindow(HINSTANCE hInstance);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
HGLRC CreateOpenGLContext(HWND wndHandle);

GLuint gVertexBuffer = 0;
GLuint gVertexAttribute = 0;
GLuint gShaderProgram = 0;

GLuint tex;
glm::mat4 Model;
glm::mat4 View;
glm::mat4 Projection;


#define BUFFER_OFFSET(i) ((char *)nullptr + (i))

void CameraSetup()
{
	static float rot = 0.0f;
	Projection = glm::perspective(3.14159f*0.45f, 640.0f / 480.0f, 0.5f, 20.0f); //Projection matrix

	View = glm::lookAt(
		glm::vec3(0, 0, 2),
		glm::vec3(0, 0, 0),
		glm::vec3(0, 1, 0)
		); //View matrix
	Model = glm::mat4(cos(rot), 0.0f, sin(rot), 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		-sin(rot), 0.0f, cos(rot), 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f);  // Model matrix (World matrix?)
	rot += 0.0004f;
}

void CreateTexture()
{
	glGenTextures(1, &tex);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, BTH_IMAGE_WIDTH, BTH_IMAGE_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, BTH_IMAGE_DATA);
}

void CreateShaders()
{
	const char* vertex_shader = R"(
		#version 400
		layout(location = 0) in vec3 vertex_position;
		layout(location = 1) in vec2 vertex_UV;
		
		out vec2 UVcoord;
		
		void main () {
			UVcoord = vertex_UV;
			gl_Position = vec4(vertex_position, 1.0);
		}
	)";

	const char* geometry_shader = R"(
		#version 400
		layout(triangles) in;
		layout(triangle_strip, max_vertices = 8) out;
		in gl_PerVertex
		{
			vec4 gl_Position;
			float gl_PointSize;
			float gl_ClipDistance[];
		} gl_in[];

		uniform mat4 View;
		uniform mat4 Projection;
		uniform mat4 Model;

		in vec2 UVcoord[];
		out vec2 UV;
		out vec3 normalOut;
		out vec4 position;

		void main() {
			vec3 vector0 = vec3(gl_in[1].gl_Position-gl_in[0].gl_Position);
			vec3 vector1 = vec3(gl_in[2].gl_Position-gl_in[0].gl_Position);
			vec3 normal = normalize(cross(vector0, vector1));
			mat3 normalMatrix = transpose(inverse(mat3(Model)));
			normalOut = normalize(normalMatrix*normal);


			for(int i = 0; i < gl_in.length(); i++)
			{
				UV = UVcoord[i];
				gl_Position = Model*gl_in[i].gl_Position;
				gl_Position = View*gl_Position;
				gl_Position = Projection*gl_Position;
				position = Model*gl_in[i].gl_Position;
				position = View*position;
				EmitVertex();
			}
			EndPrimitive();

			for(int i = 0; i < gl_in.length(); i++)
			{
				UV = UVcoord[i];
				gl_Position = Model*(gl_in[i].gl_Position-vec4(0,0,1,0));
				gl_Position = View*gl_Position;
				gl_Position = Projection*gl_Position;
				position = Model*(gl_in[i].gl_Position-vec4(0,0,1,0));
				position = View*position;
				EmitVertex();
			}
			EndPrimitive();
		}
	)";

	const char* fragment_shader = R"(
		#version 400
		in vec2 UV;
		in vec3 normalOut;
		in vec4 position;

		uniform sampler2D texSampler;
		uniform mat3 Light;
		out vec4 fragment_color;

		vec3 addShit()
		{
			vec3 pos = vec3(position);
			vec3 n = normalize(normalOut);
			vec3 s = normalize(Light[0] - pos);
			vec3 v = normalize(-pos);
			vec3 r = reflect(-s, n);
			vec3 diffuse = Light[2] * max(dot(s,n), 0.0);
			vec3 specular = vec3(1.0, 1.0, 1.0);
			float SHINYPOWER = 2000.0f;
			vec3 ambient = vec3(0.2, 0.2, 0.2);
			vec3 specularLight = specular * pow(max(dot(r,v), 0.0), SHINYPOWER);
			return Light[1] * (ambient + diffuse + specularLight);
		}
		void main () {
			vec4 text = texture(texSampler, vec2(UV.s, 1-UV.t));
			fragment_color = text * vec4(addShit(),1.0);
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

	//Geoshade
	GLuint gs = glCreateShader(GL_GEOMETRY_SHADER);
	glShaderSource(gs, 1, &geometry_shader, nullptr);
	glCompileShader(gs);

	//link shader program (connect vs and ps)
	gShaderProgram = glCreateProgram();
	glAttachShader(gShaderProgram, vs);
	glAttachShader(gShaderProgram, gs);
	glAttachShader(gShaderProgram, fs);
	glLinkProgram(gShaderProgram);
}

void CreateTriangleData()
{
	float w = (480.0f / 640.0f) / 2.0f;
	struct TriangleVertex
	{
		float x, y, z;
		float u, v;
	}
	triangleVertices[4] =
	{
		-w, 0.5f, 1.0f,	//v0
		0.0f, 1.0f,

		-w, -0.5f, 1.0f, //v1
		0.0f, 0.0f,

		w, 0.5f, 1.0f,	//v2
		1.0f, 1.0f,


		w, -0.5f, 1.0f,	//v3
		1.0f, 0.0f

	};
	//create buffer and set data
	glGenBuffers(1, &gVertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, gVertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(triangleVertices), triangleVertices, GL_STATIC_DRAW);

	//define vertex data layout
	glGenVertexArrays(1, &gVertexAttribute);
	glBindVertexArray(gVertexAttribute);
	glEnableVertexAttribArray(0); //the vertex attribute object will remember its enabled attributes
	glEnableVertexAttribArray(1);
	GLuint vertexPos = glGetAttribLocation(gShaderProgram, "vertex_position");
	glVertexAttribPointer(vertexPos, 3, GL_FLOAT, GL_FALSE, sizeof(TriangleVertex), BUFFER_OFFSET(0));
	GLuint vertexUV = glGetAttribLocation(gShaderProgram, "vertex_UV");
	glVertexAttribPointer(vertexUV, 2, GL_FLOAT, GL_FALSE, sizeof(TriangleVertex), BUFFER_OFFSET(sizeof(float) * 3));
}

void SetViewport()
{
	glViewport(0, 0, 640, 480);
}

void Render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0, 0, 0, 1);
	glUseProgram(gShaderProgram);
	glActiveTexture(GL_TEXTURE0); //Which texture to use
	glBindTexture(GL_TEXTURE_2D, tex); //Bind dat shit
	glBindVertexArray(gVertexAttribute);

	CameraSetup();
	GLuint ViewID = glGetUniformLocation(gShaderProgram, "View");
	glUniformMatrix4fv(ViewID, 1, GL_FALSE, &View[0][0]);
	GLuint ProjID = glGetUniformLocation(gShaderProgram, "Projection");
	glUniformMatrix4fv(ProjID, 1, GL_FALSE, &Projection[0][0]);
	GLuint ModelID = glGetUniformLocation(gShaderProgram, "Model");
	glUniformMatrix4fv(ModelID, 1, GL_FALSE, &Model[0][0]);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	MSG msg = { 0 };
	HWND wndHandle = InitWindow(hInstance); //1. Skapa fönster

	if (wndHandle)
	{
		HDC hDC = GetDC(wndHandle);
		HGLRC hRC = CreateOpenGLContext(wndHandle); //2. Skapa och koppla OpenGL context

		glewInit(); //3. Initiera The OpenGL Extension Wrangler Library (GLEW)
		//glEnable(GL_CULL_FACE);
		glEnable(GL_DEPTH_TEST);
		glDepthMask(GL_TRUE);
		glDepthFunc(GL_LEQUAL);

		SetViewport(); //4. Sätt viewport

		CreateTexture();

		CreateShaders(); //5. Skapa vertex- och fragment-shaders

		CreateTriangleData(); //6. Definiera triangelvertiser, 7. Skapa vertex buffer object (VBO), 8.Skapa vertex array object (VAO)

		ShowWindow(wndHandle, nCmdShow);

		while (WM_QUIT != msg.message)
		{
			if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}

			else
			{
				Render(); //9. Rendera
				SwapBuffers(hDC); //10. Växla front- och back-buffer
			}
		}

		wglMakeCurrent(NULL, NULL);
		ReleaseDC(wndHandle, hDC);
		wglDeleteContext(hRC);
		DestroyWindow(wndHandle);
	}

	return (int)msg.wParam;
}

HWND InitWindow(HINSTANCE hInstance)
{
	WNDCLASSEX wcex = { 0 };
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.hInstance = hInstance;
	wcex.lpszClassName = L"BTH_GL_DEMO";
	if (!RegisterClassEx(&wcex))
		return false;

	RECT rc = { 0, 0, 640, 480 };
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

	HWND handle = CreateWindow(
		L"BTH_GL_DEMO",
		L"BTH OpenGL Demo",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		rc.right - rc.left,
		rc.bottom - rc.top,
		nullptr,
		nullptr,
		hInstance,
		nullptr);

	return handle;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}

HGLRC CreateOpenGLContext(HWND wndHandle)
{
	//get handle to a device context (DC) for the client area
	//of a specified window or for the entire screen
	HDC hDC = GetDC(wndHandle);

	//details: http://msdn.microsoft.com/en-us/library/windows/desktop/dd318286(v=vs.85).aspx
	static  PIXELFORMATDESCRIPTOR pixelFormatDesc =
	{
		sizeof(PIXELFORMATDESCRIPTOR),    // size of this pfd  
		1,                                // version number  
		PFD_DRAW_TO_WINDOW |              // support window  
		PFD_SUPPORT_OPENGL |              // support OpenGL  
		PFD_DOUBLEBUFFER |               // double buffered
		PFD_TYPE_RGBA,                    // RGBA type  
		32,                               // 32-bit color depth  
		0, 0, 0, 0, 0, 0,                 // color bits ignored  
		0,                                // no alpha buffer  
		0,                                // shift bit ignored  
		0,                                // no accumulation buffer  
		0, 0, 0, 0,                       // accum bits ignored  
		0,                                // 0-bits for depth buffer <-- modified by Stefan      
		0,                                // no stencil buffer  
		0,                                // no auxiliary buffer  
		PFD_MAIN_PLANE,                   // main layer  
		0,                                // reserved  
		0, 0, 0                           // layer masks ignored  
	};

	//attempt to match an appropriate pixel format supported by a
	//device context to a given pixel format specification.
	int pixelFormat = ChoosePixelFormat(hDC, &pixelFormatDesc);

	//set the pixel format of the specified device context
	//to the format specified by the iPixelFormat index.
	SetPixelFormat(hDC, pixelFormat, &pixelFormatDesc);

	//create a new OpenGL rendering context, which is suitable for drawing
	//on the device referenced by hdc. The rendering context has the same
	//pixel format as the device context.
	HGLRC hRC = wglCreateContext(hDC);

	//makes a specified OpenGL rendering context the calling thread's current
	//rendering context. All subsequent OpenGL calls made by the thread are
	//drawn on the device identified by hdc. 
	wglMakeCurrent(hDC, hRC);

	return hRC;
}