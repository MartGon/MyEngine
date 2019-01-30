#include "TextureRenderer.h"
#include "GameObject.h"
#include "RendererManager.h"
#include "SceneManager.h"

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
	Vector2<float> pos = gameObject->getAbsolutePosition();

	int x = pos.x * scaler.x;
	int y = pos.y * scaler.y;

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

	x -= (cam_pos.x * scaler.x);
	y -= (cam_pos.y * scaler.y);

	texture.render(x, y, angle, fCenter, flip);
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

}

void TextureRenderer::destroy()
{
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