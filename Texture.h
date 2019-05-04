#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <string>
#include "Vector2.h"
#include "TextureFactory.h"

struct MapRGB
{
	MapRGB() {}
	MapRGB(Uint8 red, Uint8 green, Uint8 blue) : red{red}, green{green}, blue{blue}
	{}

	Uint8 red = 0;
	Uint8 green = 0;
	Uint8 blue = 0;
};

class Texture
{
public:
	// Constructors
	Texture();
	Texture(const char* resourcePath);
	Texture(const char* resourcePath, SDL_Renderer* renderer);
	Texture(const char* resourcePath, SDL_Renderer* renderer, MapRGB *colorKey);
	~Texture();
	
	// Thread safe vars
	Monitor<SDL_Texture*>* texture = new Monitor<SDL_Texture*>(nullptr);
	Monitor<SurfaceResult>* result = new Monitor<SurfaceResult>(SurfaceResult());

	// SDL Stuff
	SDL_Texture *mTexture = nullptr;
	SDL_Renderer *mRenderer = nullptr;

	// Dimensions
	int mWidth = 10;
	int mHeight = 10;

	// Scale
	// This value is modified by texureRenderer
	Vector2<float> scale = Vector2<float>(1, 1);

	// Other
	static const char* folder;
	std::string path;

	// Methods
	void render(int x, int y, double angle = 0, SDL_Point* center = nullptr, SDL_RendererFlip flip = SDL_FLIP_NONE);

	// Alpha setter/getter
	void setAlpha(Uint8 alpha);
	Uint8 getAlpha();

	// Checking
	bool isValid();

	// Free resources
	void free();

private:
	// Alpha
	Uint8 alpha = SDL_ALPHA_OPAQUE;

	// Methods
	std::string getPathFromResourceFolder(const char* localPath);
	bool load(const char* path, SDL_Renderer *renderer, MapRGB *colorKey = nullptr);
};

