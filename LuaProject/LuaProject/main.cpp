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

#include "Game.h"
#include "Menu.h"

#include "Lua/lua.hpp"
#include "Lua/lauxlib.h"
#include "Lua/lualib.h"

lua_State* buttonState;
Button buttons[10];
int nrOfButtons = 0;

State* game;

HWND InitWindow(HINSTANCE hInstance);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
HGLRC CreateOpenGLContext(HWND wndHandle);

#define BUFFER_OFFSET(i) ((char *)nullptr + (i))

void SetViewport()
{
	glViewport(0, 0, 1280, 768);
}

static int killThroughLua(lua_State* L)
{
	if (true)
		throw;
	return 0;
}

void clickUpdate(HWND* window)
{
	POINT pCur;
	GetCursorPos(&pCur);
	ScreenToClient(*window, &pCur);
	lua_getglobal(buttonState, "clickCheck");
	float mouseX = (pCur.x - (1280 / 2)) / 32.0f;
	float mouseY = -(pCur.y - (768 / 2)) / ((1280.0f / 768.0f)*32.0f);
	lua_pushnumber(buttonState, mouseX);
	lua_pushnumber(buttonState, mouseY);
	if (lua_pcall(buttonState, 2, 1, 0))
		throw;
	bool weHaveClickBoys = lua_toboolean(buttonState, -1);
	lua_pop(buttonState, 1);
}

void setupButtons()
{
	glm::vec2 northwest, southeast, pos, size;
	lua_getglobal(buttonState, "nrOfButtons");
	nrOfButtons = lua_tointeger(buttonState, -1);
	lua_pop(buttonState, 1);
	for (int c = 1; c < nrOfButtons+1; c++)
	{
		lua_getglobal(buttonState, "getButton");
		lua_pushinteger(buttonState, c);
		lua_pcall(buttonState, 1, 5, 0);
		string buttonType = lua_tostring(buttonState, -1);
		southeast.y = lua_tonumber(buttonState, -2);
		southeast.x = lua_tonumber(buttonState, -3);
		northwest.y = lua_tonumber(buttonState, -4);
		northwest.x = lua_tonumber(buttonState, -5);
		lua_pop(buttonState, 5);
		pos = glm::vec2((northwest.x + southeast.x) / 2.0f, (northwest.y + southeast.y) / 2.0f);
		size = glm::vec2(southeast.x - northwest.x, northwest.y - southeast.y);
		buttons[c-1] = Button(pos, size, buttonType);
		buttons[c-1].loadBMP("Hej.bmp");
	}
}

void buttonRender()
{
	for (int c = 0; c < nrOfButtons; c++)
	{
		game->RenderButton(&buttons[c]);
	}
}

void Update()
{
	string msg = game->update();
	if (msg == "StartGame")
	{
		delete game;
		game = new Game();
	}
	else
	{
		game->Render();
		buttonRender();
	}
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	MSG msg = { 0 };
	HWND wndHandle = InitWindow(hInstance); //1. Skapa f�nster

	if (wndHandle)
	{
		HDC hDC = GetDC(wndHandle);
		HGLRC hRC = CreateOpenGLContext(wndHandle); //2. Skapa och koppla OpenGL context

		glewInit(); //3. Initiera The OpenGL Extension Wrangler Library (GLEW)
		//glEnable(GL_CULL_FACE);

		SetViewport(); //4. S�tt viewport

		ShowWindow(wndHandle, nCmdShow);

		game = new Menu();

		buttonState = luaL_newstate();
		luaL_openlibs(buttonState);
		lua_register(buttonState, "execute", killThroughLua);
		if (luaL_loadfile(buttonState, "menuButtons.txt") || lua_pcall(buttonState, 0, 0, 0))
			throw;
		setupButtons();

		while (WM_QUIT != msg.message)
		{
			if (msg.message == WM_LBUTTONDOWN)
				clickUpdate(&wndHandle);
			if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}

			else
			{
				Update(); //9. Rendera
				SwapBuffers(hDC); //10. V�xla front- och back-buffer
			}
		}

		wglMakeCurrent(NULL, NULL);
		ReleaseDC(wndHandle, hDC);
		wglDeleteContext(hRC);
		DestroyWindow(wndHandle);
	}

	lua_close(buttonState);

	return (int)msg.wParam;
}

HWND InitWindow(HINSTANCE hInstance)
{
	WNDCLASSEX wcex = { 0 };
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.hInstance = hInstance;
	wcex.lpszClassName = L"LUAbyrinth";
	if (!RegisterClassEx(&wcex))
		return false;

	RECT rc = { 0, 0, 1280, 768 };
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

	HWND handle = CreateWindow(
		L"LUAbyrinth",
		L"GAAAAME",
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