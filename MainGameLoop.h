#pragma once
#include "Texture.h"

// Constants
extern int WINDOW_WIDTH;
extern int WINDOW_HEIGHT;

// Var
class Scene;
extern Scene *gFirstScene;

void setWindowResolution(int width, int height);
bool initGameWindow(SDL_Window* &window, SDL_Renderer* &renderer);

void closeGame();

int engine_main();


