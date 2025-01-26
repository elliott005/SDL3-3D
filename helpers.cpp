#include "helpers.hpp"


SDL_Surface* loadSurface(std::string path, SDL_Surface* window_surface)
{
	//The final optimized image
	SDL_Surface* optimizedSurface = NULL;

	//Load image at specified path
	SDL_Surface* loadedSurface = IMG_Load(path.c_str());
	if (!loadedSurface)
	{
		printf("Unable to load image %s! SDL_image Error: %s\n", path.c_str(), SDL_GetError());
	}
	else
	{
		//Convert surface to screen format
		//printf("%i", window_surface->format); //370546692; 0x16161804
		optimizedSurface = SDL_ConvertSurface(loadedSurface, SDL_PIXELFORMAT_ARGB8888);
		if (optimizedSurface == NULL)
		{
			printf("Unable to optimize image %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
		}

		/*int success = SDL_SetSurfaceBlendMode(optimizedSurface, SDL_BLENDMODE_BLEND);
		if (success != 0) {
			printf("SDL_SetSurfaceBlendMode failed with exit code: %i! SDL Error: %s\n", success, SDL_GetError());
		}*/

		//Get rid of old loaded surface
		SDL_DestroySurface(loadedSurface);
	}

	return optimizedSurface;
}

//Matrix::Matrix(double p_a, double p_b, double p_c, double p_e, double p_f, double p_g, double p_j, double p_k, double p_o) {
//	a = p_a;
//	b = p_b;
//	c = p_c;
//	e = p_e;
//	f = p_f;
//	g = p_g;
//	j = p_j;
//	k = p_k;
//	o = p_o;
//}
//
//Vec::Vec(double p_x, double p_y, double p_z) {
//	x = p_x;
//	y = p_y;
//	z = p_z;
//}
//
//double vec_dotProduct(Vec v1, Vec v2) {
//	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
//}
//
//Vec vec_crossProduct(Vec v1, Vec v2) {
//	return Vec(
//		v1.y * v2.z - v1.z * v2.y,
//		v1.z * v2.x - v1.x * v2.z,
//		v1.x * v2.y - v1.y * v2.x
//	);
//}
//
//Vec vec_substraction(Vec v1, Vec v2) {
//	return Vec(
//		v1.x - v2.x,
//		v1.y - v2.y,
//		v1.z - v2.z
//	);
//}

double rad(double degrees) {
	return degrees * (M_PI / 180);
}

LTexture::LTexture() 
{
	//Initialize texture variables
	mTexture = nullptr;
	mWidth = 0;
	mHeight = 0;
}

LTexture::~LTexture()
{
	//Clean up texture
	destroy();
}

bool LTexture::loadFromFile(std::string path, SDL_Renderer* renderer)
{
	//Clean up texture if it already exists
	destroy();

	//Load surface
	SDL_Surface* loadedSurface = IMG_Load(path.c_str());
	if (loadedSurface == nullptr)
	{
		SDL_Log("Unable to load image %s! SDL_image error: %s\n", path.c_str(), SDL_GetError());
	}
	else
	{
		//Create texture from surface
		mTexture = SDL_CreateTextureFromSurface(renderer, loadedSurface);
		if (mTexture == nullptr)
		{
			SDL_Log("Unable to create texture from loaded pixels! SDL error: %s\n", SDL_GetError());
		}
		else
		{
			//Get image dimensions
			mWidth = loadedSurface->w;
			mHeight = loadedSurface->h;
		}

		//Clean up loaded surface
		SDL_DestroySurface(loadedSurface);
	}

	//Return success if texture loaded
	return mTexture != nullptr;
}

void LTexture::destroy()
{
	//Clean up texture
	SDL_DestroyTexture(mTexture);
	mTexture = nullptr;
	mWidth = 0;
	mHeight = 0;
}

void LTexture::render(float x, float y, SDL_Renderer* renderer)
{
	//Set texture position
	SDL_FRect dstRect = { x, y, static_cast<float>(mWidth), static_cast<float>(mHeight) };

	//Render texture
	SDL_RenderTexture(renderer, mTexture, nullptr, &dstRect);
}

int LTexture::getWidth()
{
	return mWidth;
}

int LTexture::getHeight()
{
	return mHeight;
}