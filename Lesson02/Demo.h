#ifndef DEMO_H
#define DEMO_H

#include "Game.h"
#include <SOIL/SOIL.h>

class Demo :
	public Engine::Game
{
public:
	Demo();
	~Demo();
	virtual void Init();
	virtual void Update(float deltaTime);
	virtual void Render();
private:
	GLuint VBO, VAO, EBO, texture, program;
	void BuildPlayerSprite();
	void DrawPlayerSprite();
};
#endif

