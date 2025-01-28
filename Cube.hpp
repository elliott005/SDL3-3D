#pragma once

#include <SDL3/SDL.h>
#include <algorithm>
#include <tuple>
#include <set>

#include "helpers.hpp"

typedef std::vector<double> Vector;
typedef std::vector<Vector> Matrix;

class Point {
public:
    double x, y, z;

    Point() {
        this->x = 0;
        this->y = 0;
        this->z = 0;
    }

    Point(double x, double y, double z) {
        this->x = x;
        this->y = y;
        this->z = z;
    }

    Point(double x, double y) {
        this->x = x;
        this->y = y;
        this->z = 0;
    }

    double operator[](int i) const {
        if (i == 0) return this->x;
        if (i == 1) return this->y;
        return this->z;
    }

    double& operator[](int i) {
        if (i == 0) return this->x;
        if (i == 1) return this->y;
        return this->z;
    }

    Point operator*(double other) {
        return Point(this->x * other, this->y * other, this->z * other);
    }
    void operator*=(double other) {
        this->x *= other;
        this->y *= other;
        this->z *= other;
    }

    Point operator/(double other) {
        return Point(this->x / other, this->y / other, this->z / other);
    }
    void operator/=(double other) {
        this->x /= other;
        this->y /= other;
        this->z /= other;
    }

    Point operator-(Point other) {
        return Point(this->x - other.x, this->y - other.y, this->z - other.z);
    }
    void operator-=(Point other) {
        this->x -= other.x;
        this->y -= other.y;
        this->z -= other.z;
    }

    Point operator+(Point other) {
        return Point(this->x + other.x, this->y + other.y, this->z + other.z);
    }
    void operator+=(Point other) {
        this->x += other.x;
        this->y += other.y;
        this->z += other.z;
    }

    bool operator==(Point other) {
        return this->x == other.x and this->y == other.y;
    }

    double length() {
        return sqrt(pow(this->x, 2) + pow(this->y, 2) + pow(this->z, 2));
    }
};

Matrix dot(const Matrix& a, const Matrix& b);

Point transform(const Matrix& matrix, const Point& point);

Point translate(const Point& shift, const Point& point);

double vecDistance(Point v1, Point v2);

std::vector<Point> connect(SDL_Renderer* const renderer, const std::vector<Point>& points, int i, int j, Point playerPos, Point playerRot, double scale, double screenOffsetX, double screenOffsetY);

Matrix getRotationMatrix(Point rot);

Point vecSubstraction(Point v1, Point v2);

double vecDotProduct(Point v1, Point v2);

Point vecCrossProduct(Point v1, Point v2);

Point faceNormal(int face[4], std::vector<Point> points);

bool isVisible(int face[4], std::vector<Point> points, Point playerPos, Point playerRot);

bool isOnScreen(Point v, double screenSizeX, double screenSizeY);

Point screenProj(Point coords, double screenOffsetX, double screenOffsetY);

bool isVertexEqual(SDL_Vertex v1, SDL_Vertex v2);

int sign(double n);

double vecAngle(Point v1, Point v2);

bool doIntersect(Point& p1, Point& q1, Point& p2, Point& q2);

int orientation(Point& p, Point& q, Point& r);

std::pair<std::pair<Point, Point>, std::pair<Point, Point>> findDiagonals(std::vector<Point>& vertices);

std::tuple<Point, bool> rayPlaneIntersection(Point planeNormal, Point planeCenter, Point rayDirection, Point rayOrigin);

SDL_Vertex pointToVertex(Point point, double r, double g, double b);

class Cube {
public:
    Cube(double x, double y, double z, double p_width, double p_height, double p_depth, double scale, double p_r, double p_g, double p_b);

    void draw(SDL_Renderer* renderer, int screenWidth, int screenHeight, Point playerPos, Point playerRot);

    Point pos;

private:
    /*int vertices[8][3] = {
        {0, 0, 0},
        {10, 0, 0},
        {10, 10, 0},
        {0, 10, 0},
        {0, 0, 10},
        {10, 0, 10},
        {10, 10, 10},
        {0, 10, 10}
    };*/

    void resetPoints();

    int faces[6][4] = {
        {0, 1, 2, 3},
        {1, 5, 6, 2},
        {4, 0, 3, 7},
        {4, 5, 1, 0},
        {7, 3, 2, 6},
        {5, 4, 7, 6}
    };
    std::vector<Point> points;

    double scale;
    double width; double height; double depth;
    double r; double g; double b;
};

bool cubeComparator(Cube a, Cube b);
