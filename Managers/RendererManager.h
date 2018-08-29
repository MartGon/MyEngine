#pragma once
#include <SDL.h>
#include "MainGameLoop.h"
#include "Vector2.h"
#include "TextureRenderer.h"
#include "Manager.h"

class RendererManager : public Manager<TextureRenderer*>
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
	static bool lesser(const TextureRenderer* tr1, const TextureRenderer* tr2);

	// Camera
	static void setCameraPosition(Vector2<int> camera_pos, Vector2<int> level_boundaries);
	static Vector2<int> getCameraPosition();

	// Non-static methods
	void onAddComponent(TextureRenderer* tRenderer) override;
	void manage();

private:
	static int nativeWidthResolution;
	static int nativeHeightResolution;
	// Camera
	static Vector2<int> camera_position;
};