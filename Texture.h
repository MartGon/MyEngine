#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <string>
#include "Vector2.h"

extern struct MapRGB
{
	Uint8 red = 0;
	Uint8 green = 0;
	Uint8 blue = 0;
};

class Texture
{
public:
	// Constructors
	Texture();
	Texture(const char* resourcePath, SDL_Surface* screenSurface);
	Texture(const char* resourcePath, SDL_Renderer* renderer);
	Texture(const char* resourcePath, SDL_Renderer* renderer, MapRGB *colorKey);
	~Texture();
	
	// SDL Stuff
	SDL_Texture *mTexture = nullptr;
	SDL_Surface *optimizedSurface = nullptr;
	SDL_Renderer *mRenderer = nullptr;

	// Dimensions
	int mWidth = 10;
	int mHeight = 10;

	// Scale
	Vector2<float> scale;

	// Other
	static const char* folder;
	const char* path = nullptr;

	// Methods

	void render(int x, int y);
	void setAlpha(Uint8 alpha);
	Uint8 getAlpha();
	void free();
	std::string getPathFromResourceFolder(const char* localPath);
	bool isValid();
	bool load(const char* path, SDL_Renderer *renderer, MapRGB *colorKey = nullptr);
};

