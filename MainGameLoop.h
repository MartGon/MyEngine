#pragma once
#include "Texture.h"

#include <fstream>

struct GeneralConfig
{
	bool fullscreen = false;
	int window_w = 960;
	int window_h = 520;
	bool sound_enabled = false;
	int sound_volume = 100;
};

namespace Config
{
	GeneralConfig readGeneralConfig();
	bool is_empty(std::ifstream& pFile);
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
