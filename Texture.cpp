#include "Texture.h"
#include "RendererManager.h"

const char* Texture::folder = "resources/";

Texture::Texture()
{
	mRenderer = RendererManager::renderer;
	mTexture = RendererManager::nullTexture.mTexture;
}

Texture::Texture(const char* resoucePath) : Texture(resoucePath, RendererManager::renderer)
{

}

Texture::Texture(const char* resourcePath, SDL_Renderer* renderer)
{
	load(resourcePath, renderer);
}

Texture::Texture(const char* resourcePath, SDL_Renderer* renderer, MapRGB *colorKey)
{
	load(resourcePath, renderer, colorKey);
}

Texture::~Texture()
{
	//free();
}

// Methods

void Texture::render(int x, int y, double angle, SDL_Point* center, SDL_RendererFlip flip)
{
	if (!mTexture)
	{
		mTexture = texture->get_value();
		SurfaceResult surface_result = result->get_value();
		mWidth = surface_result.w;
		mHeight = surface_result.h;
		return;
	}

	// Calculate render Quad
	SDL_Rect renderQuad = { x, y, mWidth, mHeight };
	renderQuad.w *= scale.x;
	renderQuad.h *= scale.y;

	// Set alpha value
	SDL_SetTextureAlphaMod(mTexture, alpha);

	// Rotation center is taken the texture width and height as reference
	if (0 > SDL_RenderCopyEx(mRenderer, mTexture, NULL, &renderQuad, angle, center, flip))
	{
		printf("SDL_RenderCopy: %s\n", SDL_GetError());
		return;
	}
}

void Texture::setAlpha(Uint8 alpha)
{
	this->alpha = alpha;
}

Uint8 Texture::getAlpha()
{
	return alpha;
}

void Texture::free()
{
	//printf("Freeing texture %s\n", path);
	if (mTexture)
	{
		SDL_DestroyTexture(mTexture);
		mTexture = NULL;
		mWidth = 0;
		mHeight = 0;
	}
}

std::string Texture::getPathFromResourceFolder(const char* localPath)
{
	std::string strResourcePath(localPath);
	std::string lFolder(folder);
	return lFolder + strResourcePath;
}

bool Texture::isValid()
{
	return mTexture;
}

bool Texture::load(const char* resourcePath, SDL_Renderer *renderer, MapRGB *colorKey)
{
	bool correct = false;
	std::string path = getPathFromResourceFolder(resourcePath).c_str();

	// Set must values
	mRenderer = renderer;

	// Create texture
	SurfaceRequest surface_request{ path, SDL_TRUE, colorKey };
	CreateTextureRequest request{ CREATE_TEXTURE_FROM_SURFACE , texture, surface_request, result, TextureRequest()};
	TextureFactory::create_texture(request);

	/*
	if (texture.get_value())
	{
		printf("Unable to optimize surface from %s! SDL Error : %s \n", path.c_str(), SDL_GetError());
		mTexture = RendererManager::nullTexture.mTexture;
	}
	else
	{
		// Dimensions
		SurfaceResult result_surface = result.get_value();

		mWidth = result_surface.w;
		mHeight = result_surface.h;
		scale = Vector2<float>(1, 1);

		// Flag
		correct = true;
	}
	*/

	return correct;
}