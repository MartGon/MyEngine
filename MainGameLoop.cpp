#include "MainGameLoop.h"
#include "RendererManager.h"
#include "SceneManager.h"
#include "Monitor.h"
#include "TextureFactory.h"
#include <deque>


// Thread
Monitor<std::deque<SDL_Event>> events_queue{ std::deque<SDL_Event>() };
SDL_semaphore* sem = nullptr;
SDL_Thread* thread = nullptr;

Scene *gFirstScene = nullptr;
Scene* scene = nullptr;

int WINDOW_WIDTH = 640;
int WINDOW_HEIGHT = 480;

bool initGameWindow(SDL_Window* &window, SDL_Renderer* &renderer)
{
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER))
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

	window = SDL_CreateWindow("ArcherDuel", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
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

// Thread that will block for packets
int logic_thread_manager(void* data)
{
	// Scene init
	SceneManager::loadNextScene(scene);
	SceneManager::loadScene();

	// Loop
	while (true)
	{
		// Update scene
		SDL_SemWait(sem);

		// Check for next scene
		if (SceneManager::canLoadNextScene())
			SceneManager::loadScene();

		SceneManager::scene->update();

		// Handle Events
		std::deque<SDL_Event> events = events_queue.get_value();
		while (!events.empty())
		{
			SDL_Event event = events.back();
			SceneManager::scene->handleEvent(event);
			events.pop_back();
		}

		// Reset events queue
		events_queue.set_value(events);

		SDL_SemPost(sem);

		// Keep the same calculation framerate
		SDL_Delay(15);
	}

	return 0;
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
		// Renderer init
		RendererManager::renderer = renderer;
		RendererManager::init();

		// Init TextureFactory
		TextureFactory::init();

		// Scene init
		scene = gFirstScene;
		gFirstScene->renderer = renderer;

		// Create logic thread
		thread = SDL_CreateThread(logic_thread_manager, "render_thread", nullptr);
		SDL_DetachThread(thread);

		// Rendering frame
		sem = SDL_CreateSemaphore(1);
		while (true)
		{
			// Attend request
			TextureFactory::attend_requests();

			// Check fot exit
			// Get events from queue
			SDL_Event e;
			while (SDL_PollEvent(&e) != 0)
			{
				//User requests quit
				if (e.type == SDL_QUIT)
				{
					return 0;
				}
				// Add events to the queue
				else
				{
					auto events = events_queue.get_value();
					events.push_front(e);
					events_queue.set_value(events);
				}
			}

			// Calculate one frame
			SDL_SemWait(sem);
			if(RendererManager* renderer_manager = dynamic_cast<RendererManager*>(scene->getManager<TextureRenderer*>()))
				renderer_manager->manage();
			SDL_SemPost(sem);

			// Render if frame buffer is enough
			if (RendererManager::frame_buffer.size() >= 1)
			{
				SDL_SetRenderTarget(renderer, nullptr);
				SDL_RenderCopy(renderer, RendererManager::frame_buffer.back(), NULL, NULL);
				RendererManager::frame_buffer.pop_back();
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