#include "TextureRenderer.h"
#include "GameObject.h"
#include "RendererManager.h"
#include "SceneManager.h"

// TODO - Needs to be added to the manager
TextureRenderer::TextureRenderer() : Component()
{
	renderer = RendererManager::renderer;
}

TextureRenderer::TextureRenderer(const char* path, MapRGB *colorKey, Uint8 layer) : TextureRenderer(Texture(path, RendererManager::renderer, colorKey), layer)
{

}

TextureRenderer::TextureRenderer(Texture texture, Uint8 layer) : TextureRenderer()
{
	this->texture = texture;
	this->layer = layer;
	tPath = std::string(texture.path);

	// Add to renderer list
	SceneManager::scene->addComponentToManager(this);
}

TextureRenderer::~TextureRenderer()
{
	destroy();
}

void TextureRenderer::render()
{
	Vector2<float> scaler = RendererManager::getScaler();
	Vector2<float> pos = gameObject->getAbsolutePosition() + render_offset;

	int x = pos.x * scaler.x;
	int y = pos.y * scaler.y;

	texture.scale = gameObject->transform.scale * scaler * scale;

	// Rotation stuff
	double angle = gameObject->transform.zRotation;

	SDL_Point* fCenter = NULL;

	// If center is not set, pass NULL so width/2, height/2 is taken
	if (Vector2<int>* center = gameObject->transform.rotationCenter)
	{
		Vector2<int> cCenter;
		cCenter.x = center->x * scaler.x;
		cCenter.y = center->y * scaler.y;

		fCenter = getSDLPointFromVector(cCenter);
	}

	// Camera stuff
	// TODO - Check if texture has to be rendered (inside camera)
	Vector2<int> cam_pos = RendererManager::getCameraPosition();

	x -= (cam_pos.x * scaler.x);
	y -= (cam_pos.y * scaler.y);

	// Render trail effect
	if (hasTrailEffect)
		render_trail();

	// Texture set alpha
	texture.setAlpha(alpha);

	texture.render(x, y, angle, fCenter, flip);

	// Update trail
	if (hasTrailEffect)
		update_trail(Vector2<float>(x, y), fCenter, angle);
}

SDL_Point* TextureRenderer::getSDLPointFromVector(Vector2<int> center)
{
	SDL_Point* point = new SDL_Point();
	point->x = center.x;
	point->y = center.y;

	return point;
}

// overridden methods

void TextureRenderer::update()
{
	if(isVanishing)
		vanish();
	
	if (isBlinking)
		blink();
}

void TextureRenderer::destroy()
{
	//texture.free();
	SceneManager::scene->removeComponentFromManager(this);
}

// own methods
 
Uint8 TextureRenderer::getLayer()
{
	return layer;
}

void TextureRenderer::setLayer(Uint8 layer)
{
	this->layer = layer;

	// Re-order the array
	Manager<TextureRenderer*>* manager = SceneManager::scene->getManager<TextureRenderer*>();

	if (manager)
		manager->onAddComponent(this);
}

void TextureRenderer::vanish()
{
	if (!alpha)
	{
		if (gameObject)
			gameObject->onVanish();
	}
	else
		texture.setAlpha(alpha --);
}

// Misc

void TextureRenderer::setBlink(int framerate, int duration)
{
	blink_rate = framerate;
	blink_frame_duration = duration;
	isBlinking = true;
}

void TextureRenderer::unsetBlink()
{
	alpha = SDL_ALPHA_OPAQUE;
	isBlinking = false;
	blink_frame_count = 0;
}

void TextureRenderer::blink()
{
	// Check if blink has finished
	blink_frame_duration--;
	if (!blink_frame_duration)
	{
		unsetBlink();

		if (GameObject* go = gameObject)
			go->onBlinkFinish();

		return;
	}

	// Check if alpha should be changed
	blink_frame_count = (blink_frame_count + 1) % blink_rate;
	bool blink = blink_frame_count == 0;

	if (!blink)
		return;

	// Update alpha value
	if (alpha == SDL_ALPHA_OPAQUE)
		alpha = SDL_ALPHA_TRANSPARENT;
	else
		alpha = SDL_ALPHA_OPAQUE;
}

void TextureRenderer::update_trail(Vector2<float> pos, SDL_Point* r_center, double angle)
{
	RenderData r_data = { pos, r_center, angle };

	// Remove first one when too big
	if (trail_pos.size() > trail_size)
		trail_pos.pop_back();

	// Get pos and add to the list
	trail_pos.push_front(r_data);
}

void TextureRenderer::render_trail()
{
	for (int i = 0; i < trail_pos.size(); i ++)
	{
		RenderData r_data = trail_pos[i];

		// Texture set alpha
		Uint8 render_alpha = alpha / (i + 1);
		texture.setAlpha(render_alpha);

		// Fill render data
		int x = r_data.pos.x;
		int y = r_data.pos.y;
		double angle = r_data.angle;
		SDL_Point* fCenter = r_data.r_center;

		texture.render(x, y, angle, fCenter, flip);
	}
}