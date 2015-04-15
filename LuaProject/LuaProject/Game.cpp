#include "Game.h"

Game::Game()
{
	nrOfObjects = 2;
	allObjects = new GameObject*[nrOfObjects];
	allObjects[0] = new GameObject(glm::vec2(0, 0), glm::vec3(1, 0, 0));
	allObjects[1] = new GameObject(glm::vec2(0.5, 0), glm::vec3(1, 1, 0));

	nrOfButtons = 1;
	allButtons = new Button*[nrOfButtons];
	nrOfButtons = 0; //THUGL�SNING f�r att jag inte har n�gra knappar �n

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
	for (int c = 0; c < nrOfButtons; c++)
	{
		renderer->Render(allButtons[c]);
	}
}