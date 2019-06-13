#include "MainGameLoop.h"
#include "RendererManager.h"
#include "SceneManager.h"

Scene *gFirstScene = nullptr;

int WINDOW_WIDTH = 640;
int WINDOW_HEIGHT = 480;

bool initGameWindow(SDL_Window* &window, SDL_Renderer* &renderer)
{
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_AUDIO))
	{
		printf("Could not initialize SDL Video | SDL Timer, Error: %s \n", SDL_GetError());
		return false;
	}

	int imgFlags = IMG_INIT_PNG; 
	if (!(IMG_Init(imgFlags) & imgFlags)) 
	{ 
		printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError()); 
		return false;
	}

	window = SDL_CreateWindow("SDL-Pong", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
	if (!window)
	{
		printf("Window could not be created! SDL Error: %s \n", SDL_GetError());
		return false;
	}

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (!renderer)
	{
		printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
		return false;
	}

	return true;
}

void setWindowResolution(int width, int height)
{
	WINDOW_WIDTH = width;
	WINDOW_HEIGHT = height;
}

int engine_main()
{
	SDL_Window *window = NULL;
	SDL_Renderer *renderer = NULL;

	if (!initGameWindow(window, renderer))
	{
		printf("Failed to init");
		return 0;
	}
	else
	{
		// Render init
		RendererManager::renderer = renderer;
		RendererManager::init();

		// Scene init
		Scene *scene = gFirstScene;
		gFirstScene->renderer = renderer;
		SceneManager::loadNextScene(scene);
		SceneManager::loadScene();

		// Create quit flag
		bool& quit = SceneManager::quit;

		//While application is running
		SDL_Texture* frame_to_render = nullptr;
		bool slowmode = false;
		while (!(quit))
		{
			// Check for next scene
			if (SceneManager::canLoadNextScene())
				SceneManager::loadScene();

			//Handle events on queue
			SDL_Event e;
			while (SDL_PollEvent(&e) != 0)
			{
				//User requests quit
				if (e.type == SDL_QUIT)
				{
					quit = true;
				}
				if(e.type == SDL_KEYDOWN)
				{
					if (e.key.keysym.sym == SDLK_p)
					{
						//slowmode = !slowmode;
					}
				}
				else
				{
					// Add to scene event list
					SceneManager::scene->event_deque.push_back(e);
				}
			}

			// Update scene
			SceneManager::scene->update();

			// Don't render while waiting for connection
			if (SceneManager::scene->isOnline())
			{
				if (!SceneManager::scene->connectionEstablished)
					continue;
			}

			// Render if frame buffer is enough
			SDL_RenderPresent(renderer);
			SDL_RenderClear(renderer);

			if(slowmode)
				SDL_Delay(1000);
		}
	}

	return 0;
}

void closeGame()
{
	// Free all textures

	/*Destroy window	
	SDL_DestroyRenderer(*renderer);
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;
	gRenderer = NULL;
	*/

	//Quit SDL subsystems
	IMG_Quit();
	SDL_Quit();
}