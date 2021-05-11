#ifndef DEMO_H
#define DEMO_H

#include <SOIL/SOIL.h>

#include <SDL/SDL_mixer.h>
#include <SDL/SDL_thread.h>

#include <ft2build.h>
#include <freetype/freetype.h>
#include <map>

#include <GLM/glm.hpp>
#include <GLM/gtc/matrix_transform.hpp>
#include <GLM/gtc/type_ptr.hpp>

#include "Game.h"

#define NUM_FRAMES 8
#define FRAME_DUR 80

using namespace glm;

#define FONTSIZE 40
#define FONTNAME "hongkonghustle.ttf"
#define NUM_BUTTON 3

//struct Character {
//	GLuint TextureID; // ID handle of the glyph texture
//	ivec2 Size; // Size of glyph
//	ivec2 Bearing; // Offset from baseline to left/top of glyph
//	GLuint Advance; // Offset to advance to next glyph
//};

class Demo :
	public Engine::Game
{
public:
	Demo();
	~Demo();
	virtual void Init();
	virtual void DeInit();
	virtual void Update(float deltaTime);
	virtual void Render();
	float frame_width = 0, frame_height = 0, frame_width2 = 0, frame_height2 = 0, frame_width3 = 0, frame_height3 = 0, frame_width4 = 0, frame_height4 = 0
		, frame_width5 = 0, frame_height5 = 0, frame_width6 = 0, frame_height6 = 0;

	//Control 
private:
	//GUI
	/*void InitText();
	void RenderText(string text, GLfloat x, GLfloat y, GLfloat scale, vec3 color);
	void InitButton();
	void RenderButton();
	map<GLchar, Character> Characters;
	GLuint texturegui[NUM_BUTTON], hover_texturegui[NUM_BUTTON], VBOgui, VBO2gui, VAOgui, VAO2gui, programgui;
	float button_width[NUM_BUTTON], button_height[NUM_BUTTON], hover_button_width[NUM_BUTTON], hover_button_height[NUM_BUTTON];
	int activeButtonIndex = -2;*/

	float frame_dur = 0, frame_dur4 = 0, frame_dur5 = 0, oldxpos = 0, oldxpos4= 0, oldxpos5 = 0, xpos = 0, ypos = 0, xpos2 = 0, ypos2 = 0, xpos3 = 0, ypos3 = 0, xpos4 = 0, ypos4 = 0, gravity = 0, xVelocity = 0, yVelocity = 0, yposGround = 0, yVelocity4 = 0, gravity4 = 0, yposGround4 = 0
		, flip4 = 0, xVelocity4 = 0, yVelocity5 = 0, gravity5 = 0, yposGround5 = 0
		, xpos5 = 0, ypos5 = 0, flip5 = 0, xVelocity5 = 0, xpos6 = 0, ypos6 = 0, flip6 = 0, xVelocity6 = 0, gravity6 = 0, yPosGround6 = 0
		, frame_dur6 = 0, yVelocity6 = 0;
	
	//Draw
	GLuint VBO, VAO, EBO, texture, program, VBO2, VAO2, EBO2, texture2, program2, VBO3, VAO3, EBO3, texture3, program3, VBO4, VAO4, EBO4, texture4, program4
		, VBO5, VAO5, EBO5, texture5, program5, VBO6, VAO6, EBO6, texture6, program6;
	
	//Control
	bool walk_anim = false, walk_anim4 = false, walk_anim5 = false, walk_anim6 = false, onGround = true;
	unsigned int frame_idx = 0, frame_idx4 = 0, frame_idx5 = 0, frame_idx6 = 0, flip = 0;
	void BuildPlayerSprite();
	void BuildRightWallSprite();
	void BuildLeftWallSprite();
	void BuildDividerSprite();
	void BuildEnemySprite();
	void DrawRightWallSprite();
	void DrawLeftWallSprite();
	void DrawDividerSprite();
	void DrawEnemySprite();
	bool IsCollided(float x1, float y1, float width1, float height1, float x2, float y2, float width2, float height2, float x3, float y3, float width3, float height3
		, float x5, float y5, float width5, float height5);
	void DrawPlayerSprite();
	//
	void UpdatePlayerSpriteAnim(float deltaTime);
	void ControlPlayerSprite(float deltaTime);
	void UpdateDividerAnim(float deltaTime);
	void ControlDivider(float deltaTime);
	void UpdateEnemyAnim(float deltaTime);
	void ControlEnemy(float deltaTime);
};
#endif

