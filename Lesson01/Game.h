#ifndef GAME_H
#define GAME_H

#include <SDL/SDL.h>
#include <GL/glew.h>
#include <string>
#include <iostream>

using namespace std;

enum class State { RUNNING, EXIT };
enum class WindowFlag { WINDOWED, FULLSCREEN, EXCLUSIVE_FULLSCREEN, BORDERLESS};

namespace Engine {
	class Game
	{
	public:
		Game();
		~Game();
		void Start(string title, unsigned int width, unsigned int height, bool vsync, WindowFlag windowFlag, unsigned int targetFrameRate, float timeScale);
	protected:
		virtual void Init() = 0;
		virtual void Update(float deltaTime) = 0;
		virtual void Render() = 0;
	private:
		unsigned int screenWidth, screenHeight, lastFrame = 0, last = 0, _fps = 0, fps = 0;
		float targetFrameTime = 0, timeScale;
		State state;
		float GetDeltaTime();
		void GetFPS();
		void PollInput();
		void Err(string errorString);
		void LimitFPS();
		void PrintFPS();
	};
}
#endif
