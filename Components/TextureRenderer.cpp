#include "TextureRenderer.h"
#include "GameObject.h"
#include "RendererManager.h"

TextureRenderer::TextureRenderer() : Component()
{
	renderer = RendererManager::renderer;
}

TextureRenderer::TextureRenderer(const char* path) : TextureRenderer() 
{
	texture = Texture(path, RendererManager::renderer);
	tPath = std::string(path);
}

TextureRenderer::TextureRenderer(Texture texture) : Component()
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

	texture.render(x, y);
}