#pragma once
#include "Component.h"
#include "Texture.h"

#include <string>
#include <deque>
#include <optional>

struct RenderData
{
	Vector2<float> pos;
	std::optional<SDL_Point> r_center;
	double angle;
};

class TextureRenderer : public Component
{
public:
	
	TextureRenderer();
	TextureRenderer(Texture texture, Uint8 layer = 127);
	TextureRenderer(const char* path, MapRGB *colorKey = nullptr, Uint8 layer = 127);
	~TextureRenderer();

	// Attributes
	Uint8 alpha = 255;
	Texture texture;
	std::string tPath;
	SDL_RendererFlip flip = SDL_FLIP_NONE;
	SDL_Renderer *renderer = nullptr;

	// Misc
	int trail_size = 5;
	bool isVanishing = false;
	bool hasTrailEffect = false;
	
	void setBlink(int framerate, int duration);
	void unsetBlink();

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
	SDL_Point getSDLPointFromVector(Vector2<int> center);

	// Attributes
	Uint8 layer = 127;

	// Misc
	bool isBlinking = false;
	int blink_frame_duration = 0;
	int blink_rate = 2;
	int blink_frame_count = 0;

	std::deque<RenderData> trail_pos;

	// Own methods
	void vanish();
	void blink();
	void update_trail(Vector2<float> pos, std::optional<SDL_Point> r_center, double angle);
	void render_trail();
};
