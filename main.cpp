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
	double playerSpeed = 60.0;
	double playerRotSpeed = 1.5;
	Point playerRot(0, 0, 0);
	double playerRotLimitX = 0.8;

	int scale = 20;

	std::vector<Cube> cubes = {
		Cube(0, 0, -250, 10, 1, 10, scale, 0.7, 0.5, 0),
		Cube(0, 0, 300, 1, 1, 10, scale, 0, 1, 1),
		Cube(100, 0, 0, 1, 5, 1, scale, 0, 1, 0),
		Cube(-150, 0, 0, 1, 1, 1, scale, 0, 0, 0)
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

			case SDLK_X:
				playerPos.y -= playerSpeed * deltaTime;
				break;
			case SDLK_C:
				playerPos.y += playerSpeed * deltaTime;
				break;

			case SDLK_SPACE:
				playerPos = Point();
				playerRot = Point();
				break;

			case SDLK_A:
				playerRot.y -= playerRotSpeed * deltaTime;
				break;
			case SDLK_D:
				playerRot.y += playerRotSpeed * deltaTime;
				break;
			case SDLK_W:
				//playerRot += transform(getRotationMatrix(Point(0, -playerRot.y, 0)), Point(1, 0, 0)) * playerRotSpeed * deltaTime;
				//printf("%f, %f, %f\n", playerRot.x, playerRot.y, playerRot.z);
				playerRot.x += playerRotSpeed * deltaTime;
				break;
			case SDLK_S:
				playerRot.x -= playerRotSpeed * deltaTime;
				break;
			default:
				break;
			}
		}
		if (playerRot.x) {
			playerRot.x = std::min(playerRotLimitX, std::max(-playerRotLimitX, playerRot.x));
		}

		for (Cube& cube : cubes) {
			cube.pos = translate(playerPos, cube.pos);
			/*Point playerVerticalRot = transform(getRotationMatrix(Point(0, playerRot.y, 0)), Point(playerRot.x, 0, 0));
			Matrix playerRotMatrix = getRotationMatrix(Point(-playerVerticalRot.x, -playerRot.y, -playerVerticalRot.z));
			cube.pos = transform(playerRotMatrix, cube.pos);*/
		}
		std::sort(cubes.begin(), cubes.end(), cubeComparator);
		//printf("other: %f, %f, %f\n", cubes[0].pos.x, cubes[0].pos.y, cubes[0].pos.z);
		
		int idx = 0;
		for (Cube& cube : cubes) {
			cube.pos = translate(playerPos * -1, cube.pos);
			cube.draw(game.m_renderer, game.SCREEN_WIDTH, game.SCREEN_HEIGHT, playerPos, playerRot);
			idx++;
		}

		game.updateScreen();

		SDL_Delay(std::max(0.0, frameRate - deltaTime) * 1000);
	}

	return 0;
}