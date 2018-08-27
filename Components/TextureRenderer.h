#pragma once
#include "Component.h"
#include "Texture.h"
#include <string>

class TextureRenderer : public Component
{
public:
	
	TextureRenderer();
	TextureRenderer(Texture texture, Uint8 layer = 127);
	TextureRenderer(const char* path, MapRGB *colorKey = nullptr, Uint8 layer = 127);
	~TextureRenderer();

	// Attributes
	Texture texture;
	std::string tPath;
	Uint8 layer = 0;

	SDL_Renderer *renderer = nullptr;

	// methods
	void update() override;
	void destroy() override;
	void render();

	// Operators
	bool operator < (const TextureRenderer& tRenderer) const
	{
		return (layer < tRenderer.layer);
	}

private:
	SDL_Point* getSDLPointFromVector(Vector2<int> center);
};