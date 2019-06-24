#pragma once
#include "Texture.h"

struct GeneralConfig
{
	bool fullscreen = false;
	int window_w = 0;
	int window_h = 0;
	bool sound_enabled = false;
	int sound_volume = 0;
};

namespace Config
{
	GeneralConfig readGeneralConfig();
}

// Constants
extern int WINDOW_WIDTH;
extern int WINDOW_HEIGHT;

// Var
class Scene;
extern Scene *gFirstScene;

void setWindowResolution(int width, int height);
bool initGameWindow(SDL_Window* &window, SDL_Renderer* &renderer, GeneralConfig config);

void closeGame();

int engine_main();
