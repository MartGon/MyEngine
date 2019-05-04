#pragma once
#include <SDL.h>
#include "Monitor.h"
#include <deque>

struct MapRGB;

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

struct CreateTextureRequest
{
	CreateTextureRequestType type = CREATE_NONE;
	Monitor<SDL_Texture*>* texture;
	SurfaceRequest surface_request;
	Monitor<SurfaceResult>* surface_result;
	TextureRequest texture_requset;
};

class TextureFactory
{
public:
	// Init factory
	static void init();

	// Main method to get a texture, blocks until the texture is available
	static int create_texture(CreateTextureRequest request);

	// Method to check by main thread
	static void attend_requests();
private:

	// List of requests
	static Monitor<std::deque<CreateTextureRequest>> create_texture_requests;

	// Methods
	static void add_request(CreateTextureRequest& request);
};