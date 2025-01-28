#include "Cube.hpp"

Cube::Cube(double x, double y, double z, double p_width, double p_height, double p_depth, double p_scale, int p_r, int p_g, int p_b) {
	pos.x = x;
	pos.y = y;
	pos.z = z;
	scale = p_scale;

	width = p_width; height = p_height; depth = p_depth;
	r = p_r; g = p_g; b = p_b;

	/*std::tuple<Point, bool> ipTuple = rayPlaneIntersection(Point(0, 0, -1), Point(0, 0, -5), Point(1, 0, 1), Point(1, 0, -10));
	Point ip = std::get<0>(ipTuple);
	printf("%i, %f, %f, %f\n", std::get<1>(ipTuple), ip.x, ip.y, ip.z);*/
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
			std::vector<Point> facePoints;
			for (int j = 0; j < 4; j++) {
				std::vector<Point> pointsVector = connect(renderer, points, faces[i][j], faces[i][(j + 1) % 4], playerPos, playerRot, scale, screenOffsetX, screenOffsetY);
				if (pointsVector.size()) {
					for (int idx = 0; idx < pointsVector.size(); idx++) {
						Point point = pointsVector[idx];
						facePoints.push_back(point);
					}
				}
				
			}
			std::vector<Point> facePointsUnique;
			for (const Point& point1 : facePoints) {
				bool exists = false;
				for (const Point& point2 : facePointsUnique) {
					if (point1.x == point2.x and point1.y == point2.y) {
						exists = true;
						break;
					}
				}
				if (not exists) {
					facePointsUnique.push_back(point1);
				}
			}
			if (facePointsUnique.size() >= 3) {
				std::vector<SDL_Vertex> trig1Array;
				if (facePointsUnique.size() == 4) {
					std::pair<std::pair<Point, Point>, std::pair<Point, Point>> diags = findDiagonals(facePointsUnique);
					/*printf("first.first: %f, %f\n", diags.first.first.x, diags.first.first.y);
					printf("first.second: %f, %f\n", diags.first.second.x, diags.first.second.y);
					printf("second.first: %f, %f\n", diags.second.first.x, diags.second.first.y);
					printf("second.second: %f, %f\n", diags.second.second.x, diags.second.second.y);*/
					Point second;
					for (const Point& point : facePoints) {
						if (not (diags.first.first == point) and not (diags.first.second == point) and not (diags.second.first == point)) {
							second = point;
							break;
						}
					}
					//printf("second: %f, %f\n", second.x, second.y);
					trig1Array = {
						pointToVertex(diags.second.second, r, g, b),
						pointToVertex(second, r, g, b),
						pointToVertex(diags.first.second, r, g, b),
						pointToVertex(second, r, g, b),
						pointToVertex(diags.second.second, r, g, b),
						pointToVertex(diags.second.first, r, g, b),
					};
				}
				else if (facePointsUnique.size() == 3) {
					trig1Array = {
						pointToVertex(facePointsUnique[0], r, g, b),
						pointToVertex(facePointsUnique[1], r, g, b),
						pointToVertex(facePointsUnique[2], r, g, b),
					};
				}
				else if (facePointsUnique.size() == 5) {
					trig1Array = {
						pointToVertex(facePointsUnique[0], r, g, b),
						pointToVertex(facePointsUnique[1], r, g, b),
						pointToVertex(facePointsUnique[2], r, g, b),

						pointToVertex(facePointsUnique[2], r, g, b),
						pointToVertex(facePointsUnique[0], r, g, b),
						pointToVertex(facePointsUnique[3], r, g, b),

						pointToVertex(facePointsUnique[0], r, g, b),
						pointToVertex(facePointsUnique[3], r, g, b),
						pointToVertex(facePointsUnique[4], r, g, b),
					};
				}
				if (trig1Array.size()) {
					bool success = SDL_RenderGeometry(renderer, NULL, trig1Array.data(), trig1Array.size(), NULL, 0);
					if (not success) {
						printf("could not render geometry! SDL Error: %s\n", SDL_GetError());
					}
				}
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

double vecAngle(Point v1, Point v2) {
	return pow(std::cos(vecDotProduct(v1, v2) / (std::abs(v1.length() * v2.length()))), -1);
}

Point faceNormal(int face[4], std::vector<Point> points) {
	Point vs1 = vecSubstraction(points[face[1]], points[face[0]]);
	Point vs2 = vecSubstraction(points[face[2]], points[face[0]]);
	return vecCrossProduct(vs1, vs2);
}

bool isVisible(int face[4], std::vector<Point> points, Point playerPos, Point playerRot) {
	//playerPos.z += 5.0;
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

bool isVertexEqual(SDL_Vertex v1, SDL_Vertex v2) {
	return (v1.position.x == v2.position.x) and (v1.position.y == v2.position.y);
}

int orientation(Point& p, Point& q, Point& r) {
	double val = (q.y - p.y) * (r.x - q.x) - (q.x - p.x) * (r.y - q.y);
	if (val == 0) return 0;              // Collinear
	return (val > 0) ? 1 : 2;            // Clockwise or Counterclockwise
}

// Function to check if two segments p1q1 and p2q2 intersect
bool doIntersect(Point& p1, Point& q1, Point& p2, Point& q2) {
	// Find the orientations
	int o1 = orientation(p1, q1, p2);
	int o2 = orientation(p1, q1, q2);
	int o3 = orientation(p2, q2, p1);
	int o4 = orientation(p2, q2, q1);

	// General case
	if (o1 != o2 && o3 != o4) return true;

	return false; // No intersection
}

std::pair<std::pair<Point, Point>, std::pair<Point, Point>> findDiagonals(std::vector<Point>& vertices) {
	if (vertices.size() != 4) {
		printf("There must be exactly 4 vertices.");
		throw std::invalid_argument("There must be exactly 4 vertices.");
	}

	// Check all pairs of lines to find the intersecting ones
	for (size_t i = 0; i < 4; ++i) {
		for (size_t j = i + 2; j < 4; ++j) {
			size_t next_i = (i + 1) % 4; // Next point for edge i
			size_t next_j = (j + 1) % 4; // Next point for edge j

			if (doIntersect(vertices[i], vertices[next_i], vertices[j], vertices[next_j])) {
				// The two diagonals are (i, j) and (next_i, next_j)
				return { {vertices[i], vertices[j]}, {vertices[next_i], vertices[next_j]} };
			}
		}
	}

	printf("Invalid quadrilateral: no intersecting diagonals found.");
	throw std::logic_error("Invalid quadrilateral: no intersecting diagonals found.");
}

SDL_Vertex pointToVertex(Point point, double r, double g, double b) {
	SDL_Vertex vert;
	vert.position.x = point.x;
	vert.position.y = point.y;
	vert.color.r = r;
	vert.color.g = g;
	vert.color.b = b;
	vert.color.a = 1.0;
	vert.tex_coord = SDL_FPoint{ 0 };
	return vert;
}

std::vector<Point> connect(SDL_Renderer* const renderer, const std::vector<Point>& points, int i, int j, Point playerPos, Point playerRot, double scale, double screenOffsetX, double screenOffsetY) {
	double screenSizeX = screenOffsetX * 2;
	double screenSizeY = screenOffsetY * 2;
	
	Point playerVerticalRot = transform(getRotationMatrix(Point(0, playerRot.y, 0)), Point(playerRot.x, 0, 0));
	//printf("%f, %f\n", playerVerticalRot.x, playerVerticalRot.z);
	Matrix playerRotMatrix = getRotationMatrix(Point(-playerVerticalRot.x, -playerRot.y, -playerVerticalRot.z));
	Point posI = translate(playerPos, points[i]);
	posI = transform(playerRotMatrix, posI);
	//printf("dist: %f\n", vecDistance(playerPos, pos));
	Point iScreenCoords = screenProj(posI, screenOffsetX, screenOffsetY);
	bool iInside = true;
	if (not isOnScreen(iScreenCoords, screenSizeX, screenSizeY)) {
		iInside = false;
	}
	Point posJ = translate(playerPos, points[j]);
	posJ = transform(playerRotMatrix, posJ);
	Point jScreenCoords = screenProj(posJ, screenOffsetX, screenOffsetY);
	bool jInside = true;
	if (not isOnScreen(jScreenCoords, screenSizeX, screenSizeY)) {
		jInside = false;
	}
	//printf("x: %f, y: %f, z: %f\n", iScreenCoords.x, iScreenCoords.y, iScreenCoords.z);
	//printf("(jScreenCoords - iScreenCoords).length(): %f\n", (jScreenCoords - iScreenCoords).length());
	
	if (iInside and jInside) {
		/*SDL_RenderLine(
			renderer,
			iScreenCoords.x,
			iScreenCoords.y,
			jScreenCoords.x,
			jScreenCoords.y
		);*/
		return {iScreenCoords, jScreenCoords};
	} else if (iInside or jInside) {
		Point vec1 = iScreenCoords;
		Point vec2 = jScreenCoords;
		if (jInside) {
			vec1 = jScreenCoords;
			vec2 = iScreenCoords;
		}

		// line ray
		Point rayOrigin = vec1;
		double t = (vec2 - rayOrigin).length();
		Point rayDirection = (vec2 - rayOrigin) / t;
		// viewport plane
		Point planeCenter(0, 0, -5);
		Point planeNormal(0, 0, -1);

		Point intersectionPoint;
		bool intersects = true;

		std::tuple<Point, bool> ipTuple = rayPlaneIntersection(planeNormal, planeCenter, rayDirection, rayOrigin);
		if (std::get<1>(ipTuple)) {
			intersectionPoint = std::get<0>(ipTuple);
		}
		else {
			double frustumDistance = -5.0;
			Point planeCenter(frustumDistance, 0, -5);
			Point planeNormal(1, 0, 0);
			std::tuple<Point, bool> ipTuple = rayPlaneIntersection(planeNormal, planeCenter, rayDirection, rayOrigin);
			if (std::get<1>(ipTuple)) {
				intersectionPoint = std::get<0>(ipTuple);
			}
			else {
				Point planeCenter(-frustumDistance, 0, -5);
				Point planeNormal(-1, 0, 0);
				std::tuple<Point, bool> ipTuple = rayPlaneIntersection(planeNormal, planeCenter, rayDirection, rayOrigin);
				if (std::get<1>(ipTuple)) {
					intersectionPoint = std::get<0>(ipTuple);
				}
				else {
					Point planeCenter(0, frustumDistance, -5);
					Point planeNormal(0, 1, 0);
					std::tuple<Point, bool> ipTuple = rayPlaneIntersection(planeNormal, planeCenter, rayDirection, rayOrigin);
					if (std::get<1>(ipTuple)) {
						intersectionPoint = std::get<0>(ipTuple);
					}
					else {
						Point planeCenter(0, -frustumDistance, -5);
						Point planeNormal(0, -1, 0);
						std::tuple<Point, bool> ipTuple = rayPlaneIntersection(planeNormal, planeCenter, rayDirection, rayOrigin);
						if (std::get<1>(ipTuple)) {
							intersectionPoint = std::get<0>(ipTuple);
						}
						else {
							intersects = false;
						}
					}
				}
			}
		}

		if (intersects) {
			if (vec2.z >= 0.0) {
				intersectionPoint -= Point(vec1.x, vec1.y, vec1.z);
				intersectionPoint *= -1.0;
				intersectionPoint *= std::abs(intersectionPoint.z);
				intersectionPoint += vec1;
			}
			else if (vec2.z >= -5.0) {
				intersectionPoint -= Point(vec1.x, vec1.y, vec1.z);
				intersectionPoint *= std::abs(intersectionPoint.z);
				intersectionPoint += vec1;
			}
			vec1 = iScreenCoords;
			if (jInside) {
				vec1 = jScreenCoords;
			}
			/*SDL_RenderLine(
				renderer,
				vec1.x,
				vec1.y,
				intersectionPoint.x,
				intersectionPoint.y
			);*/
			if (jInside) {
				return { intersectionPoint, vec1 };
			}
			else {
				return { vec1, intersectionPoint };
			}
		}
	}
	/*else {
		return { posJ };
	}*/
}

bool isOnScreen(Point v, double screenSizeX, double screenSizeY) {
	return (v.z < -5.0);// and (v.x > 0.0) and (v.y > 0.0) and (v.x < screenSizeX) and (v.y < screenSizeY);
}

std::tuple<Point, bool> rayPlaneIntersection(Point planeNormal, Point planeCenter, Point rayDirection, Point rayOrigin) {
	//float denom = normal.dot(ray.direction);
	//if (abs(denom) > 0.0001f) // your favorite epsilon
	//{
	//	float t = (center - ray.origin).dot(normal) / denom;
	//	if (t >= 0) return true; // you might want to allow an epsilon here too
	//}
	//return false;
	Point diff = rayOrigin - planeCenter;
	double prod1 = vecDotProduct(diff, planeNormal);
	double prod2 = vecDotProduct(rayDirection, planeNormal);
	if (std::abs(prod2) > 0.0001f) {
		double prod3 = -prod1 / prod2;
		if (prod3 >= 0.0) {
			Point intersectionPoint = rayOrigin + rayDirection * prod3;
			return std::make_tuple(intersectionPoint, true);
		}
	}
	/*else {
		return std::make_tuple(Point(-1, 0), false);
	}*/
	return std::make_tuple(Point(), false);

	/*Vector3D diff = rayPoint - planePoint;
	double prod1 = diff.dot(planeNormal);
	double prod2 = rayVector.dot(planeNormal);
	double prod3 = prod1 / prod2;
	return rayPoint - rayVector * prod3;*/
}

int sign(double n) {
	if (n < 0.0) {
		return -1;
	}
	else if (n > 0.0) {
		return 1;
	}
	return 0;
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