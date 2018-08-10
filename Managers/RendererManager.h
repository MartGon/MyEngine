#pragma once
#include <SDL.h>
#include "MainGameLoop.h"
#include "Vector2.h"

class RendererManager
{
public:
	static SDL_Renderer* renderer;
	static void setNativeResolution(int width, int height);
	static Vector2<float> getScaler();
	static Vector2<int> getNativeResolution();

private:
	static int nativeWidthResolution;
	static int nativeHeightResolution;
};