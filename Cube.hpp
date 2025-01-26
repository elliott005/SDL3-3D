#pragma once

#include <SDL3/SDL.h>
#include <algorithm>

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

    double length() {
        return sqrt(pow(this->x, 2) + pow(this->y, 2) + pow(this->z, 2));
    }
};

Matrix dot(const Matrix& a, const Matrix& b);

Point transform(const Matrix& matrix, const Point& point);

Point translate(const Point& shift, const Point& point);

double vecDistance(Point v1, Point v2);

void connect(SDL_Renderer* const renderer, const std::vector<Point>& points, int i, int j, Point playerPos, Point playerRot, double screenOffsetX, double screenOffsetY);

Matrix getRotationMatrix(Point rot);

Point vecSubstraction(Point v1, Point v2);

double vecDotProduct(Point v1, Point v2);

Point vecCrossProduct(Point v1, Point v2);

Point faceNormal(int face[4], std::vector<Point> points);

bool isVisible(int face[4], std::vector<Point> points, Point playerPos, Point playerRot);

Point screenProj(Point coords, double screenOffsetX, double screenOffsetY);

class Cube {
public:
    Cube(double x, double y, double z, double scale, int p_r, int p_g, int p_b);

    void draw(SDL_Renderer* renderer, int screenWidth, int screenHeight, Point playerPos, Point playerRot);

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
    std::vector<Point> points = {
        Point(0, 0, 0),
        Point(1, 0, 0),
        Point(1, 1, 0),
        Point(0, 1, 0),
        Point(0, 0, 1),
        Point(1, 0, 1),
        Point(1, 1, 1),
        Point(0, 1, 1)
    };
    Point pos;

    double scale;

    int r; int g; int b;
};