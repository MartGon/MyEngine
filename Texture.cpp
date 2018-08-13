#include "Texture.h"
#include "RendererManager.h"

const char* Texture::folder = "resources/";

Texture::Texture()
{
	mRenderer = RendererManager::renderer;
	mTexture = RendererManager::nullTexture.mTexture;
}

Texture::Texture(const char* resourcePath, SDL_Surface* screenSurface)
{
	path = getPathFromResourceFolder(resourcePath).c_str();
	SDL_Surface *imgSurface = IMG_Load(path);
	if (!imgSurface)
	{
		printf("Unable to load png file from %s! SDL Error: %s \n", path, SDL_GetError());
	}
	else
	{
		optimizedSurface = SDL_ConvertSurface(imgSurface, screenSurface->format, NULL);

		if (!optimizedSurface)
		{
			printf("Unable to optimize surface from %s! SDL Error : %s \n", path, SDL_GetError());
		}

		SDL_FreeSurface(imgSurface);
	}
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

void Texture::render(int x, int y)
{
	SDL_Rect renderQuad = { x, y, mWidth, mHeight };
	renderQuad.w *= scale.x;
	renderQuad.h *= scale.y;

	if (0 > SDL_RenderCopy(mRenderer, mTexture, NULL, &renderQuad))
	{
		printf("SDL_RenderCopy: %s\n", SDL_GetError());
		return;
	}
}

void Texture::setAlpha(Uint8 alpha)
{
	SDL_SetTextureAlphaMod(mTexture, alpha);
}

Uint8 Texture::getAlpha()
{
	Uint8 alpha;
	SDL_GetTextureAlphaMod(mTexture, &alpha);

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
	std::string tempRet = getPathFromResourceFolder(resourcePath).c_str();

	// Set must values
	path = tempRet.c_str();
	mRenderer = renderer;

	SDL_Surface *imgSurface = IMG_Load(path);
	if (!imgSurface)
	{
		printf("Unable to load png file from %s! SDL Error: %s \n", path, SDL_GetError());
		mTexture = RendererManager::nullTexture.mTexture;
	}
	else
	{
		if (colorKey)
		{
			SDL_SetColorKey(imgSurface, SDL_TRUE, SDL_MapRGB(imgSurface->format, colorKey->red, colorKey->green, colorKey->blue));
		}

		mTexture = SDL_CreateTextureFromSurface(renderer, imgSurface);

		if (!mTexture)
		{
			printf("Unable to optimize surface from %s! SDL Error : %s \n", path, SDL_GetError());
			mTexture = RendererManager::nullTexture.mTexture;
		}
		else
		{
			// Dimensions
			mWidth = imgSurface->w;
			mHeight = imgSurface->h;
			scale = Vector2<float>(1, 1);

			// Flag
			correct = true;
		}

		SDL_FreeSurface(imgSurface);
	}

	return correct;
}