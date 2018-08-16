#pragma once
#include "Component.h"
#include "Texture.h"
#include <string>

class TextureRenderer : public Component
{
public:
	
	TextureRenderer();
	TextureRenderer(Texture texture);
	TextureRenderer(const char* path, MapRGB *colorKey = nullptr);

	// Attributes
	Texture texture;
	std::string tPath;

	SDL_Renderer *renderer = nullptr;

	// Rotation center
	Vector2<int> *center = nullptr;

	// methods
	void update() override;

private:
	SDL_Point* getSDLPointFromVector(Vector2<int> center);
};