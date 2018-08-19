#include "RendererManager.h"
#include "SceneManager.h"

Texture RendererManager::nullTexture;

SDL_Renderer* RendererManager::renderer = nullptr;

// GBA Res: 240 x 160

int RendererManager::nativeWidthResolution = 240;
int RendererManager::nativeHeightResolution = 160;

// Cam position
Vector2<int> RendererManager::camera_position = Vector2<int>(120, 80);

void RendererManager::setNativeResolution(int width, int height)
{
	nativeHeightResolution = width;
	nativeWidthResolution = height;
}

Vector2<float> RendererManager::getScaler()
{
	Vector2<float> scaler(float(WINDOW_WIDTH) / float(nativeWidthResolution), float(WINDOW_HEIGHT) / float(nativeHeightResolution));
	return scaler;
}

Vector2<int> RendererManager::getNativeResolution()
{
	return Vector2<int>(nativeWidthResolution, nativeHeightResolution);
}

void RendererManager::init()
{
	if (!renderer)
	{
		printf("Renderer was not set before initialization\n");
		return;
	}

	nullTexture = Texture("null.png", renderer);
}

void RendererManager::setCameraPosition(Vector2<int> camera_pos, Vector2<int> level_boundaries)
{
	if (camera_pos.x < 0)
		camera_position.x = 0;
	else if (camera_pos.x > (level_boundaries.x - nativeWidthResolution))
		camera_position.x = (level_boundaries.x - nativeWidthResolution);
	else
		camera_position.x = camera_pos.x;

	if (camera_pos.y < 0)
		camera_position.y = 0;
	else if (camera_pos.y > (level_boundaries.y - nativeHeightResolution))
		camera_position.y = (level_boundaries.y - nativeHeightResolution);
	else
		camera_position.y = camera_pos.y;

	// Set
}

Vector2<int> RendererManager::getCameraPosition()
{
	return camera_position;
}