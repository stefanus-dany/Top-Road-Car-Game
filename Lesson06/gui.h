#ifndef gui_H
#define gui_H

#include <SOIL/SOIL.h>
#include <SDL/SDL_mixer.h>
#include <SDL/SDL_thread.h>
#include <ft2build.h>
#include <freetype/freetype.h>
#include <map>
#include "Demo.h"

#include <GLM/glm.hpp>
#include <GLM/gtc/matrix_transform.hpp>
#include <GLM/gtc/type_ptr.hpp>

#include "Game.h"

using namespace glm;

#define FONTSIZE 40
#define FONTNAME "hongkonghustle.ttf"
#define FONTSIZE2 20
#define FONTNAME2 "hongkonghustle.ttf"
#define NUM_BUTTON 3

struct Character {
	GLuint TextureID; // ID handle of the glyph texture
	ivec2 Size; // Size of glyph
	ivec2 Bearing; // Offset from baseline to left/top of glyph
	GLuint Advance; // Offset to advance to next glyph
};

struct Character2 {
	GLuint TextureID2; // ID handle of the glyph texture
	ivec2 Size2; // Size of glyph
	ivec2 Bearing2; // Offset from baseline to left/top of glyph
	GLuint Advance2; // Offset to advance to next glyph
};

class gui :
	public Engine::Game
{
public:
	int scoreGui = -1;
	gui();
	~gui();
	virtual void Init();
	virtual void DeInit();
	virtual void Update(float deltaTime);
	virtual void Render();
private:
	void InitText();
	void InitText2();
	void RenderText(string text, GLfloat x, GLfloat y, GLfloat scale, vec3 color);
	void RenderText2(string text, GLfloat x, GLfloat y, GLfloat scale, vec3 color);
	void InitButton();
	void RenderButton();
	map<GLchar, Character> Characters;
	map<GLchar, Character2> Characters2;
	GLuint texture[NUM_BUTTON], hover_texture[NUM_BUTTON], VBOgui, VBO2gui, VAOgui, VAO2gui, programgui, VBOgui2, VBO2gui2, VAOgui2, VAO2gui2;
	float button_width[NUM_BUTTON], button_height[NUM_BUTTON], hover_button_width[NUM_BUTTON], hover_button_height[NUM_BUTTON];
	int activeButtonIndex = -2;
};
#endif