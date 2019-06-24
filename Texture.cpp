#include "Texture.h"
#include "RendererManager.h"

const char* Texture::folder = "resources/textures/";
std::unordered_map<std::string, TextureData> Texture::textures;

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
	// Try to load texture if we couldn't
	if (!mTexture)
	{
		if (!imgSurface)
		{
			imgSurface = IMG_Load(path.c_str());

			if (!imgSurface)
				imgSurface = nullptr;
		}
		
		if (imgSurface)
		{
			SDL_SetColorKey(imgSurface, SDL_TRUE, SDL_MapRGB(imgSurface->format, color_key.red, color_key.green, color_key.blue));
			mTexture = SDL_CreateTextureFromSurface(mRenderer, imgSurface);
		}

		if (mTexture)
		{
			// Add to repository
			TextureData data{ mTexture, mWidth, mHeight };
			textures.insert({ path, data });

			// Free surface
			SDL_FreeSurface(imgSurface);
			imgSurface = nullptr;
		}
	}

	if (mTexture)
	{
		// Calculate render Quad
		SDL_Rect renderQuad = { x, y, mWidth, mHeight };
		renderQuad.w *= scale.x;
		renderQuad.h *= scale.y;

		// Set alpha value
		SDL_SetTextureAlphaMod(mTexture, alpha);

		// Set color mod
		SDL_SetTextureColorMod(mTexture, color_mod.red, color_mod.green, color_mod.blue);

		// Rotation center is taken the texture width and height as reference
		if (0 > SDL_RenderCopyEx(mRenderer, mTexture, NULL, &renderQuad, angle, center, flip))
		{
			printf("SDL_RenderCopy: %s\n", SDL_GetError());
			return;
		}
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

	if (textures.find(path) != textures.end())
	{
		if (mTexture)
		{
			SDL_DestroyTexture(mTexture);
			mTexture = NULL;
		}

		textures.erase(path);
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

	// Set path
	path = tempRet.c_str();

	auto texture = textures.find(path);
	if (texture == textures.end())
	{
		mRenderer = renderer;

		imgSurface = IMG_Load(path.c_str());
		if (!imgSurface)
		{
			printf("Unable to load png file from %s! SDL Error: %s \n", path, SDL_GetError());
			mTexture = RendererManager::nullTexture.mTexture;
		}
		else
		{
			if (colorKey)
			{
				color_key = *colorKey;
				SDL_SetColorKey(imgSurface, SDL_TRUE, SDL_MapRGB(imgSurface->format, colorKey->red, colorKey->green, colorKey->blue));
			}

			// Dimensions
			mWidth = imgSurface->w;
			mHeight = imgSurface->h;
			scale = Vector2<float>(1, 1);

			mTexture = SDL_CreateTextureFromSurface(renderer, imgSurface);

			if (mTexture)
			{
				// Free texture
				SDL_FreeSurface(imgSurface);
				imgSurface = nullptr;

				// Add to repository
				TextureData data{ mTexture, mWidth, mHeight };
				textures.insert({ path, data });
			}
			else
			{
				printf("Unable to optimize surface from %s! SDL Error : %s \n", path, SDL_GetError());
				SDL_FreeSurface(imgSurface);
				imgSurface = nullptr;
			}
				
		}
	}
	else
	{
		// Load texture form repository
		mRenderer = renderer;
		TextureData data = texture->second;
		mTexture = data.texture;
		mWidth = data.w;
		mHeight = data.h;
		scale = Vector2<float>(1, 1);
		correct = true;
	}

	return correct;
}