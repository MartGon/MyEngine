#include "RendererManager.h"

Texture RendererManager::nullTexture;

SDL_Renderer* RendererManager::renderer = nullptr;

// GBA Res: 240 x 160

int RendererManager::nativeWidthResolution = 240;
int RendererManager::nativeHeightResolution = 160;

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