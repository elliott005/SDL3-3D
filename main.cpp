#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_image/SDL_image.h>
#include <stdio.h>
#include <string>
#include <vector>

#include "application.hpp"
#include "Cube.hpp"
#include "Player.hpp"

int main(int argc, char* args[]) {
	Application game("3D!!!!");

	SDL_SetWindowRelativeMouseMode(game.m_window, true);

	//Main loop flag
	bool quit = false;

	double frameRate = 1.0 / 60.0;

	Uint64 NOW = SDL_GetPerformanceCounter();
	Uint64 LAST = 0;
	double deltaTime = 0;

	int scale = 20;

	std::vector<Cube> cubes = {
		Cube(0, 0, 0, 5, 1, 5, scale, 0.5, 0.5, 0.5),
		Cube(0, 10, -100, 10, 1, 2, scale, 0.7, 0.5, 0),
		Cube(-140, 0, -100, 1, 3, 1, scale, 0, 1, 1),
		Cube(-150, 0, 0, 1, 1, 1, scale, 0, 0, 0),
		Cube(40, 50, 0, 1, 5, 1, scale, 0, 1, 0),
	};

	Player player(0, -100, 0, 0, 0, 0);

	std::vector<int> keysPressed;
	std::vector<int> keysPressedOnce;

	//While application is running
	while (!quit) {
		keysPressedOnce.clear();
		while (SDL_PollEvent(&game.m_window_event) != 0)
		{
			//User requests quit
			if (game.m_window_event.type == SDL_EVENT_QUIT)
			{
				quit = true;
			} else if (game.m_window_event.type == SDL_EVENT_KEY_DOWN) {
				std::vector<int>::iterator it2 = std::find(keysPressed.begin(), keysPressed.end(), game.m_window_event.key.key);
				if (it2 == keysPressed.end()) {
					keysPressedOnce.push_back(game.m_window_event.key.key);
				}
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

		player.update(deltaTime, keysPressed, keysPressedOnce, cubes);

		for (Cube& cube : cubes) {
			cube.pos = translate(player.pos, cube.pos);
			/*Point playerVerticalRot = transform(getRotationMatrix(Point(0, playerRot.y, 0)), Point(playerRot.x, 0, 0));
			Matrix playerRotMatrix = getRotationMatrix(Point(-playerVerticalRot.x, -playerRot.y, -playerVerticalRot.z));
			cube.pos = transform(playerRotMatrix, cube.pos);*/
		}
		std::sort(cubes.begin(), cubes.end(), cubeComparator);
		//printf("other: %f, %f, %f\n", cubes[0].pos.x, cubes[0].pos.y, cubes[0].pos.z);
		
		int idx = 0;
		for (Cube& cube : cubes) {
			cube.pos = translate(player.pos * -1, cube.pos);
			cube.draw(game.m_renderer, game.SCREEN_WIDTH, game.SCREEN_HEIGHT, player.pos, player.rot);
			idx++;
		}

		game.updateScreen();

		SDL_Delay(std::max(0.0, frameRate - deltaTime) * 1000);
	}

	return 0;
}