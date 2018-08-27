#pragma once
#include <SDL.h>
#include "MainGameLoop.h"
#include "Vector2.h"
#include "TextureRenderer.h"

class RendererManager
{
public:
	// Constructor
	RendererManager();

	// Attributes
	static Texture nullTexture;
	static SDL_Renderer* renderer;

	// Methods
	static void init();
	static void setNativeResolution(int width, int height);
	static Vector2<float> getScaler();
	static Vector2<int> getNativeResolution();

	// Camera
	static void setCameraPosition(Vector2<int> camera_pos, Vector2<int> level_boundaries);
	static Vector2<int> getCameraPosition();

	// Non-static methods
	void addTextureRenderer(TextureRenderer* tRenderer);
	void removeTextureRenderer(TextureRenderer* tRenderer);
	void manage();

private:
	static int nativeWidthResolution;
	static int nativeHeightResolution;
	// Camera
	static Vector2<int> camera_position;

	// Object dependent
	std::vector<TextureRenderer*> texture_renderers;
};