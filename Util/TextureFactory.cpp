#include "TextureFactory.h"
#include "RendererManager.h"

// Private
Monitor<std::deque<CreateTextureRequest>> TextureFactory::create_texture_requests{ std::deque<CreateTextureRequest>() };

// Public
void TextureFactory::init()
{
}

int TextureFactory::create_texture(CreateTextureRequest request)
{
	// Activate flag
	add_request(request);

	// Return created texture
	return 0;
}

void TextureFactory::attend_requests()
{
	// Get request and block
	create_texture_requests.lock();
	std::deque<CreateTextureRequest> requests = create_texture_requests.get_value();

	while (!requests.empty())
	{
		CreateTextureRequest request = requests.back();
		requests.pop_back();

		if (request.type == CREATE_NONE)
			return;

		// Create texture
		if (request.type == CREATE_TEXTURE)
		{
			TextureRequest texture_request = request.texture_requset;
			SDL_Texture* texture = SDL_CreateTexture(RendererManager::renderer, texture_request.format, texture_request.access, texture_request.w, texture_request.h);
			
			// Set Response
			request.texture->set_value(texture);
		}
		// Create texture from surface
		if (request.type == CREATE_TEXTURE_FROM_SURFACE)
		{
			SurfaceRequest surface_request = request.surface_request;
			SurfaceResult surface_result;

			if (SDL_Surface* surface = IMG_Load(surface_request.path.c_str()))
			{
				if (surface_request.rgb)
					SDL_SetColorKey(surface, surface_request.flag, SDL_MapRGB(surface->format, surface_request.rgb->red, surface_request.rgb->green, surface_request.rgb->blue));
				SDL_Texture* texture = SDL_CreateTextureFromSurface(RendererManager::renderer, surface);

				surface_result.status = true;
				surface_result.h = surface->h;
				surface_result.w = surface->w;

				SDL_FreeSurface(surface);

				// Set Response
				request.texture->set_value(texture);
				request.surface_result->set_value(surface_result);
			}
		}

	}

	// Empty requests and unlock
	create_texture_requests.set_value(requests);
	create_texture_requests.unlock();
}

void TextureFactory::add_request(CreateTextureRequest& request)
{
	std::deque<CreateTextureRequest> requests = create_texture_requests.get_value();
	requests.push_front(request);
	create_texture_requests.set_value(requests);
}