#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_image/SDL_image.h>
#include <stdio.h>
#include <string>
#include <vector>

#include "application.hpp"
#include "Cube.hpp"

int main(int argc, char* args[]) {
	Application game("3D!!!!");

	//Main loop flag
	bool quit = false;

	double frameRate = 1.0 / 60.0;

	Uint64 NOW = SDL_GetPerformanceCounter();
	Uint64 LAST = 0;
	double deltaTime = 0;

	Point playerPos(0, 0, 0);
	double playerSpeed = 30.0;
	double playerRotSpeed = 1.0;
	Point playerRot(0, 0, 0);

	int scale = 20;

	std::vector<Cube> cubes = {
		Cube(0, 0, -100, scale, 100, 200, 0),
		Cube(0, 0, 100, scale, 10, 100, 200)
	};

	std::vector<int> keysPressed;


	//While application is running
	while (!quit) {
		while (SDL_PollEvent(&game.m_window_event) != 0)
		{
			//User requests quit
			if (game.m_window_event.type == SDL_EVENT_QUIT)
			{
				quit = true;
			} else if (game.m_window_event.type == SDL_EVENT_KEY_DOWN) {
				std::vector<int>::iterator it = std::find(keysPressed.begin(), keysPressed.end(), game.m_window_event.key.key);
				if (it == keysPressed.end()) {
					keysPressed.push_back(game.m_window_event.key.key);
				}
				switch (game.m_window_event.key.key)
				{
				case SDLK_ESCAPE:
					quit = true;
				default:
					break;
				}
			}
			else if (game.m_window_event.type == SDL_EVENT_KEY_UP) {
				std::vector<int>::iterator it = std::find(keysPressed.begin(), keysPressed.end(), game.m_window_event.key.key);
				if (it != keysPressed.end()) {
					keysPressed.erase(it);
				}
			}
		}

		LAST = NOW;
		NOW = SDL_GetPerformanceCounter();
		deltaTime = (double)((NOW - LAST) * 1000 / (double)SDL_GetPerformanceFrequency());
		deltaTime *= 0.001;


		for (int k : keysPressed) {
			switch (k)
			{
			case SDLK_LEFT:
				playerPos += transform(getRotationMatrix(Point(0, playerRot.y, 0)), Point(-1, 0, 0)) * playerSpeed * deltaTime;
				break;
			case SDLK_RIGHT:
				playerPos += transform(getRotationMatrix(Point(0, playerRot.y, 0)), Point(1, 0, 0)) * playerSpeed * deltaTime;
				break;
			case SDLK_UP:
				playerPos += transform(getRotationMatrix(Point(0, playerRot.y, 0)), Point(0, 0, 1)) * playerSpeed * deltaTime;
				break;
			case SDLK_DOWN:
				playerPos += transform(getRotationMatrix(Point(0, playerRot.y, 0)), Point(0, 0, -1)) * playerSpeed * deltaTime;
				break;
			case SDLK_A:
				playerRot.y -= playerRotSpeed * deltaTime;
				break;
			case SDLK_D:
				playerRot.y += playerRotSpeed * deltaTime;
				break;
			case SDLK_W:
				playerRot.x += playerRotSpeed * deltaTime;
				break;
			case SDLK_S:
				playerRot.x -= playerRotSpeed * deltaTime;
				break;
			default:
				break;
			}
		}

		for (Cube cube : cubes) {
			cube.draw(game.m_renderer, game.SCREEN_WIDTH, game.SCREEN_HEIGHT, playerPos, playerRot);
		}

		game.updateScreen();

		SDL_Delay(std::max(0.0, frameRate - deltaTime) * 1000);
	}

	return 0;
}