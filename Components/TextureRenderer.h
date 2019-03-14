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
	SDL_RendererFlip flip = SDL_FLIP_NONE;
	SDL_Renderer *renderer = nullptr;

	// Specifications
	Vector2<float> render_offset;
	Vector2<float> scale = Vector2<float>(1.f, 1.f);

	// overridden methods
	void update() override;
	void destroy() override;

	// Own methods
	void setLayer(Uint8 layer);
	Uint8 getLayer();
	void render();

private:
	SDL_Point* getSDLPointFromVector(Vector2<int> center);

	// Attributes
	Uint8 layer = 127;
};
