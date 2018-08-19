#include "TextureRenderer.h"
#include "GameObject.h"
#include "RendererManager.h"

TextureRenderer::TextureRenderer() : Component()
{
	renderer = RendererManager::renderer;
}

TextureRenderer::TextureRenderer(const char* path, MapRGB *colorKey) : TextureRenderer() 
{
	texture = Texture(path, RendererManager::renderer, colorKey);
	tPath = std::string(path);
}

TextureRenderer::TextureRenderer(Texture texture) : TextureRenderer()
{
	this->texture = texture;
	tPath = std::string(texture.path);
}

void TextureRenderer::update()
{
	Vector2<float> scaler = RendererManager::getScaler();

	int x = gameObject->transform.position.x * scaler.x;
	int y = gameObject->transform.position.y * scaler.y;

	texture.scale = gameObject->transform.scale * scaler;

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

	x -= cam_pos.x * scaler.x;
	y -= cam_pos.y * scaler.y;

	texture.render(x, y, angle, fCenter);
}

SDL_Point* TextureRenderer::getSDLPointFromVector(Vector2<int> center)
{
	SDL_Point* point = new SDL_Point();
	point->x = center.x;
	point->y = center.y;

	return point;
}