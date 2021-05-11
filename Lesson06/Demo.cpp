#include "Demo.h"
#include "gui.h"
int ovehicleX[4], ovehicleY[4];
int i;
Uint8* keystate;
bool collide;
int score;


Demo::Demo()
{
}

Demo::~Demo()
{
}

void Demo::Init()
{
	score = -1;
	InitTextScore();
	BuildDividerSprite();
	BuildPlayerSprite();
	BuildRightWallSprite();
	BuildLeftWallSprite();
	BuildEnemySprite();
}

void Demo::Update(float deltaTime)
{

		if (IsKeyDown("Quit")) {
			SDL_Quit();
			exit(0);
		}

		UpdateDividerAnim(deltaTime);
		UpdateEnemyAnim(deltaTime);
		UpdatePlayerSpriteAnim(deltaTime);
		ControlPlayerSprite(deltaTime);
		ControlDivider(deltaTime);
		ControlEnemy(deltaTime);
}

void Demo::DeInit() {

}


void Demo::Render()
{
	
		//Setting Viewport
		glViewport(0, 0, GetScreenWidth(), GetScreenHeight());

		//Clear the color and depth buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//Set the background color
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		
		string scoreText = to_string(score);

		RenderTextScore("Score : "+scoreText, 10, 10, 1.0f, vec3(244.0f / 255.0f, 12.0f / 255.0f, 116.0f / 255.0f));
		DrawDividerSprite();
		DrawPlayerSprite();
		DrawRightWallSprite();
		DrawLeftWallSprite();
		DrawEnemySprite();

	
}

void Demo::InitTextScore() {
	/*Init Freetype*/
	FT_Library ft;
	if (FT_Init_FreeType(&ft)) {
		Err("ERROR::FREETYPE: Could not init FreeType Library");
	}
	FT_Face face;
	if (FT_New_Face(ft, FONTNAME, 0, &face)) {
		Err("ERROR::FREETYPE: Failed to load font");
	}

	FT_Set_Pixel_Sizes(face, 0, FONTSIZE);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // Disable byte-alignment restriction
	for (GLubyte c = 0; c < 128; c++)
	{
		/*Load character glyph*/
		if (FT_Load_Char(face, c, FT_LOAD_RENDER))
		{
			std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
			continue;
		}
		/*Generate texture*/
		GLuint texturegui;
		glGenTextures(1, &texturegui);
		glBindTexture(GL_TEXTURE_2D, texturegui);
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RED,
			face->glyph->bitmap.width,
			face->glyph->bitmap.rows,
			0,
			GL_RED,
			GL_UNSIGNED_BYTE,
			face->glyph->bitmap.buffer
		);
		/*Set texture options*/
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		/*Now store character for later use*/
		CharacterScore characterScore = {
			texturegui,
			ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
			ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
			face->glyph->advance.x
		};
		CharactersScore.insert(pair<GLchar, CharacterScore>(c, characterScore));
	}

	FT_Done_Face(face);
	FT_Done_FreeType(ft);

	glGenVertexArrays(1, &VAOscore);
	glGenBuffers(1, &VBOscore);
	glBindVertexArray(VAOscore);
	glBindBuffer(GL_ARRAY_BUFFER, VBOscore);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 4 * 4, NULL,
		GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Demo::RenderTextScore(string text, GLfloat x, GLfloat y, GLfloat scale, vec3 color)
{
	// Activate corresponding render state
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	this->programscore = BuildShader("shaderGui.vert", "shaderGui.frag");
	// Pass n to shader
	UseShader(this->programscore);
	glUniform1f(glGetUniformLocation(this->programscore, "n"), 1.0f / NUM_FRAMES);

	glUniform3f(glGetUniformLocation(this->programscore, "ourColor"), color.x, color.y, color.z);
	glUniform1i(glGetUniformLocation(this->programscore, "text"), 1);
	glActiveTexture(GL_TEXTURE0);
	glUniform1i(glGetUniformLocation(this->programscore, "ourTexture"), 0);
	mat4 model;
	glUniformMatrix4fv(glGetUniformLocation(this->programscore, "model"), 1, GL_FALSE, value_ptr(model));
	glBindVertexArray(VAOscore);

	// Set orthographic projection
	mat4 projection;
	projection = ortho(0.0f, static_cast<GLfloat>(GetScreenWidth()), static_cast<GLfloat>(GetScreenHeight()), 0.0f, -1.0f, 1.0f);
	glUniformMatrix4fv(glGetUniformLocation(this->programscore, "projection"), 1, GL_FALSE, value_ptr(projection));


	// Iterate through all characters
	string::const_iterator c;
	for (c = text.begin(); c != text.end(); c++)
	{
		CharacterScore chScore = CharactersScore[*c];
		/*GLfloat xpos = x + ch.Bearing.x * scale;*/
		GLfloat xpos = (x + chScore.BearingScore.x * scale) + (GetScreenWidth() / 2.7);
		GLfloat ypos = (y + (this->CharactersScore['H'].BearingScore.y - chScore.BearingScore.y) * scale);
		GLfloat w = chScore.SizeScore.x * scale;
		GLfloat h = chScore.SizeScore.y * scale;
		// Update VBO for each character

		GLfloat vertices[] = {
			// Positions   // Texture Coords
			xpos + w,	ypos + h,	1.0f, 1.0f, // Bottom Right
			xpos + w,	ypos,		1.0f, 0.0f, // Top Right
			xpos,		ypos,		0.0f, 0.0f, // Top Left
			xpos,		ypos + h,	0.0f, 1.0f  // Bottom Left 
		};
		// Render glyph texture over quad
		glBindTexture(GL_TEXTURE_2D, chScore.TextureIDScore);
		// Update content of VBO memory
		glBindBuffer(GL_ARRAY_BUFFER, VBOscore);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		// Render quad
		glDrawArrays(GL_QUADS, 0, 4);
		// Now advance cursors for next glyph (note that advance is number of 1 / 64 pixels)
		x += (chScore.AdvanceScore >> 6) * scale; // Bitshift by 6 to get value in pixels(2 ^ 6 = 64)
	}
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_BLEND);
}

void Demo::UpdatePlayerSpriteAnim(float deltaTime)
{
	// Update animation
	frame_dur += deltaTime;

	if (walk_anim && frame_dur > FRAME_DUR) {
		frame_dur = 0;
		if (frame_idx == NUM_FRAMES - 1) frame_idx = 0;  else frame_idx++;

		// Pass frameIndex to shader
		UseShader(this->program);
		glUniform1i(glGetUniformLocation(this->program, "frameIndex"), frame_idx);
	}
}

void Demo::ControlPlayerSprite(float deltaTime)
{
	walk_anim = false;
	oldxpos = xpos;

	if (IsKeyDown("Move Right")) {
		xpos += deltaTime * xVelocity;
		flip = 0;
		walk_anim = true;
	}

	if (IsKeyDown("Move Left")) {
		xpos -= deltaTime * xVelocity;
		flip = 1;
		walk_anim = true;
	}
	// check collision
	collide = false;
	if (IsCollided(xpos, ypos, frame_width, frame_height, xpos2, ypos2, frame_width2, frame_height2, xpos3, ypos3, frame_width3, frame_height3
		, xpos5, ypos5, frame_width5, frame_height5)) {
		xpos = oldxpos;
		collide = true;
	}

	if (collide == true) {
		Engine::Game& game = Demo();
		game.End();
		Engine::Game& gameGo = gui();
		gameGo.Start("User Interface gui", 800, 600, false, WindowFlag::WINDOWED, 60, 1);
	}
}

void Demo::DrawPlayerSprite() {
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Bind Textures using texture units
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	UseShader(this->program);
	glUniform1i(glGetUniformLocation(this->program, "ourTexture"), 0);

	// set flip
	glUniform1i(glGetUniformLocation(this->program, "flip"), flip);
	mat4 model;
	// Translate sprite along x-axis
	model = translate(model, vec3(xpos, ypos, 0.0f));
	// Scale sprite 
	model = scale(model, vec3(frame_width, frame_height, 1));
	glUniformMatrix4fv(glGetUniformLocation(this->program, "model"), 1, GL_FALSE, value_ptr(model));

	// Draw sprite
	glBindVertexArray(VAO);
	glDrawElements(GL_QUADS, 4, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture when done, so we won't accidentily mess up our texture.
	glDisable(GL_BLEND);
}

void Demo::BuildPlayerSprite()
{
	this->program = BuildShader("playerSprite.vert", "playerSprite.frag");
	// Pass n to shader
	UseShader(this->program);
	glUniform1f(glGetUniformLocation(this->program, "n"), 1.0f / NUM_FRAMES);

	// Load and create a texture 
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture); // All upcoming GL_TEXTURE_2D operations now have effect on our texture object

	// Set texture filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// Load, create texture 
	int width, height;
	unsigned char* image = SOIL_load_image("homeranim.png", &width, &height, 0, SOIL_LOAD_RGBA);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture when done, so we won't accidentily mess up our texture.

	// Set up vertex data (and buffer(s)) and attribute pointers
	frame_width = ((float)width) / NUM_FRAMES;
	frame_height = (float)height;
	GLfloat vertices[] = {
		// Positions   // Colors           // Texture Coords
		1,  1, 0.0f,   1.0f, 1.0f, 1.0f,   1.0f, 1.0f, // Bottom Right
		1,  0, 0.0f,   1.0f, 1.0f, 0.0f,   1.0f, 0.0f, // Top Right
		0,  0, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 0.0f, // Top Left
		0,  1, 0.0f,   1.0f, 1.0f, 1.0f,   0.0f, 1.0f  // Bottom Left 
	};

	GLuint indices[] = {  // Note that we start from 0!
		0, 3, 2, 1
	};

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), 0);
	glEnableVertexAttribArray(0);
	// Color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	// TexCoord attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	glBindVertexArray(0); // Unbind VAO

	// Set orthographic projection
	mat4 projection;
	projection = ortho(0.0f, static_cast<GLfloat>(GetScreenWidth()), static_cast<GLfloat>(GetScreenHeight()), 0.0f, -1.0f, 1.0f);
	glUniformMatrix4fv(glGetUniformLocation(this->program, "projection"), 1, GL_FALSE, value_ptr(projection));

	// set sprite position, gravity, velocity
	xpos = (GetScreenWidth() - frame_width) / 2;
	yposGround = GetScreenHeight() - frame_height;
	ypos = yposGround;
	gravity = 0.05f;
	xVelocity = 0.5f;

	// Add input mapping
	// to offer input change flexibility you can save the input mapping configuration in a configuration file (non-hard code) !
	InputMapping("Move Right", SDLK_RIGHT);
	InputMapping("Move Left", SDLK_LEFT);
	InputMapping("Move Right", SDLK_d);
	InputMapping("Move Left", SDLK_a);
	InputMapping("Move Right", SDL_BUTTON_RIGHT);
	InputMapping("Move Left", SDL_BUTTON_LEFT);
	InputMapping("Move Right", SDL_CONTROLLER_BUTTON_DPAD_RIGHT);
	InputMapping("Move Left", SDL_CONTROLLER_BUTTON_DPAD_LEFT);
	InputMapping("Quit", SDLK_ESCAPE);
}

void Demo::BuildRightWallSprite()
{
	this->program2 = BuildShader("crateSprite.vert", "crateSprite.frag");
	UseShader(this->program2);

	// Load and create a texture 
	glGenTextures(1, &texture2);
	glBindTexture(GL_TEXTURE_2D, texture2); // All upcoming GL_TEXTURE_2D operations now have effect on our texture object

	// Set texture filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// Load, create texture 
	int width, height;
	unsigned char* image = SOIL_load_image("road.png", &width, &height, 0, SOIL_LOAD_RGBA);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture when done, so we won't accidentily mess up our texture.

	// Set up vertex data (and buffer(s)) and attribute pointers
	frame_width2 = (float)width;
	frame_height2 = (float)height;
	GLfloat vertices[] = {
		// Positions   // Colors           // Texture Coords
		1,  1, 0.0f,   1.0f, 1.0f, 1.0f,   1.0f, 1.0f, // Bottom Right
		1,  0, 0.0f,   1.0f, 1.0f, 0.0f,   1.0f, 0.0f, // Top Right
		0,  0, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 0.0f, // Top Left
		0,  1, 0.0f,   1.0f, 1.0f, 1.0f,   0.0f, 1.0f  // Bottom Left 
	};

	GLuint indices[] = {  // Note that we start from 0!
		0, 3, 2, 1
	};

	glGenVertexArrays(1, &VAO2);
	glGenBuffers(1, &VBO2);
	glGenBuffers(1, &EBO2);

	glBindVertexArray(VAO2);

	glBindBuffer(GL_ARRAY_BUFFER, VBO2);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO2);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), 0);
	glEnableVertexAttribArray(0);
	// Color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	// TexCoord attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	glBindVertexArray(0); // Unbind VAO

	// Set orthographic projection
	mat4 projection;
	projection = ortho(0.0f, static_cast<GLfloat>(GetScreenWidth()), static_cast<GLfloat>(GetScreenHeight()), 0.0f, -1.0f, 1.0f);
	glUniformMatrix4fv(glGetUniformLocation(this->program2, "projection"), 1, GL_FALSE, value_ptr(projection));

	// set sprite position, gravity, velocity
	xpos2 = (GetScreenWidth() - frame_width2) / 1.25;
	ypos2 = GetScreenHeight() - frame_height2;
}

void Demo::DrawRightWallSprite() {
	// Bind Textures using texture units
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texture2);
	// Activate shader
	UseShader(this->program2);
	glUniform1i(glGetUniformLocation(this->program2, "ourTexture"), 1);

	mat4 model;
	// Translate sprite along x-axis
	model = translate(model, vec3(xpos2, ypos2, 0.1f));
	// Scale sprite 
	model = scale(model, vec3(frame_width2, frame_height2, 1));
	glUniformMatrix4fv(glGetUniformLocation(this->program2, "model"), 1, GL_FALSE, value_ptr(model));

	// Draw sprite
	glBindVertexArray(VAO2);
	glDrawElements(GL_QUADS, 4, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture when done, so we won't accidentily mess up our texture.
}

void Demo::BuildLeftWallSprite()
{
	this->program3 = BuildShader("crateSprite.vert", "crateSprite.frag");
	UseShader(this->program3);

	// Load and create a texture 
	glGenTextures(1, &texture3);
	glBindTexture(GL_TEXTURE_2D, texture3); // All upcoming GL_TEXTURE_2D operations now have effect on our texture object

	// Set texture filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// Load, create texture 
	int width, height;
	unsigned char* image = SOIL_load_image("road.png", &width, &height, 0, SOIL_LOAD_RGBA);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture when done, so we won't accidentily mess up our texture.

	// Set up vertex data (and buffer(s)) and attribute pointers
	frame_width3 = (float)width;
	frame_height3 = (float)height;
	GLfloat vertices[] = {
		// Positions   // Colors           // Texture Coords
		1,  1, 0.0f,   1.0f, 1.0f, 1.0f,   1.0f, 1.0f, // Bottom Right
		1,  0, 0.0f,   1.0f, 1.0f, 0.0f,   1.0f, 0.0f, // Top Right
		0,  0, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 0.0f, // Top Left
		0,  1, 0.0f,   1.0f, 1.0f, 1.0f,   0.0f, 1.0f  // Bottom Left 
	};

	GLuint indices[] = {  // Note that we start from 0!
		0, 3, 2, 1
	};

	glGenVertexArrays(1, &VAO3);
	glGenBuffers(1, &VBO3);
	glGenBuffers(1, &EBO3);

	glBindVertexArray(VAO3);

	glBindBuffer(GL_ARRAY_BUFFER, VBO3);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO3);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), 0);
	glEnableVertexAttribArray(0);
	// Color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	// TexCoord attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	glBindVertexArray(0); // Unbind VAO

	// Set orthographic projection
	mat4 projection;
	projection = ortho(0.0f, static_cast<GLfloat>(GetScreenWidth()), static_cast<GLfloat>(GetScreenHeight()), 0.0f, -1.0f, 1.0f);
	glUniformMatrix4fv(glGetUniformLocation(this->program3, "projection"), 1, GL_FALSE, value_ptr(projection));

	// set sprite position, gravity, velocity
	xpos3 = (GetScreenWidth() - frame_width3) / 5;
	ypos3 = GetScreenHeight() - frame_height3;
}

void Demo::DrawLeftWallSprite() {
	// Bind Textures using texture units
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texture3);
	// Activate shader
	UseShader(this->program3);
	glUniform1i(glGetUniformLocation(this->program3, "ourTexture"), 1);

	mat4 model;
	// Translate sprite along x-axis
	model = translate(model, vec3(xpos3, ypos3, 0.1f));
	// Scale sprite 
	model = scale(model, vec3(frame_width3, frame_height3, 1));
	glUniformMatrix4fv(glGetUniformLocation(this->program3, "model"), 1, GL_FALSE, value_ptr(model));

	// Draw sprite
	glBindVertexArray(VAO3);
	glDrawElements(GL_QUADS, 4, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture when done, so we won't accidentily mess up our texture.
}

void Demo::BuildDividerSprite()
{
	this->program4 = BuildShader("crateSprite.vert", "crateSprite.frag");
	UseShader(this->program4);
	glUniform1f(glGetUniformLocation(this->program4, "n"), 1.0f / NUM_FRAMES);

	// Load and create a texture 
	glGenTextures(1, &texture4);
	glBindTexture(GL_TEXTURE_2D, texture4); // All upcoming GL_TEXTURE_2D operations now have effect on our texture object

	// Set texture filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// Load, create texture 
	int width, height;
	unsigned char* image = SOIL_load_image("road2.png", &width, &height, 0, SOIL_LOAD_RGBA);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture when done, so we won't accidentily mess up our texture.

	// Set up vertex data (and buffer(s)) and attribute pointers
	frame_width4 = (float)width;
	frame_height4 = (float)height;
	GLfloat vertices[] = {
		// Positions   // Colors           // Texture Coords
		1,  1, 0.0f,   1.0f, 1.0f, 1.0f,   1.0f, 1.0f, // Bottom Right
		1,  0, 0.0f,   1.0f, 1.0f, 0.0f,   1.0f, 0.0f, // Top Right
		0,  0, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 0.0f, // Top Left
		0,  1, 0.0f,   1.0f, 1.0f, 1.0f,   0.0f, 1.0f  // Bottom Left 
	};

	GLuint indices[] = {  // Note that we start from 0!
		0, 3, 2, 1
	};

	glGenVertexArrays(1, &VAO4);
	glGenBuffers(1, &VBO4);
	glGenBuffers(1, &EBO4);

	glBindVertexArray(VAO4);

	glBindBuffer(GL_ARRAY_BUFFER, VBO4);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO4);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), 0);
	glEnableVertexAttribArray(0);
	// Color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	// TexCoord attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	glBindVertexArray(0); // Unbind VAO

	// Set orthographic projection
	mat4 projection;
	projection = ortho(0.0f, static_cast<GLfloat>(GetScreenWidth()), static_cast<GLfloat>(GetScreenHeight()), 0.0f, -1.0f, 1.0f);
	glUniformMatrix4fv(glGetUniformLocation(this->program4, "projection"), 1, GL_FALSE, value_ptr(projection));

	// set sprite position, gravity, velocity
	xpos4 = (GetScreenHeight()) / 1.8;
	ypos4 = 35;
	gravity4 = 0.05f;
	yVelocity4 = 0.5f;

	//InputMapping("Move Right", SDLK_d);
	//InputMapping("Move Left", SDLK_a);
}

void Demo::DrawDividerSprite() {
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	// Bind Textures using texture units
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texture4);
	// Activate shader
	UseShader(this->program4);
	glUniform1i(glGetUniformLocation(this->program4, "ourTexture"), 1);

	//// set flip
	//glUniform1i(glGetUniformLocation(this->program4, "flip"), flip);

	//mat4 model;
	//// Translate sprite along x-axis
	//model = translate(model, vec3(xpos4, ypos4, 0.1f));
	//// Scale sprite 
	//model = scale(model, vec3(frame_width4, frame_height4, 1));
	//glUniformMatrix4fv(glGetUniformLocation(this->program4, "model"), 1, GL_FALSE, value_ptr(model));

	// Draw sprite
	glBindVertexArray(VAO4);
	glDrawElements(GL_QUADS, 4, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture when done, so we won't accidentily mess up our texture.
	glDisable(GL_BLEND);
}

void Demo::UpdateDividerAnim(float deltaTime)
{
	// Update animation
	frame_dur4 += deltaTime;

	if (walk_anim4 && frame_dur4 > FRAME_DUR) {
		frame_dur4 = 0;
		if (frame_idx4 == NUM_FRAMES - 1) frame_idx4 = 0;  else frame_idx4++;

		// Pass frameIndex to shader
		UseShader(this->program4);
		glUniform1i(glGetUniformLocation(this->program4, "frameIndex"), frame_idx4);
	}
}

void Demo::ControlDivider(float deltaTime)
{
	//ypos4 += deltaTime * 0.1f;
	ypos4 += deltaTime * yVelocity4;

	if (ypos4 > GetScreenWidth()-525) ypos4 = 35;

	mat4 model;
	/*model = translate(model, vec3(ypos4 - frame_width4, 0, 0.0f));*/
	model = translate(model, vec3(xpos4, ypos4, 0.0f));

	//// Rotate sprite at origin
	//model = translate(model, vec3(0.5f * frame_width4, 0.5f * frame_height4, 0.0f));
	//model = rotate(model, radians(180.0f), vec3(0.0f, 0.0f, 1.0f));
	//model = translate(model, glm::vec3(-0.5f * frame_width4, -0.5f * frame_height4, 0.0f));

	// Scale sprite 
	model = scale(model, vec3(frame_width4, frame_height4, 1));


	UseShader(this->program4);
	glUniformMatrix4fv(glGetUniformLocation(this->program4, "model"), 1, GL_FALSE, value_ptr(model));

}

//Enemy
void Demo::UpdateEnemyAnim(float deltaTime)
{
	// Update animation
	frame_dur5 += deltaTime;

	if (walk_anim5 && frame_dur5 > FRAME_DUR) {
		//lokasi = 1;
		frame_dur5 = 0;
		if (frame_idx5 == NUM_FRAMES - 1) frame_idx5 = 0;  else frame_idx5++;

		// Pass frameIndex to shader
		GLint location = glGetUniformLocation(this->program5, "frameIndex");
		UseShader(this->program5);
		glUniform1i(location, frame_idx5);
	}
}

void Demo::ControlEnemy(float deltaTime)
{
	ypos5 += deltaTime * yVelocity5;

	if (ypos5 > GetScreenWidth()-170) {
		ypos5 = 35;
		//std::random_device rd; // obtain a random number from hardware
		//std::mt19937 gen(rd()); // seed the generator
		//std::uniform_int_distribution<> distr(2, 3); // define the range
		float a = 1.3 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (2.3 - 1.3)));
		score++;
		xpos5 = (GetScreenHeight()) / a;
	}

	mat4 model;
	model = translate(model, vec3(xpos5, ypos5, 0.0f));

	// Rotate sprite at origin
	/*model = translate(model, vec3(0.5f * frame_width5, 0.5f * frame_height5, 0.0f));
	model = rotate(model, radians(180.0f), vec3(0.0f, 0.0f, 1.0f));
	model = translate(model, glm::vec3(-0.5f * frame_width5, -0.5f * frame_height5, 0.0f));*/

	// Scale sprite 
	model = scale(model, vec3(frame_width5, frame_height5, 1));

	UseShader(this->program5);
	glUniformMatrix4fv(glGetUniformLocation(this->program5, "model"), 1, GL_FALSE, value_ptr(model));
}

void Demo::DrawEnemySprite() {
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Bind Textures using texture units
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture5);

	// Activate shader
	UseShader(this->program5);
	glUniform1i(glGetUniformLocation(this->program5, "ourTexture"), 0);
	glUniform1i(glGetUniformLocation(this->program5, "flip"), flip);

	mat4 model;
	model = translate(model, vec3(xpos5, ypos5, 0.0f));

	model = scale(model, vec3(frame_width5, frame_height5, 1));
	glUniformMatrix4fv(glGetUniformLocation(this->program5, "model"), 1, GL_FALSE, value_ptr(model));

	// Draw sprite
	glBindVertexArray(VAO5);
	glDrawElements(GL_QUADS, 4, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture when done, so we won't accidentily mess up our texture.
	glDisable(GL_BLEND);
}

void Demo::BuildEnemySprite()
{
	for (size_t i = 0; i < 3; i++)
	{
		this->program5 = BuildShader("playerSprite.vert", "playerSprite.frag");

		// Pass n to shader
		GLint location = glGetUniformLocation(this->program5, "n");
		UseShader(this->program5);
		glUniform1f(location, 1.0f / NUM_FRAMES);

		// Load and create a texture 
		glGenTextures(1, &texture5);
		glBindTexture(GL_TEXTURE_2D, texture5); // All upcoming GL_TEXTURE_2D operations now have effect on our texture object

		// Set texture filtering
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		// Load, create texture 
		int width, height;
		unsigned char* image = SOIL_load_image("carblack.png", &width, &height, 0, SOIL_LOAD_RGBA);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
		SOIL_free_image_data(image);
		glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture when done, so we won't accidentily mess up our texture.

		// Set up vertex data (and buffer(s)) and attribute pointers
		frame_width5 = ((float)width) / NUM_FRAMES;
		frame_height5 = (float)height;
		GLfloat vertices[] = {
			// Positions   // Colors           // Texture Coords
			1,  1, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // Bottom Right
			1,  0, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 0.0f, // Top Right
			0,  0, 0.0f,   1.0f, 1.0f, 1.0f,   0.0f, 0.0f, // Top Left
			0,  1, 0.0f,   1.0f, 1.0f, 1.0f,   0.0f, 1.0f  // Bottom Left 
		};

		GLuint indices[] = {  // Note that we start from 0!
			0, 3, 2, 1
		};

		glGenVertexArrays(1, &VAO5);
		glGenBuffers(1, &VBO5);
		glGenBuffers(1, &EBO5);

		glBindVertexArray(VAO5);

		glBindBuffer(GL_ARRAY_BUFFER, VBO5);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO5);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

		// Position attribute
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), 0);
		glEnableVertexAttribArray(0);
		// Color attribute
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
		glEnableVertexAttribArray(1);
		// TexCoord attribute
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
		glEnableVertexAttribArray(2);

		glBindVertexArray(0); // Unbind VAO

		// Set orthographic projection
		mat4 projection;
		projection = ortho(0.0f, static_cast<GLfloat>(GetScreenWidth()), static_cast<GLfloat>(GetScreenHeight()), 0.0f, -1.0f, 1.0f);
		UseShader(this->program5);
		glUniformMatrix4fv(glGetUniformLocation(this->program5, "projection"), 1, GL_FALSE, value_ptr(projection));

		// set sprite position, gravity, velocity
		/*float r3 = 1.1 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (10 - 1.1)));*/
		//srand(time(NULL));
		//for (int t = 0; t < 10; t++)
		//{
		//	int random_x;
		//	random_x = rand() % 100;
		//	xpos5 = (GetScreenHeight()) / random_x;
		//	
		//}

		//if (rand() % 2 == 0) {
		//	lokasi = 1;
		//	xpos5 = (GetScreenHeight()) / lokasi;
		//	ypos5 = GetScreenWidth() - frame_width5;
		//	gravity5 = 0.05f;
		//	yVelocity5 = 0.5f;
		//}
		//else {
		//	lokasi = 2.3;
		//	xpos5 = (GetScreenHeight()) / lokasi;
		//	ypos5 = GetScreenWidth() - frame_width5;
		//	gravity5 = 0.05f;
		//	yVelocity5 = 0.5f;
		//}
		xpos5 = (GetScreenHeight()) / 2.3;
		ypos5 = 35;
		gravity5 = 0.05f;
		yVelocity5 = 0.5f;
		
	}
}


bool Demo::IsCollided(float x1, float y1, float width1, float height1,
	float x2, float y2, float width2, float height2, float x3, float y3, float width3, float height3, float x5, float y5, float width5, float height5) {
	return (x1 < x2 + width2 && x1 + width1 > x2 && y1 < y2 + height2 && y1 + height1 > y2 || x1 < x3 + width3 && x1 + width1 > x3 && y1 < y3 + height3 && y1 + height1 > y3
		|| x1 < x5 + width5 && x1 + width1 > x5 && y1 < y5 + height5 && y1 + height1 > y5);
}


//int main(int argc, char** argv) {
//
//	Engine::Game& game = Demo();
//	game.Start("User Interface gui", 800, 600, true, WindowFlag::WINDOWED, 60, 1);
//
//	return 0;
//}