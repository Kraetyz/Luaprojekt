#include "Game.h"

Game::Game()
{
	nrOfObjects = 2;
	allObjects = new GameObject*[nrOfObjects];
	allObjects[0] = new GameObject(glm::vec2(0, 0), glm::vec3(1, 0, 0));
	allObjects[1] = new GameObject(glm::vec2(0.5, 0), glm::vec3(0, 1, 0));

	nrOfButtons = 1;
	allButtons = new Button*[nrOfButtons];
	//nrOfButtons = 0; //THUGLÖSNING för att jag inte har några knappar än
	allButtons[0] = new Button(vec2(0.2f, 0.5f), "Hitlerknapp");
	allButtons[0]->loadBMP("HEIL.bmp");

	renderer = new Renderer();
}

Game::~Game()
{
	delete renderer;
	for (int c = 0; c < nrOfObjects; c++)
	{
		delete allObjects[c];
	}
	delete[]allObjects;

	for (int c = 0; c < nrOfButtons; c++)
	{
		delete allButtons[c];
	}
	delete[]allButtons;
}

void Game::Render()
{
	renderer->setProgram();
	for (int c = 0; c < nrOfObjects; c++)
	{
		renderer->Render(allObjects[c]);
	}
	renderer->setBtnProgram();
	for (int c = 0; c < nrOfButtons; c++)
	{
		renderer->Render(allButtons[c]);
	}
}