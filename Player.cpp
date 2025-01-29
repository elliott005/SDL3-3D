#include "Player.hpp"

Player::Player(double x, double y, double z, double rx, double ry, double rz) {
	pos = Point(x, y, z);
	rot = Point(rx, ry, rz);
}

void Player::update(double deltaTime, std::vector<int> keysPressed, std::vector<int> keysPressedOnce, std::vector<Cube> cubes) {
	float* mouseX = &mouseBaseX;
	float* mouseY = &mouseBaseY;
	SDL_GetRelativeMouseState(mouseX, mouseY);

	//printf("%f, %f\n", *mouseX, *mouseY);
	rot.y += *mouseX * mouseSensitivity;
	rot.x -= *mouseY * mouseSensitivity;

	//velocity = Point(0, velocity.y, 0);

	bool movementInputed = false;

	for (int k : keysPressed) {
		switch (k)
		{
		case SDLK_A:
			velocity += transform(getRotationMatrix(Point(0, rot.y, 0)), Point(-1, 0, 0)) * acceleration * deltaTime;
			movementInputed = true;
			break;
		case SDLK_D:
			velocity += transform(getRotationMatrix(Point(0, rot.y, 0)), Point(1, 0, 0)) * acceleration * deltaTime;
			movementInputed = true;
			break;
		case SDLK_W:
			velocity += transform(getRotationMatrix(Point(0, rot.y, 0)), Point(0, 0, 1)) * acceleration * deltaTime;
			movementInputed = true;
			break;
		case SDLK_S:
			velocity += transform(getRotationMatrix(Point(0, rot.y, 0)), Point(0, 0, -1)) * acceleration * deltaTime;
			movementInputed = true;
			break;

		/*case SDLK_E:
			pos.y -= speed * deltaTime;
			break;
		case SDLK_Q:
			pos.y += speed * deltaTime;
			break;*/

		case SDLK_RETURN:
			pos = Point(0, -100, 0);
			rot = Point();
			velocity = Point();
			break;

		case SDLK_LEFT:
			rot.y -= rotSpeed * deltaTime;
			break;
		case SDLK_RIGHT:
			rot.y += rotSpeed * deltaTime;
			break;
		case SDLK_UP:
			//playerRot += transform(getRotationMatrix(Point(0, -playerRot.y, 0)), Point(1, 0, 0)) * playerRotSpeed * deltaTime;
			//printf("%f, %f, %f\n", playerRot.x, playerRot.y, playerRot.z);
			rot.x += rotSpeed * deltaTime;
			break;
		case SDLK_DOWN:
			rot.x -= rotSpeed * deltaTime;
			break;
		default:
			break;
		}
	}

	Point speed = Point(velocity.x, 0, velocity.z);
	if (speed.length() > maxSpeed) {
		speed = speed / speed.length() * maxSpeed;
	}
	velocity.x = speed.x;
	velocity.z = speed.z;

	if (not movementInputed) {
		Point currentSpeed = Point(velocity.x, 0, velocity.z);
		currentSpeed = currentSpeed / (currentSpeed.length() - friction * deltaTime);
		velocity.x = currentSpeed.x;
		velocity.z = currentSpeed.z;
	}

	for (int k : keysPressedOnce) {
		switch (k)
		{
		case SDLK_SPACE:
			if (onFloor) {
				velocity.y = -jumpStrength;
			}
			break;
		default:
			break;
		}
	}

	if (rot.x) {
		rot.x = std::min(rotLimitX, std::max(-rotLimitX, rot.x));
	}

	//printf("%i\n", canMoveY);
	velocity.y += gravity * deltaTime;

	onFloor = false;
	newPos = translate(velocity * deltaTime, pos);

	if (checkCollision(&newPos, cubes)) {
		pos = newPos;
	}

	if (not canMoveY) {
		velocity.y = 0.0;
	}
}

bool Player::checkCollision(Point* position, std::vector<Cube> cubes) {
	canMoveX = true;
	canMoveY = true;
	canMoveZ = true;
	for (Cube cube : cubes) {
		if (checkAxis(*position, Point(1, 0, 0), cube)) {
			canMoveX = false;
		}
		if (checkAxis(*position, Point(0, 1, 0), cube)) {
			if ((*position - pos).y > 0.0) {
				onFloor = true;
			}
			canMoveY = false;
		}
		if (checkAxis(*position, Point(0, 0, 1), cube)) {
			canMoveZ = false;
		}
		if (not canMoveX and not canMoveY and not canMoveZ) {
			break;
		}
	}
	//printf("%i\n", canMoveY);
	if (canMoveX or canMoveY or canMoveZ) {
		if (not canMoveX) {
			position->x = pos.x;
		}
		if (not canMoveY) {
			position->y = pos.y;
		}
		if (not canMoveZ) {
			position->z = pos.z;
		}
		return true;
	}
	return false;
}

bool Player::checkAxis(Point position, Point axis, Cube cube) {
	Point tempPoint = pos + (position - pos) * axis;

	float cubeLeft = (-cube.pos.x - cube.width / 2 * cube.scale);
	float cubeRight = (-cube.pos.x + cube.width / 2 * cube.scale);
	float cubeBottom = cube.pos.y - cube.height / 2 * cube.scale;
	float cubeTop = cube.pos.y + cube.height / 2 * cube.scale;
	float cubeFront =- cube.pos.z - cube.depth / 2 * cube.scale;
	float cubeBack = -cube.pos.z + cube.depth / 2 * cube.scale;
	//printf("cube sides: %f, %f, %f, %f; pos: %f, %f, %f\n", cubeFront, cubeBack, cubeRight, cubeLeft, pos.x, pos.y, pos.z);
	//printf("cube pos: %f, %f, %f; cube width: %f\n", cube.pos.x, cube.pos.y, cube.pos.z, cube.width);
	return (
		((cubeLeft < tempPoint.x + width / 2 and tempPoint.x + width / 2 < cubeRight) or (cubeLeft < tempPoint.x - width / 2 and tempPoint.x - width / 2 < cubeRight)) and
		((cubeBottom < -tempPoint.y and -tempPoint.y < cubeTop) or (cubeBottom < -tempPoint.y - height and -tempPoint.y - height < cubeTop)) and
		((cubeFront < tempPoint.z + width / 2 and tempPoint.z + width / 2 < cubeBack) or (cubeFront < tempPoint.z - width / 2 and tempPoint.z - width / 2 < cubeBack))
	);
}