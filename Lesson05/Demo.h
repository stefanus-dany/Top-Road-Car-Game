#ifndef DEMO_H
#define DEMO_H


#include <SOIL/SOIL.h>

#include <GLM/glm.hpp>
#include <GLM/gtc/matrix_transform.hpp>
#include <GLM/gtc/type_ptr.hpp>

#include "Game.h"

#define NUM_FRAMES 8
#define FRAME_DUR 80

using namespace glm;

class Demo :
	public Engine::Game
{
public:
	Demo();
	~Demo();
	virtual void Init();
	virtual void Update(float deltaTime);
	virtual void Render();
	float frame_width = 0, frame_height = 0;
private:
	float frame_dur = 0, xpos = 0, ypos = 0, gravity = 0, xVelocity = 0, yVelocity = 0, xposGround = 0, yposGround = 0;
	GLuint VBO, VAO, EBO, texture, program;
	bool walk_anim = false, onGround = true;
	unsigned int frame_idx = 0, flip = 0;
	void BuildPlayerSprite();
	void DrawPlayerSprite();
	void UpdateSpriteAnim(float deltaTime);
	void ControlPlayerSprite(float deltaTime);

	/*void BuildRoad();
	void DrawRoad();
	void UpdateRoadAnim(float deltaTime);
	void ControlRoad(float deltaTime);*/
};
#endif

