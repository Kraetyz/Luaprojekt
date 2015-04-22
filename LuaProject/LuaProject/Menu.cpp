#include "Menu.h"
#include "Windows.h"

Menu::Menu()
{
	nrOfButtons = 1;
	buttons = new Button*[nrOfButtons];
	nrOfButtons = 0;
	renderer = new Renderer();
}

Menu::~Menu()
{
	delete renderer;
	for (int c = 0; c < nrOfButtons; c++)
	{
		delete buttons[c];
	}
	delete[]buttons;
}

string Menu::update()
{
	if (GetKeyState('G') && GetAsyncKeyState('G'))
		return "StartGame";
	return "";
}

void Menu::Render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	renderer->setBtnProgram();
	for (int c = 0; c < nrOfButtons; c++)
	{
		renderer->Render(buttons[c]);
	}
}