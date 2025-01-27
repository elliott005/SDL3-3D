#include "Cube.hpp"

Cube::Cube(double x, double y, double z, double p_width, double p_height, double p_depth, double p_scale, int p_r, int p_g, int p_b) {
	pos.x = x;
	pos.y = y;
	pos.z = z;
	scale = p_scale;

	width = p_width; height = p_height; depth = p_depth;
	r = p_r; g = p_g; b = p_b;
}

void Cube::draw(SDL_Renderer* renderer, int screenWidth, int screenHeight, Point playerPos, Point playerRot) {
	resetPoints();
	for (Point& p : points) {
		//p = transform(getRotationMatrix(playerRot * -1), p);
		p.x = (scale * p.x + pos.x);
		p.y = (scale * p.y + pos.y);
		p.z = (scale * p.z + pos.z);
	}
	
	double screenOffsetX = screenWidth / 2;
	double screenOffsetY = screenHeight / 2;

	SDL_SetRenderDrawColor(renderer, r, g, b, 255);
	/*for (int i = 0; i < 4; i++) {
		connect(renderer, points, i, (i + 1) % 4, screenOffsetX, screenOffsetY);
		connect(renderer, points, i + 4, ((i + 1) % 4) + 4, screenOffsetX, screenOffsetY);
		connect(renderer, points, i, i + 4, screenOffsetX, screenOffsetY);
	}*/
	for (int i = 0; i < 6; i++) {
		if (isVisible(faces[i], points, playerPos, playerRot)) {
			for (int j = 0; j < 4; j++) {
				connect(renderer, points, faces[i][j], faces[i][(j + 1) % 4], playerPos, playerRot, scale, screenOffsetX, screenOffsetY);
			}
		}
	}
}

void Cube::resetPoints() {
	points = {
		Point(-width / 2, -height / 2, -depth / 2),
		Point(width / 2, -height / 2, -depth / 2),
		Point(width / 2, height / 2, -depth / 2),
		Point(-width / 2, height / 2, -depth / 2),
		Point(-width / 2, -height / 2, depth / 2),
		Point(width / 2, -height / 2, depth / 2),
		Point(width / 2, height / 2, depth / 2),
		Point(-width / 2, height / 2, depth / 2)
	};
}

Matrix dot(const Matrix& a, const Matrix& b) {
	Matrix result = Matrix(a.size(), Vector(b[0].size(), 0));
	for (int i = 0; i < a.size(); i++) {
		for (int j = 0; j < b[0].size(); j++) {
			for (int k = 0; k < b.size(); k++) {
				result[i][j] += a[i][k] * b[k][j];
			}
		}
	}
	return result;
}

Point transform(const Matrix& matrix, const Point& point) {
	Matrix p = { {point.x}, {point.y}, {point.z} };
	Matrix r = dot(matrix, p);
	return Point(r[0][0], r[1][0], r[2][0]);
}

Point translate(const Point& shift, const Point& point) {
	return Point(
		point.x + shift.x,
		point.y + shift.y,
		point.z + shift.z
	);
}

Point vecSubstraction(Point v1, Point v2) {
	return Point(
		v1.x - v2.x,
		v1.y - v2.y,
		v1.z - v2.z
	);
}

double vecDotProduct(Point v1, Point v2) {
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

Point vecCrossProduct(Point v1, Point v2) {
	return Point(
		v1.y * v2.z - v1.z * v2.y,
		v1.z * v2.x - v1.x * v2.z,
		v1.x * v2.y - v1.y * v2.x
	);
}

Point faceNormal(int face[4], std::vector<Point> points) {
	Point vs1 = vecSubstraction(points[face[1]], points[face[0]]);
	Point vs2 = vecSubstraction(points[face[2]], points[face[0]]);
	return vecCrossProduct(vs1, vs2);
}

bool isVisible(int face[4], std::vector<Point> points, Point playerPos, Point PlayerRot) {
	Point viewVec = vecSubstraction(playerPos * -1, points[face[0]]);
	//Point viewVec = transform(getRotationMatrix(PlayerRot), Point(0, 0, -1));
	return vecDotProduct(faceNormal(face, points), viewVec) < 0;
}

Point screenProj(Point coords, double screenOffsetX, double screenOffsetY) {
	return Point(
		(coords.x / coords.z * screenOffsetX) + screenOffsetX,
		(coords.y / coords.z * screenOffsetY) + screenOffsetY,
		coords.z
	);
}

double vecDistance(Point v1, Point v2) {
	return sqrt(pow(v1.x - v2.x, 2) + pow(v1.y - v2.y, 2) + pow(v1.z - v2.z, 2));
}

void connect(SDL_Renderer* const renderer, const std::vector<Point>& points, int i, int j, Point playerPos, Point playerRot, double scale, double screenOffsetX, double screenOffsetY) {
	Point playerVerticalRot = transform(getRotationMatrix(Point(0, playerRot.y, 0)), Point(playerRot.x, 0, 0));
	//printf("%f, %f\n", playerVerticalRot.x, playerVerticalRot.z);
	Matrix playerRotMatrix = getRotationMatrix(Point(-playerVerticalRot.x, -playerRot.y, -playerVerticalRot.z));
	Point pos = translate(playerPos, points[i]);
	pos = transform(playerRotMatrix, pos);
	int outside = 0;
	if (not isOnScreen(pos, screenOffsetX * 2, screenOffsetY * 2)) {
		outside++;
	}
	//printf("dist: %f\n", vecDistance(playerPos, pos));
	Point iScreenCoords = screenProj(pos, screenOffsetX, screenOffsetY);
	pos = translate(playerPos, points[j]);
	pos = transform(playerRotMatrix, pos);
	if (not isOnScreen(pos, screenOffsetX * 2, screenOffsetY * 2)) {
		outside++;
	}
	Point jScreenCoords = screenProj(pos, screenOffsetX, screenOffsetY);
	//printf("x: %f, y: %f, z: %f\n", iScreenCoords.x, iScreenCoords.y, iScreenCoords.z);
	//printf("(jScreenCoords - iScreenCoords).length(): %f\n", (jScreenCoords - iScreenCoords).length());
	
	if (outside < 1) {
		SDL_RenderLine(
			renderer,
			iScreenCoords.x,
			iScreenCoords.y,
			jScreenCoords.x,
			jScreenCoords.y
		);
	}
	//else {
	//	double len = (jScreenCoords - iScreenCoords).length();
	//	//printf("%f, %f\n", len, scale);
	//	for (int i = 0; i < len; i += scale) {
	//		Point vec1;
	//		if (i == 0) {
	//			vec1 = iScreenCoords;
	//		}
	//		else {
	//			vec1 = iScreenCoords + (jScreenCoords - iScreenCoords) * i / len;
	//		}
	//		Point vec2;
	//		if (i + scale >= len) {
	//			vec2 = jScreenCoords;
	//		}
	//		else {
	//			vec2 = jScreenCoords + (iScreenCoords - jScreenCoords) * (i + scale) / len;
	//		}
	//		//printf("vec1: %f, %f, %f, vec2: %f, %f, %f\n", vec1.x, vec1.y, vec1.z, vec2.x, vec2.y, vec2.z);
	//		if (isOnScreen(vec1, screenOffsetX * 2, screenOffsetY * 2) and isOnScreen(vec2, screenOffsetX * 2, screenOffsetY * 2)) {
	//			SDL_RenderLine(
	//				renderer,
	//				vec1.x,
	//				vec1.y,
	//				vec2.x,
	//				vec2.y
	//			);
	//		}
	//	}
	//}
}

bool isOnScreen(Point v, double screenSizeX, double screenSizeY) {
	return (v.z < -5.0) or ((v.x < -screenSizeX) or (v.y < -screenSizeY) or (v.x > 2 * screenSizeX) or (v.y > 2 * screenSizeY));
}

Matrix getRotationMatrix(Point rot) {
	double alpha = rot.x;//0.001;
	Matrix rotationX = {
		{1, 0, 0},
		{0, cos(alpha), -sin(alpha)},
		{0, sin(alpha), cos(alpha)}
	};

	double beta = rot.y;//0.002;
	Matrix rotationY = {
		{cos(beta), 0, sin(beta)},
		{0, 1, 0},
		{-sin(beta), 0, cos(beta)}
	};

	double gamma = rot.z;//0.003;
	Matrix rotationZ = {
		{cos(gamma), -sin(gamma), 0},
		{sin(gamma), cos(gamma), 0},
		{0, 0, 1}
	};

	return dot(rotationY, dot(rotationX, rotationZ));
	//return dot(rotationZ, dot(rotationY, rotationX));
}