#pragma once

#include <SDL3/SDL.h>
#include <vector>

#include "Cube.hpp"

class Player {
public:
	Player(double x, double y, double z, double rx, double ry, double rz);
	
	void update(double deltaTime, std::vector<int> keysPressed, std::vector<int> keysPressedOnce, std::vector<Cube> cubes);

	Point pos;
	Point rot;

private:
	bool checkCollision(Point* position, std::vector<Cube> cubes);
	bool checkAxis(Point position, Point axis, Cube cube);

	float mouseBaseX = 0.0;
	float mouseBaseY = 0.0;
	double rotLimitX = 0.8;
	float mouseSensitivity = 0.002;
	double maxSpeed = 60.0;
	double acceleration = 250.0;
	double friction = 250.0;
	double rotSpeed = 1.5;

	double gravity = 50.0;
	Point velocity{ 0, 0, 0 };
	double jumpStrength = 35.0;
	double width = 15.0;
	double height = 25.0;

	Point newPos;

	bool canMoveX = true;
	bool canMoveY = true;
	bool canMoveZ = true;

	bool onFloor = false;
};