#include "Demo.h"


Demo::Demo()
{
}


Demo::~Demo()
{
}

void Demo::Init()
{
}

void Demo::Update(float deltaTime)
{
}

void Demo::Render()
{
	//Clear the color and depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Set the background color
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
}

int main(int argc, char** argv) {

	Engine::Game &game = Demo();
	game.Start("Game Loop", 800, 600, false, WindowFlag::WINDOWED, 0, 1);

	return 0;
}