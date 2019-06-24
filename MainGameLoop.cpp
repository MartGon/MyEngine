#include "MainGameLoop.h"
#include "RendererManager.h"
#include "SceneManager.h"
#include "AudioManager.h"

Scene *gFirstScene = nullptr;

int WINDOW_WIDTH = 640;
int WINDOW_HEIGHT = 480;

bool initGameWindow(SDL_Window* &window, SDL_Renderer* &renderer, GeneralConfig config)
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

	window = SDL_CreateWindow("Archers Duel", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
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

	SDL_DisplayMode dm;
	SDL_GetCurrentDisplayMode(0, &dm);

	// Enable fullscreen
	if (config.fullscreen)
	{
		// Set size to screen size
		SDL_SetWindowSize(window, dm.w, dm.h);

		// Set to fullscreen
		SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
	}
	else
	{
		// Enable borderless if size equals screen size
		if(dm.w == config.window_w && dm.h == config.window_h)
			SDL_SetWindowFullscreen(window, SDL_WINDOW_BORDERLESS);

		// Set size to screen size
		SDL_SetWindowSize(window, config.window_w, config.window_h);
	}

	// Center
	SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);

	// Get final window size
	int w = 0;
	int h = 0;
	SDL_GetWindowSize(window, &w, &h);

	// SetWindow resolution
	setWindowResolution(w, h);

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

	// Read config file
	GeneralConfig config = Config::readGeneralConfig();

	// Set sound
	AudioManager::sound_enabled = config.sound_enabled;
	AudioManager::volume = config.sound_volume;

	if (!initGameWindow(window, renderer, config))
	{
		printf("Failed to init");
		return 0;
	}
	else
	{
		// Render init
		RendererManager::renderer = renderer;
		RendererManager::window = window;
		RendererManager::init();

		// Scene init
		Scene *scene = gFirstScene;
		gFirstScene->renderer = renderer;
		SceneManager::loadNextScene(scene);
		SceneManager::loadScene();

		// Create quit flag
		bool& quit = SceneManager::quit;

		//While application is running
		bool slowmode = false;
		Uint32 ticks = 0;
		while (!(quit))
		{
			Uint32 now = SDL_GetTicks();
			Uint32 diff = now - ticks;

			if (diff > 15)
			{
				// Set var
				ticks = now;

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
					else
					{
						// Add to scene event list
						SceneManager::scene->event_deque.push_back(e);
					}
				}

				// Update scene
				SceneManager::scene->update();

				// Render frame
				if (SDL_Texture* frame_to_render = SceneManager::scene->frame_to_render)
				{
					SDL_SetRenderTarget(renderer, nullptr);
					SDL_RenderCopy(renderer, frame_to_render, NULL, NULL);
				}

				// Render if frame buffer is enough
				SDL_RenderPresent(renderer);
				SDL_RenderClear(renderer);
			}
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

GeneralConfig Config::readGeneralConfig()
{
	// Create return value
	GeneralConfig config;

	// Open file
	std::ifstream config_file("resources/general-config.txt");

	if (config_file)
	{
		// Read values

		// Fullscreen
		std::string fullscreen_str;
		std::getline(config_file, fullscreen_str);

		std::istringstream(fullscreen_str) >> config.fullscreen;

		// Window w
		std::string window_w;
		std::getline(config_file, window_w);

		config.window_w = std::stoi(window_w);

		// Window h
		std::string window_h;
		std::getline(config_file, window_h);

		config.window_h = std::stoi(window_h);

		// Sound enabled
		std::string sound_str;
		std::getline(config_file, sound_str);

		std::istringstream(sound_str) >> config.sound_enabled;

		// Sound volume
		std::string sound_volume_str;
		std::getline(config_file, sound_volume_str);

		config.sound_volume = std::stoi(sound_volume_str);

		config_file.close();
	}

	return config;
}