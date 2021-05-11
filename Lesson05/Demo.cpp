#include "Demo.h"


Demo::Demo()
{
}


Demo::~Demo()
{
}

void Demo::Init()
{
	BuildPlayerSprite();
	//BuildRoad();
}

void Demo::Update(float deltaTime)
{
	if (IsKeyDown("Quit")) {
		SDL_Quit();
		exit(0);
	}
	UpdateSpriteAnim(deltaTime);
	ControlPlayerSprite(deltaTime);

	//UpdateRoadAnim(deltaTime);
	//ControlRoad(deltaTime);
}

void Demo::Render()
{
	//Setting Viewport
	glViewport(0, 0, GetScreenWidth(), GetScreenHeight());

	//Clear the color and depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Set the background color
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

	DrawPlayerSprite();
	//DrawRoad();
}

void Demo::UpdateSpriteAnim(float deltaTime)
{
	// Update animation
	frame_dur += deltaTime;

	if (walk_anim && frame_dur > FRAME_DUR) {
		frame_dur = 0;
		if (frame_idx == NUM_FRAMES - 1) frame_idx = 0;  else frame_idx++;

		// Pass frameIndex to shader
		glUniform1i(glGetUniformLocation(this->program, "frameIndex"), frame_idx);
	}
}

void Demo::ControlPlayerSprite(float deltaTime)
{
	walk_anim = false;

	if (IsKeyDown("Move Right")) {
		ypos += deltaTime * yVelocity;
		flip = 0;
		walk_anim = true;
	}

	if (IsKeyDown("Move Left")) {
		ypos -= deltaTime * yVelocity;
		flip = 1;
		walk_anim = true;
	}

	if (IsKeyDown("Jump")) {
		if (onGround) {
			xVelocity = -12.0f;
			onGround = false;
		}
	}

	if (IsKeyUp("Jump")) {
		if (xVelocity < -6.0f) {
			xVelocity = -6.0f;
		}
	}

	xVelocity += gravity * deltaTime;
	xpos += deltaTime * xVelocity;

	if (xpos > xposGround) {
		xpos = xposGround;
		xVelocity = 0;
		onGround = true;
	}


	glUniform1i(glGetUniformLocation(this->program, "flip"), flip);

	mat4 model;
	// Translate sprite along x-axis
	model = translate(model, vec3(xpos, ypos, 0.0f));
	// Scale sprite 
	model = scale(model, vec3(frame_width, frame_height, 1));
	glUniformMatrix4fv(glGetUniformLocation(this->program, "model"), 1, GL_FALSE, value_ptr(model));
}

void Demo::DrawPlayerSprite() {
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Bind Textures using texture units
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);

	// Activate shader
	UseShader(this->program);

	// Draw sprite
	glBindVertexArray(VAO);
	glDrawElements(GL_QUADS, 4, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture when done, so we won't accidentily mess up our texture.
	glDisable(GL_BLEND);
}

void Demo::BuildPlayerSprite()
{
	this->program = BuildShader("spriteAnim.vert", "spriteAnim.frag");

	// Pass n to shader
	GLint location = glGetUniformLocation(this->program, "n");
	UseShader(this->program);
	glUniform1f(location, 1.0f / NUM_FRAMES);

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
	UseShader(this->program);
	glUniformMatrix4fv(glGetUniformLocation(this->program, "projection"), 1, GL_FALSE, value_ptr(projection));

	// set sprite position, gravity, velocity
	ypos = (GetScreenWidth() - frame_width) / 2;
	xposGround = GetScreenHeight() - frame_height;
	xpos = xposGround;
	gravity = 0.05f;
	yVelocity = 0.5f;

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
	InputMapping("Jump", SDLK_SPACE);
	InputMapping("Jump", SDL_CONTROLLER_BUTTON_A);
}

//create road

//void Demo::UpdateRoadAnim(float deltaTime)
//{
//	// Update animation
//	frame_dur += deltaTime;
//
//	if (walk_anim && frame_dur > FRAME_DUR) {
//		frame_dur = 0;
//		if (frame_idx == NUM_FRAMES - 1) frame_idx = 0;  else frame_idx++;
//
//		// Pass frameIndex to shader
//		glUniform1i(glGetUniformLocation(this->program, "frameIndex"), frame_idx);
//	}
//}
//
//void Demo::ControlRoad(float deltaTime)
//{
//	walk_anim = false;
//
//	if (IsKeyDown("Move Right")) {
//		xpos += deltaTime * xVelocity;
//		flip = 0;
//		walk_anim = true;
//	}
//
//	if (IsKeyDown("Move Left")) {
//		xpos -= deltaTime * xVelocity;
//		flip = 1;
//		walk_anim = true;
//	}
//
//	if (IsKeyDown("Jump")) {
//		if (onGround) {
//			yVelocity = -12.0f;
//			onGround = false;
//		}
//	}
//
//	if (IsKeyUp("Jump")) {
//		if (yVelocity < -6.0f) {
//			yVelocity = -6.0f;
//		}
//	}
//
//	yVelocity += gravity * deltaTime;
//	ypos += deltaTime * yVelocity;
//
//	if (ypos > yposGround) {
//		ypos = yposGround;
//		yVelocity = 0;
//		onGround = true;
//	}
//
//
//	glUniform1i(glGetUniformLocation(this->program, "flip"), flip);
//
//	mat4 model;
//	// Translate sprite along x-axis
//	model = translate(model, vec3(xpos, ypos, 0.0f));
//	// Scale sprite 
//	model = scale(model, vec3(frame_width, frame_height, 1));
//	glUniformMatrix4fv(glGetUniformLocation(this->program, "model"), 1, GL_FALSE, value_ptr(model));
//}
//
//void Demo::DrawRoad() {
//	glEnable(GL_BLEND);
//	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//
//	// Bind Textures using texture units
//	glActiveTexture(GL_TEXTURE0);
//	glBindTexture(GL_TEXTURE_2D, texture);
//
//	// Activate shader
//	UseShader(this->program);
//
//	// Draw sprite
//	glBindVertexArray(VAO);
//	glDrawElements(GL_QUADS, 4, GL_UNSIGNED_INT, 0);
//	glBindVertexArray(0);
//
//	glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture when done, so we won't accidentily mess up our texture.
//	glDisable(GL_BLEND);
//}
//
//void Demo::BuildRoad()
//{
//	this->program = BuildShader("spriteAnim.vert", "spriteAnim.frag");
//
//	// Pass n to shader
//	GLint location = glGetUniformLocation(this->program, "n");
//	UseShader(this->program);
//	glUniform1f(location, 1.0f / NUM_FRAMES);
//
//	// Load and create a texture 
//	glGenTextures(1, &texture);
//	glBindTexture(GL_TEXTURE_2D, texture); // All upcoming GL_TEXTURE_2D operations now have effect on our texture object
//
//	// Set texture filtering
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//
//	// Load, create texture 
//	int width, height;
//	unsigned char* image = SOIL_load_image("homeranim.png", &width, &height, 0, SOIL_LOAD_RGBA);
//	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
//	SOIL_free_image_data(image);
//	glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture when done, so we won't accidentily mess up our texture.
//
//	// Set up vertex data (and buffer(s)) and attribute pointers
//	frame_width = ((float)width) / NUM_FRAMES;
//	frame_height = (float)height;
//	GLfloat vertices[] = {
//		// Positions   // Colors           // Texture Coords
//		3,  3, 0.0f,   1.0f, 1.0f, 1.0f,   1.0f, 1.0f, // Bottom Right
//		3,  2, 0.0f,   1.0f, 1.0f, 0.0f,   1.0f, 0.0f, // Top Right
//		2,  2, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 0.0f, // Top Left
//		2,  3, 0.0f,   1.0f, 1.0f, 1.0f,   0.0f, 1.0f  // Bottom Left 
//	};
//
//	GLuint indices[] = {  // Note that we start from 0!
//		0, 3, 2, 1
//	};
//
//	glGenVertexArrays(1, &VAO);
//	glGenBuffers(1, &VBO);
//	glGenBuffers(1, &EBO);
//
//	glBindVertexArray(VAO);
//
//	glBindBuffer(GL_ARRAY_BUFFER, VBO);
//	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
//
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
//	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
//
//	// Position attribute
//	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), 0);
//	glEnableVertexAttribArray(0);
//	// Color attribute
//	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
//	glEnableVertexAttribArray(1);
//	// TexCoord attribute
//	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
//	glEnableVertexAttribArray(2);
//
//	glBindVertexArray(0); // Unbind VAO
//
//	// Set orthographic projection
//	mat4 projection;
//	projection = ortho(0.0f, static_cast<GLfloat>(GetScreenWidth()), static_cast<GLfloat>(GetScreenHeight()), 0.0f, -1.0f, 1.0f);
//	UseShader(this->program);
//	glUniformMatrix4fv(glGetUniformLocation(this->program, "projection"), 1, GL_FALSE, value_ptr(projection));
//
//	// set sprite position, gravity, velocity
//	xpos = (GetScreenWidth() - frame_width + 200) / 2;
//	yposGround = GetScreenHeight() - frame_height + 200;
//	ypos = yposGround;
//	gravity = 0.05f;
//	xVelocity = 0.5f;
//
//	// Add input mapping
//	// to offer input change flexibility you can save the input mapping configuration in a configuration file (non-hard code) !
//	InputMapping("Move Right", SDLK_RIGHT);
//	InputMapping("Move Left", SDLK_LEFT);
//	InputMapping("Move Right", SDLK_d);
//	InputMapping("Move Left", SDLK_a);
//	InputMapping("Move Right", SDL_BUTTON_RIGHT);
//	InputMapping("Move Left", SDL_BUTTON_LEFT);
//	InputMapping("Move Right", SDL_CONTROLLER_BUTTON_DPAD_RIGHT);
//	InputMapping("Move Left", SDL_CONTROLLER_BUTTON_DPAD_LEFT);
//	InputMapping("Quit", SDLK_ESCAPE);
//	InputMapping("Jump", SDLK_SPACE);
//	InputMapping("Jump", SDL_CONTROLLER_BUTTON_A);
//}




int main(int argc, char** argv) {

	Engine::Game &game = Demo();
	game.Start("Move Animated Sprite using Input Device", 800, 600, false, WindowFlag::WINDOWED, 60, 1);

	return 0;
}
