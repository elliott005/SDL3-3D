#pragma once

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <iostream>
#include <vector>
#define _USE_MATH_DEFINES
#include <math.h>

SDL_Surface* loadSurface(std::string path, SDL_Surface* window_surface);

//class Matrix {
//public:
//    Matrix(double p_a, double p_b, double p_c, double p_e, double p_f, double p_g, double p_j, double p_k, double p_o);
//    double a; double b; double c; double e; double f; double g; double j; double k; double o;
//};
//
//class Vec {
//public:
//    Vec(double p_x, double p_y, double p_z);
//    double x; double y; double z;
//};
//
//double vec_dotProduct(Vec v1, Vec v2);
//
//Vec vec_crossProduct(Vec v1, Vec v2);
//
//Vec vec_substraction(Vec v1, Vec v2);
//
//double rad(double degrees);

class LTexture
{
public:
    //Initializes texture variables
    LTexture();

    //Cleans up texture variables
    ~LTexture();

    //Loads texture from disk
    bool loadFromFile(std::string path, SDL_Renderer* renderer);

    //Cleans up texture
    void destroy();

    //Draws texture
    void render(float x, float y, SDL_Renderer* renderer);

    //Gets texture dimensions
    int getWidth();
    int getHeight();

private:
    //Contains texture data
    SDL_Texture* mTexture = nullptr;

    //Texture dimensions
    int mWidth;
    int mHeight;
};