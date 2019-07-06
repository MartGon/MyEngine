#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <string>
#include "Vector2.h"
<<<<<<< HEAD

#include <unordered_map>
=======
>>>>>>> a08af820f0891ce4e34601920c02045c7d9db40e

struct MapRGB
{
	MapRGB() {}
	MapRGB(Uint8 red, Uint8 green, Uint8 blue) : red{red}, green{green}, blue{blue}
	{}

	Uint8 red = 0;
	Uint8 green = 0;
	Uint8 blue = 0;
};

struct TextureData
{
	SDL_Texture* texture = nullptr;
	int w = 0;
	int h = 0;
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
	
	// SDL Stuff
	SDL_Texture *mTexture = nullptr;
<<<<<<< HEAD
	SDL_Surface *imgSurface = nullptr;
=======
	SDL_Surface *optimizedSurface = nullptr;
>>>>>>> a08af820f0891ce4e34601920c02045c7d9db40e
	SDL_Renderer *mRenderer = nullptr;

	// Dimensions
	int mWidth = 10;
	int mHeight = 10;

	// Scale
	// This value is modified by texureRenderer
	Vector2<float> scale;

	// Other
	static const char* folder;
	std::string path;

	// Methods
	void render(int x, int y, double angle = 0, SDL_Point* center = nullptr, SDL_RendererFlip flip = SDL_FLIP_NONE);

	// Alpha setter/getter
	void setAlpha(Uint8 alpha);
	Uint8 getAlpha();

	// Color Mod
	MapRGB color_key;
	MapRGB color_mod = {255, 255, 255};

	// Checking
	bool isValid();

	// Free resources
	void free();

	static std::unordered_map<std::string, TextureData> textures;

private:
	// Alpha
	Uint8 alpha = SDL_ALPHA_OPAQUE;

	// Methods
	std::string getPathFromResourceFolder(const char* localPath);
	bool load(const char* path, SDL_Renderer *renderer, MapRGB *colorKey = nullptr);
};

