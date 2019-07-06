#include "TextureFactory.h"

// Private

Monitor<CreateTextureRequestType> TextureFactory::do_create{CREATE_NONE};
SDL_Texture* TextureFactory::created_texture = nullptr;
TextureRequest TextureFactory::texture_request;
SurfaceRequest TextureFactory::surface_request;
SurfaceResult TextureFactory::surface_result;
SDL_semaphore* TextureFactory::sem = nullptr;

// Public
void TextureFactory::init()
{
	// Create mutex
	sem = SDL_CreateSemaphore(0);
}

int TextureFactory::create_texture(TextureRequest request, SDL_Texture*& texture)
{
	// Activate flag
	do_create.set_value(CREATE_TEXTURE);

	// Set request value
	texture_request = request;

	// Wait for the main thread to create a texture
	SDL_SemWait(sem);

	// Return
	texture = created_texture;

	// Deactivate flag
	do_create.set_value(CREATE_NONE);

	// Return created texture
	return 0;
}

SurfaceResult TextureFactory::create_texture_from_surface(SurfaceRequest request, SDL_Texture*& texture)
{
	// Activate flag
	do_create.set_value(CREATE_TEXTURE_FROM_SURFACE);

	// Set request value
	surface_request = request;

	// Wait for the main thread to create a texture
	SDL_SemWait(sem);

	// Return
	texture = created_texture;

	// Deactivate flag
	do_create.set_value(CREATE_NONE);

	// Return created texture
	return surface_result;
}

void TextureFactory::attend_requests()
{
	// Return if there are no requests
	CreateTextureRequestType type = do_create.get_value();

	if (type == CREATE_NONE)
		return;

	// Create texture
	if (type == CREATE_TEXTURE)
		created_texture = SDL_CreateTexture(RendererManager::renderer, texture_request.format, texture_request.access, texture_request.w, texture_request.h);

	// Create texture from surface
	if (type == CREATE_TEXTURE_FROM_SURFACE)
	{
		if (SDL_Surface* surface = IMG_Load(surface_request.path.c_str()))
		{
			if(surface_request.rgb)
				SDL_SetColorKey(surface, surface_request.flag, SDL_MapRGB(surface->format, surface_request.rgb->red, surface_request.rgb->green, surface_request.rgb->blue));
			created_texture = SDL_CreateTextureFromSurface(RendererManager::renderer, surface);

			surface_result.status = true;
			surface_result.h = surface->h;
			surface_result.w = surface->w;

			SDL_FreeSurface(surface);
		}
	}

	// Unlock mutex
	SDL_SemPost(sem);
}

