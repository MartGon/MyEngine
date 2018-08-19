#pragma once
#include <SDL.h>
#include "MainGameLoop.h"
#include "Vector2.h"

class RendererManager
{
public:
	static Texture nullTexture;
	static SDL_Renderer* renderer;

	static void init();
	static void setNativeResolution(int width, int height);
	static Vector2<float> getScaler();
	static Vector2<int> getNativeResolution();

	// Camera
	static void setCameraPosition(Vector2<int> camera_pos, Vector2<int> level_boundaries);
	static Vector2<int> getCameraPosition();

private:
	static int nativeWidthResolution;
	static int nativeHeightResolution;
	// Camera
	static Vector2<int> camera_position;
};