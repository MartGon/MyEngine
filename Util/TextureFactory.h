#pragma once
#include <SDL.h>
#include "Monitor.h"
#include "RendererManager.h"

enum CreateTextureRequestType
{
	CREATE_NONE,
	CREATE_TEXTURE,
	CREATE_TEXTURE_FROM_SURFACE
};

struct SurfaceRequest
{
	std::string path;
	int flag = 0;
	MapRGB* rgb = nullptr;
};

struct SurfaceResult
{
	bool status = false;
	int w = 0;
	int h = 0;
};

struct TextureRequest
{
	Uint32 format = 0;
	int access = 0;
	int w = 0;
	int h = 0;
};

class TextureFactory
{
public:
	// Init factory
	static void init();

	// Main method to get a texture, blocks until the texture is available
	static int create_texture(TextureRequest request, SDL_Texture*& texture);

	// Maint mehtod to get a texture from a surface
	static SurfaceResult create_texture_from_surface(SurfaceRequest request, SDL_Texture*& texture);

	// Method to check by main thread
	static void attend_requests();
private:


	// Create texture flag to know if a texture has to be created
	static Monitor<CreateTextureRequestType> do_create;

	// Texture to return after a request is made
	static SDL_Texture* created_texture;

	// Request texture params
	static TextureRequest texture_request;

	// Set request surface
	static SurfaceRequest surface_request;
	static SurfaceResult surface_result;

	// Mutex to activate return of values
	static SDL_semaphore* sem;
};